#if 0

input="$(readlink -f "$(command -v "$0")")"
output="${0%.*}"

#// Compile ourseves
if [ "$input" -nt "$output" ] ; then
	printf 'Compiling %s...\n' "${output##*/}" >&2
	${CXX:-g++} -std=c++11 -Wall -Wextra "$input" -o "$output" > /dev/null < /dev/null || exit 1
fi

#// Run the executable
exec "$output" "$@"

exit

#endif // 0

/*
 * Copyright (C) 2011-2014 Daniel Scharrer
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
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <utility>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

static bool is_end_char(char c) {
	return (c >= 64 && c < 127);
}

static void print_escaped(char cc) {
	unsigned char c = cc;
	if(c == '\\') {
		std::cerr << "\\\\";
	} else if(c == '\e') {
		std::cerr << "\\e";
	} else if(c == '\n') {
		std::cerr << "\\n";
	} else if(c == '\r') {
		std::cerr << "\\r";
	} else if(c == '\b') {
		std::cerr << "\\b";
	} else if(c == '\a') {
		std::cerr << "\\a";
	} else if(c == '\0') {
		std::cerr << "\\0";
	} else if(c == '\t') {
		std::cerr << "\\t";
	} else if(c == '"') {
		std::cerr << "\\\"";
	} else if(c <= 32 && c != ' ') {
		std::cerr << "\\x" << std::setfill('0') << std::setw(2) << int(c);
	} else {
		std::cerr << (char)c;
	}
}

static void print_escaped(const char * str, std::size_t length) {
	for(std::size_t i = 0; i < length; i++) {
		print_escaped(str[i]);
	}
}

typedef std::size_t command_class;
typedef std::size_t command;

typedef std::map<command_class, command> commands_t;

command_class get_command_class(command cmd) {
	if(cmd == 2 || cmd == 21 || cmd == 22) {
		return 1; // Boldness
	} else if(cmd == 6 || cmd == 25) {
		return 5; // Blink
	} else if(cmd >= 10 && cmd <= 19) {
		return 10; // Font
	} else if(cmd == 20 || cmd == 23) {
		return 3; // Italic/Fraktur
	} else if(cmd == 24) {
		return 4; // Underlined
	} else if(cmd == 27) {
		return 7; // Negative
	} else if(cmd == 28) {
		return 8; // Conceal
	} else if(cmd == 29) {
		return 9; // Strike-through
	} else if((cmd > 30 && cmd <= 39) || (cmd >= 90 && cmd <= 99)) {
		return 30; // Foreground color
	} else if((cmd > 40 && cmd <= 49) || (cmd >= 100 && cmd <= 109)) {
		return 40; // Background color
	} else if(cmd == 52 || cmd == 54) {
		return 52; // Framed / encircled
	} else if(cmd == 55) {
		return 53; // Overlined
	} else {
		return cmd;
	}
}

class output {
	
public:
	
	virtual ~output() { };
	
	virtual void text(const char * text, std::size_t n) = 0;
	
	virtual void color(commands_t && commands) = 0;
	
	virtual void reset() = 0;
	
};

template <typename K, typename V1, typename V2, typename Compare>
bool is_subset(const std::map<K, V1, Compare> & a, const std::map<K, V2, Compare> & b) {
	
	if(a.size() > b.size()) {
		return false; // a is larger, cannot be subset
	}
	
	auto ia = a.begin();
	auto ib = b.begin();
	
	// technically, a.key_comp() and b.key_comp() might not be equivalent
	Compare comp{}; // cannot use value_comp() as V1 and V2 might differ
	
	while(ia != a.end()) {
		
		while(ib != b.end() && comp(ib->first, ia->first)) {
			ib++;
		}
		
		if(ib == b.end() || comp(ia->first, ib->first)) {
			return false; // b does not contain *ia
		}
		
		// *ia == *ib
		ia++, ib++;
		
	}
	
	return true;
}

typedef std::map<command, const char *> tags_t;

class html_format : public output {
	
	const commands_t defaults = []() {
		commands_t defaults;
		defaults[1] = 21; // Not bold, not faint
		defaults[3] = 23; // Not italic, not fraktur
		defaults[4] = 24; // Not underlined
		defaults[5] = 25; // Not blinking
		defaults[7] = 27; // Not negative
		defaults[8] = 28; // Not conceiled
		defaults[9] = 29; // No strike-through
		defaults[10] = 10; // Default font
		defaults[30] = 39; // Default foreground color
		defaults[40] = 49; // Default background color
		defaults[52] = 54; // Not framed, not encircled
		defaults[53] = 55; // Not overlined
		return std::move(defaults);
	}();
	
	const tags_t tags = []() {
		tags_t tags;
		tags[1] = "b";
		tags[3] = "i";
		tags[4] = "u";
		tags[5] = "blink";
		tags[9] = "strike";
		return std::move(tags);
	}();
	
	typedef std::pair<commands_t, commands_t> frame_t;
	std::stack<frame_t> stack;
	
	void close() {
		
		const char * tag = "span";
		if(stack.top().first.size() == 1) {
			auto it = tags.find(stack.top().first.begin()->second);
			if(it != tags.end()) {
				tag = it->second;
			}
		}
		
		std::cout << "</" << tag << ">";
		
		stack.pop();
	}
	
public:
	
	html_format() { }
	
	void text(const char * text, std::size_t length) {
		
		const char * end = text + length;
		
		while(true) {
			
			const char * c = std::find_if(text, end, [](char c) {
				return c == '&' || c == '<' || c == '>';
			});
			if(c == end) {
				std::cout.write(text, length);
				break;
			} else {
				std::cout.write(text, c - text);
				text = c + 1;
				length = end - text;
			}
			
			switch(*c) {
				case '&': std::cout << "&amp;"; break;
				case '<': std::cout << "&lt;";  break;
				case '>': std::cout << "&gt;";  break;
			}
			
		}
		
	}
	
	void color(commands_t && cmds) {
		
		while(!stack.empty() && is_subset(stack.top().first, cmds)) {
			// Completely overwrites stack frame, can close now
			close();
		}
		
		commands_t current;
		if(stack.empty()) {
			current = defaults;
		} else {
			current = stack.top().second;
		}
		
		for(commands_t::iterator it = cmds.begin(); it != cmds.end(); ) {
			
			auto cit = current.find(it->first);
			if(cit != current.end() && cit->second == it->second) {
				// No change
				it = cmds.erase(it);
				continue;
			}
			
			// Record the changed state
			current[it->first] = it->second;
			
			auto tit = tags.find(it->second);
			if(tit != tags.end()) {
				// We have a special tag for this type
				std::cout << "<" << tit->second << ">";
				commands_t cmd;
				cmd[it->first] = it->second;
				stack.emplace(std::move(cmd), current);
				it = cmds.erase(it);
				continue;
			}
			
			it++;
			
		}
		
		if(cmds.empty()) {
			// No commands left
			return;
		}
		
		std::cout << "<span class=\"";
		bool first = true;
		for(std::pair<command_class, command> cmd : cmds) {
			if(first) {
				first = false;
			} else {
				std::cout << ' ';
			}
			std::cout << 'c' << cmd.second;
		}
		std::cout << "\">";
		
		stack.emplace(std::move(cmds), std::move(current));
		
	}
	
	void reset() {
		
		while(!stack.empty()) {
			close();
		}
		
	}
	
};

class plain_format : public output {
	
public:
	
	void text(const char * text, std::size_t n) {
		std::cout.write(text, std::streamsize(n));
	}
	
	void color(commands_t && cmds) {
		(void)cmds;
		// No colors supported
	}
	
	void reset() {
		// Nothing to reset
	}
	
};

int main(int argc,  char * argv[]) {
	
	const char * format = "html";
	if(argc > 1) {
		if(argc == 3 && boost::equals(argv[1], "--format")) {
			format = argv[2];
		} else {
			std::cerr << "Usage: console2html [--format <format>]\n";
			return 1;
		}
	}
	
	std::unique_ptr<output> out;
	if(boost::equals(format, "html")) {
		out.reset(new html_format);
	} else if(boost::equals(format, "plain")) {
		out.reset(new plain_format);
	} else {
		std::cerr << "Unknown format: " << format << "\n";
		return 1;
	}
	
	std::size_t line = 0;
	
	std::string buf;
	commands_t cmds;
	bool close = false;
	std::size_t newlines = 0;
	
	while(!std::cin.eof()) {
		
		getline(std::cin, buf);
		
		size_t p = 0;
		while(true) {
			
			size_t i = buf.find('\x1B', p);
			
			// Ignore carriage return at start/end end of lines
			std::size_t end = (i == std::string::npos) ? buf.length() : i;
			while(p != end && buf[p] == '\r') p++;
			while(p != end && buf[end - 1] == '\r') end--;
			
			// We have actual text!
			if(end != p) {
				// Flush the command buffer: resets
				if(close) {
					out->reset();
					close = false;
				}
				// Print newlines
				for(; newlines; newlines--) {
					out->text("\n", 1);
				}
				// Flush the command buffer: new colors
				if(!cmds.empty()) {
					out->color(std::move(cmds));
					cmds.clear();
				}
				// Print plain text
				out->text(buf.data() + p, end - p);
			}
			
			if(i == std::string::npos) {
				break;
			}
			
			p = i + 1;
			if(p >= buf.length()) {
				std::cerr << line << ':' << i << ": line terminated after escape code\n";
				break;
			}
			
			if(buf[p] == '=') {
				// msvc/wine generates this
				p++;
				continue;
			} else if(buf[p] != '[') {
				std::cerr << line << ':' << i << ": invalid escape code start: \"\\e";
				print_escaped(buf[p]);
				std::cerr << "\"\n";
				continue;
			}
			p++;
			
			size_t e = p;
			while(e < buf.length()) {
				char c = buf[e];
				e++;
				if(is_end_char(c)) {
					break;
				}
			}
			
			if(p < e && buf[e - 1] == 'm') {
				
				// Color command
				
				while(p < e) {
					
					i = buf.find_first_not_of("0123456789", p);
					if(i == std::string::npos) {
						std::cerr << line << ':' << i << ": line terminated after escape code\n";
						break;
					}
					
					std::size_t cmd;
					try {
						cmd = (i == p) ? 0 : boost::lexical_cast<std::size_t>(buf.data() + p, i - p);
					} catch(...) {
						std::cerr << line << ':' << i << ": bad color number: \"";
						print_escaped(buf.data() + p, i - p);
						std::cerr << "\"\n";
					}
					
					if(cmd == 0) {
						cmds.clear();
						close = true;
					} else {
						cmds[get_command_class(cmd)] = cmd;
					}
					
					p = i + 1;
					
					if(buf[i] != 'm' && buf[i] != ';') {
						std::cerr << line << ':' << i << ": invalid color char: '";
						print_escaped(buf[i]);
						std::cerr << "'\n";
					}
					
				}
				
			} else if(e - p == 3 && buf[p] == '?' && buf[p + 1] == '1' && buf[p + 2] == 'h') {
				// msvc/wine generates this
			} else if(e - p == 1 && buf[p] == 'K') {
				/*
				 * gcc 4.10 generates this
				 * This is meant to clear the background of wrapped lines as some terminals
				 * fill them with the current background color.
				 * Unfortunately they emit this even if they never changed the background color!
				 * As we don't wrap lines it can be safely ignored.
				 */
			} else {
				
				// Other command
				
				std::cerr << line << ':' << i << ": ignoring unknown escape code: \"\\e[";
				print_escaped(buf.data() + p, e - p);
				std::cerr << "\"\n";
				
			}
			
			p = e;
			
		}
		
		newlines++;
		line++;
	}
	
	out->reset();
	// Ignore trailing commands!
	
	for(; newlines > 1; newlines--) {
		out->text("\n", 1);
	}
	
	return 0;
}
