
#include <iostream>
#include <vector>
#include <sstream>

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

int main() {
	
	size_t open = 0;
	
	while(!cin.eof()) {
		
		string buf;
		getline(cin, buf);
		
		size_t p = 0;
		size_t i;
		while((i = buf.find('\x1B', p)) != string::npos) {
			
			cout << escape(buf.substr(p, i - p));
			
			p = i + 1;
			if(p >= buf.length() || buf[p] != '[') {
				cerr << "invalid escape code start " << buf << endl;
				return 1;
			}
			p++;
			
			vector<string> cmds;
			bool close = false;
			
			while(1) {
				
				i = buf.find_first_not_of("0123456789", p);
				if(i == string::npos || i == p) {
					cerr << "invalid escape code " << buf << endl;
					return 1;
				}
				
				string cmd = buf.substr(p, i - p);
				if(cmd == "0") {
					close = true;
				} else {
					cmds.push_back(cmd);
				}
				
				p = i + 1;
				
				if(buf[i] == 'm') {
					break;
				}
				
				if(buf[i] != ';') {
					cerr << "invalid escape code end " << buf[p] << " " << buf << endl;
					return 1;
				}
			}
			
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
	}
	
	close_tags(open);
	
	return 0;
}
