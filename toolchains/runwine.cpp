#if 0

input="$(readlink -f "$(command -v "$0")")"
output="${0%.*}"

#// Compile ourselves
if [ "$input" -nt "$output" ] ; then
	printf 'Compiling %s...\n' "${output##*/}" >&2
	${CXX:-g++} -std=c++11 -Wall -Wextra "$input" -o "$output" > /dev/null < /dev/null || exit 1
fi

#// Run the executable
exec "$output" "$@"

exit

#endif // 0

/*
 * Copyright (C) 2014 Daniel Scharrer
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the author(s) be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <algorithm>

#include <cstdio>

#include <fcntl.h>
#include <signal.h>
#include <spawn.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#include <sys/select.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char ** environ;

#define error(err, ...) std::fprintf(stderr, "[runwine] " err "\n", ## __VA_ARGS__);

/*!
 * Wine sometimes leaves stdin/stdout file descriptors open indefinitely in wineserver or
 * other helper processes after the original executable has exited.
 * This can cause CMake to hang during the configure step.
 * Even starting a persistent wineserver beforehand does not solve this!
 * This utility will put a pipe between Wine and CMake and immediately closes the original
 * stdout and stderr file descriptors once the child hast exited.
 * It also removes all carriage return bytes form stdout and stderr.
 */

int main(int argc, char ** argv) {
	
	if(argc < 2) {
		error("Usage: runwine [command]");
		return 1;
	}
	
	int r;
	constexpr int npipes = 2;
	
	int pipes[npipes][2];
	constexpr int fds[npipes] = { 1, 2 };
	r = 0;
	for(int i = 0; i < npipes; i++) {
		if(fcntl(fds[i], F_GETFD) >= 0) {
			r = r || pipe(pipes[i]);
			r = r || fcntl(pipes[i][0], F_SETFL, fcntl(pipes[i][0], F_GETFL, 0) | O_NONBLOCK);
		} else {
			pipes[i][0] = -1;
			pipes[i][1] = -1;
		}
	}
	if(r) {
		error("Could not create pipes");
		return 1;
	}
	
	sigset_t mask;
	if(sigemptyset(&mask) || sigaddset(&mask, SIGCHLD)) {
		error("Could not construct signal set");
		return 1;
	}
	int signal = signalfd(-1, &mask, O_NONBLOCK | SFD_CLOEXEC);
	if(signal < 0) {
		error("Could not construct signal fd");
		return 1;
	}
	
	sigset_t oldmask;
	if(sigprocmask(SIG_BLOCK, &mask, &oldmask)) {
		error("Could not set signal mask");
		return 1;
	}
	
	posix_spawn_file_actions_t file_actions;
	r = posix_spawn_file_actions_init(&file_actions);
	r = r || posix_spawn_file_actions_addclose(&file_actions, 0);
	for(int i = 0; i < npipes; i++) {
		if(pipes[i][0] >= 0) {
			r = r || posix_spawn_file_actions_adddup2(&file_actions, pipes[i][1], fds[i]);
			r = r || posix_spawn_file_actions_addclose(&file_actions, pipes[i][0]);
			r = r || posix_spawn_file_actions_addclose(&file_actions, pipes[i][1]);
		}
	}
	if(r) {
		error("Could not create spawn file actions struct");
		return 1;
	}
	
	posix_spawnattr_t attr;
	r = posix_spawnattr_init(&attr);
	r = r || posix_spawnattr_setsigmask(&attr, &oldmask);
	if(r) {
		error("Could not create spawn attributes struct");
		return 1;
	}
	
	pid_t pid = -1;
	char ** vargs = argv + 1;
	if(posix_spawnp(&pid, vargs[0], &file_actions, &attr, vargs, environ) || pid <= 0) {
		error("Could not start %s process", vargs[0]);
		return 1;
	}
	
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(signal, &readfds);
	int nfds = signal;
	for(int i = 0; i < npipes; i++) {
		if(pipes[i][0] >= 0) {
			FD_SET(pipes[i][0], &readfds);
			nfds = std::max(nfds, pipes[i][0]);
			close(pipes[i][1]);
		}
	}
	
	while(true) {
		
		int r = select(nfds + 1, &readfds, nullptr, nullptr, nullptr);
		if(r < 0) {
			error("Select failed")
			return 1;
		}
		
		bool exited = false;
		int status = -1;
		if(FD_ISSET(signal, &readfds)) {
			while(true) {
				struct signalfd_siginfo siginfo;
				ssize_t nread = read(signal, &siginfo, sizeof(siginfo));
				if(nread <= 0) {
					break;
				}
				if(siginfo.ssi_signo == SIGCHLD) {
					if(waitpid(pid, &status, WNOHANG) == pid) {
						if(WIFEXITED(status) || WIFSIGNALED(status)) {
							/*
							 * Child has exited!
							 * Perform one last non-blocking read on each pipe and then exit,
							 * closing the original stdout/stderr.
							 */
							exited = true;
							break;
						}
					}
				}
			}
		} else {
			FD_SET(signal, &readfds);
		}
		
		for(int i = 0; i < npipes; i++) {
			
			if(pipes[i][0] < 0) {
				continue;
			}
			
			if(exited || FD_ISSET(pipes[i][0], &readfds)) {
				
				do {
					
					// Read as much as we can from the pipe
					char buffer[4096];
					ssize_t nread = read(pipes[i][0], buffer, sizeof(buffer));
					if(nread <= 0) {
						if(nread == -1 && errno != EAGAIN && errno != EINTR) {
							// Input is broken, close output
							if(errno != EBADF) {
								error("Read error on pipe %d (%d): %d", i, pipes[i][0], errno);
							}
							FD_CLR(pipes[i][0], &readfds);
							close(fds[i]);
							pipes[i][0] = -1;
						}
						break;
					}
					
					// Remove pesky bytes
					nread = std::remove(buffer, buffer + nread, '\r') - buffer;
					
					// Write the whole buffer to stdout / stderr
					const char * p = buffer;
					while(nread > 0) {
						ssize_t nwritten = write(fds[i], p, nread);
						if(nwritten <= 0 && errno != EINTR) {
							if(errno == EAGAIN) {
								fcntl(fds[i], F_SETFL, fcntl(fds[i], F_GETFL, 0) & ~O_NONBLOCK);
							} else {
								// Output is broken, close input
								if(errno != EBADF && errno != EPIPE) {
									error("Write error on pipe %d (%d): %d", i, fds[i], errno);
								}
								FD_CLR(pipes[i][0], &readfds);
								close(pipes[i][0]);
								pipes[i][0] = -1;
								break;
							}
						}
						if(nwritten > 0) {
							nread -= nwritten;
							p += nwritten;
						}
					}
					
				} while(pipes[i][0] >= 0);
				
			} else {
				FD_SET(pipes[i][0], &readfds);
			}
			
		}
		
		if(exited) {
			if(WIFSIGNALED(status)) {
				error("Received signal %d", WTERMSIG(status));
				return 1;
			} else {
				return WEXITSTATUS(status);
			}
		}
		
	}
	
}
