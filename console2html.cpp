/*
 * Copyright (C) 2011 Daniel Scharrer
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

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

#include <boost/algorithm/string/predicate.hpp>

using std::cin;
using std::cout;
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::ostringstream;

string escape(const string & str) {
	
	ostringstream oss;
	
	for(string::const_iterator i = str.begin(); i != str.end(); ++i) {
		if(*i == '&') {
			oss << "&amp;";
		} else if(*i == '<') {
			oss << "&lt;";
		} else if(*i == '>') {
			oss << "&gt;";
		} else {
			oss << *i;
		}
	}
	
	return oss.str();
}

void close_tags(size_t & open) {
	
	for(size_t i = 0; i < open; i++) {
		cout << "</span>";
	}
	
	open = 0;
}

static bool is_end_char(char c) {
	return (c >= 64 && c < 127);
}

static void print_escaped_char(char cc) {
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
	} else if(c <= 32) {
		std::cerr << "\\x" << std::setfill('0') << std::setw(2) << int(c);
	} else {
		std::cerr << (char)c;
	}
}

int main() {
	
	size_t open = 0;
	
	size_t line = 0;
	
	while(!cin.eof()) {
		
		string buf;
		getline(cin, buf);
		
		size_t p = 0;
		size_t i;
		while((i = buf.find('\x1B', p)) != string::npos) {
			
			cout << escape(buf.substr(p, i - p));
			
			p = i + 1;
			if(p >= buf.length()) {
				std::cerr << line << ':' << i << ": line terminated after escape code";
				break;
			}
			
			if(buf[p] == '=') {
				// msvc/wine generates this
				p++;
				continue;
			} else if(buf[p] != '[') {
				std::cerr << line << ':' << i << ": invalid escape code start: \"\\e";
				if(p < buf.length()) {
					print_escaped_char(buf[p]);
				}
				std::cerr << "\"" << endl;
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
			
			std::vector<std::string> cmds;
			bool close = false;
			
			if(p < e && buf[e - 1] == 'm') {
				
				// Color command
				
				while(p < e) {
					
					i = buf.find_first_not_of("0123456789", p);
					if(i == string::npos || i == p) {
						break;
					}
					
					string cmd = buf.substr(p, i - p);
					if(cmd == "0") {
						close = true;
					} else {
						cmds.push_back(cmd);
					}
					
					p = i + 1;
					
					if(buf[i] != 'm' && buf[i] != ';') {
						std::cerr << line << ':' << i << ": invalid escape code " << buf[p]
						          << " " << buf << endl;
					}
				}
				
			} else {
				
				std::string command = buf.substr(p, e - p);
				
				if(command == "?1h") {
					// msvc/wine generates this
				} else {
					
					// Other command
					
					std::cerr << line << ':' << i << ": ignoring unknown escape code: \"\\e[";
					for(; p < e; p++) {
						print_escaped_char(buf[p]);
					}
					std::cerr << "\"" << std::endl;
					
				}
				
			}
			
			p = e;
			
			if(close) {
				close_tags(open);
			}
			
			if(!cmds.empty()) {
				cout << "<span class=\"";
				for(vector<string>::const_iterator j = cmds.begin(); j != cmds.end(); ++j) {
					if(j != cmds.begin()) {
						cout << ' ';
					}
					cout << 'c' << *j;
				}
				cout << "\">";
				open++;
			}
			
		}
		
		cout << escape(buf.substr(p)) << endl;
		line++;
	}
	
	close_tags(open);
	
	return 0;
}
