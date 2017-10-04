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
 * Copyright (C) 2017 Daniel Scharrer
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

#include <string>
#include <vector>
#include <map>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multi_array.hpp>
#include <boost/range/adaptor/reversed.hpp>

struct warning {
	
	// Original warning line
	std::string text;
	
	// Parsed warning information
	std::size_t line;
	std::size_t pos;
	std::string message;
	
	// Corresponding code lines
	std::string prev;
	std::string code;
	std::string next;
	
};

typedef std::vector<warning> warnings;

typedef std::map<std::string, warnings> parsed_warnings;

typedef std::map<std::string, std::pair<warnings, warnings>> matched_warnings;

std::string exec(const std::string & cmd) {
	std::string result;
	FILE * pipe = popen(cmd.c_str(), "r");
	if(pipe) {
		while(!feof(pipe)) {
			char buffer[2014];
			size_t count = fread(buffer, 1, sizeof(buffer), pipe);
			result.append(buffer, count);
		}
		pclose(pipe);
	}
	return result;
}

std::string escape(const std::string & str) {
	return str; // TODO
}

std::vector<std::string> git_get_file(const std::string & repo, std::string & filename,
                                      const std::string & commit) {
	
	std::vector<std::string> result;
	
	std::string contents = exec("git --git-dir=" + escape(repo) + " show "
	                            + escape(commit) + ":" + escape(filename));
	
	if(!contents.empty()) {
		std::istringstream iss(contents);
		std::string line;
		while(std::getline(iss, line)) {
			result.push_back(line);
		}
	}
	
	return result;
}

std::string git_get_old_filename(const std::string & repo, std::string & filename,
                                 const std::string & old_commit, const std::string & commit) {
	
	std::string result = exec("git --git-dir=" + escape(repo) + " log --oneline --name-only --follow "
	                          + escape(old_commit) + ".." + escape(commit) + " -- " + escape(filename));
	if(result.size() <= 1) {
		return std::string();
	}
	
	std::size_t end = result.size();
	std::size_t pos = result.find_last_of('\n');
	if(pos != std::string::npos && pos + 1 == result.size()) {
		end = pos;
		pos = result.find_last_of('\n', pos - 1);
	}
	if(pos == std::string::npos) {
		return std::string();
	}
	
	return result.substr(pos + 1, end - pos - 1);
}

struct WarningSorter {
	
	bool operator()(const warning & a, const warning & b) {
		
		if(a.line < b.line) {
			return true;
		} else if(a.line > b.line) {
			return false;
		}
		
		if(a.pos < b.pos) {
			return true;
		} else if(a.pos > b.pos) {
			return false;
		}
		
		return a.text.compare(b.text) < 0;
	}
	
};

parsed_warnings parse(const std::string prefix, const std::string & repo,
                      const std::string & listfile, const std::string & commit) {
	
	parsed_warnings result;
	
	std::ifstream ifs(listfile);
	if(!ifs.is_open()) {
		std::cerr << "could not open \"" << listfile << "\"\n";
		std::terminate();
	}
	
	// Parse warning lines and group them by file
	std::string line;
	while(std::getline(ifs, line)) {
		
		boost::trim(line);
		
		if(line.empty()) {
			continue;
		}
		
		std::string file;
		warning w;
		w.text = line;
		w.line = std::size_t(-1);
		w.pos = std::size_t(-1);
		
		w.message = line;
		std::size_t file_start = 0;
		if(line[0] == '[') {
			file_start = 1;
		}
		std::size_t file_end = line.find(':', file_start);
		if(file_end != std::string::npos && file_end != file_start) {
			file = line.substr(file_start, file_end - file_start);
			w.message = line.substr(file_end);
			std::size_t line_start = file_end + 1;
			std::size_t line_end = line.find_first_not_of("0123456789", line_start);
			if(line_end != std::string::npos && line_end != line_start) {
				try {
					w.line = boost::lexical_cast<std::size_t>(line.substr(line_start, line_end - line_start));
					w.message = line.substr(line_end);
					std::size_t pos_start = line_end + 1;
					std::size_t pos_end = line.find_first_not_of("0123456789", pos_start);
					if(pos_end != std::string::npos && pos_end != pos_start) {
						try {
							w.pos = boost::lexical_cast<std::size_t>(line.substr(pos_start, pos_end - pos_start));
							w.message = line.substr(pos_end);
						} catch(...) { }
					}
				} catch(...) { }
			}
		}
		
		result[file].push_back(w);
	}
	
	for(parsed_warnings::value_type & file : result) {
		
		// Get the corresponding code line for each warning as well as some context
		if(boost::starts_with(file.first, prefix)) {
			std::string filename = file.first.substr(prefix.length());
			std::vector<std::string> code = git_get_file(repo, filename, commit);
			if(code.empty()) {
				std::cerr << "could not find file \"" << filename << "\" at commit " << commit << "\n";
			} else {
				for(warning & w : file.second) {
					if(w.line != std::size_t(-1) && w.line > 0 && w.line <= code.size()) {
						if(w.line > 1) {
							w.prev = code[w.line - 2];
						}
						w.code = code[w.line - 1];
						if(w.line + 1 <= code.size()) {
							w.next = code[w.line];
						}
					}
				}
			}
		}
		
		// Sort warnings within a file by line number and position in the line
		std::sort(file.second.begin(), file.second.end(), WarningSorter());
	}
	
	return result;
}

matched_warnings match_files(const std::string prefix, const std::string & repo,
                             const parsed_warnings & a, const std::string commit_a,
                             const parsed_warnings & b, const std::string commit_b) {
	
	matched_warnings result;
	
	for(const parsed_warnings::value_type & file : b) {
		
		std::string filename;
		parsed_warnings::const_iterator i = a.end();
		if(boost::starts_with(filename, prefix)) {
			std::string new_filename = filename.substr(prefix.length());
			std::string old_filename = prefix + git_get_old_filename(repo, filename, commit_a, commit_b);
			if(b.find(old_filename) == b.end()) {
				filename = old_filename;
				i = a.find(filename);
			}
		}
		if(i == a.end()) {
			filename = file.first;
			i = a.find(filename);
		}
		
		if(i == a.end()) {
			result[filename] = std::make_pair(warnings(), file.second);
		} else {
			result[filename] = std::make_pair(i->second, file.second);
		}
		
	}
	
	for(const parsed_warnings::value_type & file : a) {
		if(result.find(file.first) == result.end()) {
			result[file.first] = std::make_pair(file.second, warnings());
		}
	}
	
	return result;
}

constexpr size_t insert_cost = 8 * 1024;
constexpr size_t edit_impossible = 3 * insert_cost + 1;

size_t edit_cost(const warning & a, const warning & b) {
	
	if(a.text == b.text) {
		return 0; // same warning
	}
	
	if(a.message != b.message || a.pos != b.pos || a.code != b.code) {
		return edit_impossible; // different warning
	}
	
	if(a.pos == std::size_t(-1) && a.code.empty()) {
		return edit_impossible; // no idea if the line moved or not
	}
	
	// Prefer warnings with closer line positions
	size_t cost = std::min(size_t(std::labs(a.line - b.line)), insert_cost / 4 - 1);
	
	// Prefer lines with similar context
	if(a.prev != b.prev) {
		cost += insert_cost / 4;
	}
	if(a.next != b.next) {
		cost += insert_cost / 4;
	}
	
	return cost;
}

void match_lines(const warnings & a, const warnings & b) {
	
	// Calculate edit costs
	boost::multi_array<size_t, 2> m(boost::extents[a.size() + 1][b.size() + 1]);
	for(std::size_t i = 0; i <= a.size(); i++) {
		m[i][0] = i * insert_cost;
	}
	for(std::size_t j = 0; j <= b.size(); j++) {
		m[0][j] = j * insert_cost;
	}
	for(std::size_t i = 1; i <= a.size(); i++) {
		for(std::size_t j = 1; j <= b.size(); j++) {
			size_t cost = m[i - 1][j - 1] + edit_cost(a[i - 1], b[j - 1]);
			cost = std::min(cost, m[i][j - 1] + insert_cost);
			cost = std::min(cost, m[i - 1][j] + insert_cost);
			m[i][j] = cost;
		}
	}
	
	// Backtrace path for cheapest edit
	std::vector<int> edits;
	std::size_t i = a.size(), j = b.size();
	while(i != 0 || j != 0) {
		if(j != 0 && m[i][j] == m[i][j - 1] + insert_cost) {
			edits.push_back(int(j));
			j--;
		} else if(i != 0 && m[i][j] == m[i - 1][j] + insert_cost) {
			edits.push_back(-int(i));
			i--;
		} else {
			i--;
			j--;
		}
	}
	
	// Print required edits
	for(int e : boost::adaptors::reverse(edits)) {
		if(e > 0) {
			std::cout << "+" << b[std::size_t(e) - 1].text << "\n";
		} else {
			std::cout << "-" << a[std::size_t(-e) - 1].text << "\n";
		}
	}
	
}

int main(int argc, const char * argv[]) {
	
	if(argc < 7) {
		std::cerr << "Usage: warningdiff <fileprefix> <gitrepo> <commita> <listfilea> <commitb> <listfileb>\n";
		return 1;
	}
	
	std::string prefix = argv[1];
	std::string repo = argv[2];
	std::string commit_a = argv[3];
	std::string list_a = argv[4];
	std::string commit_b = argv[5];
	std::string list_b = argv[6];
	
	parsed_warnings a = parse(prefix, repo, list_a, commit_a);
	parsed_warnings b = parse(prefix, repo, list_b, commit_b);
	
	matched_warnings files = match_files(prefix, repo, a, commit_a, b, commit_b);
	
	for(const matched_warnings::value_type & file : files) {
		match_lines(file.second.first, file.second.second);
	}
	
	return 0;
}
