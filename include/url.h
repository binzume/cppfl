#include <string>
#include <sstream>

namespace net{

	class Url {
		public:
		bool valid;
		std::string scheme;
		std::string host;
		int port;
		std::string path;
		std::string query;
		std::string fragment;
		Url(const std::string &s) {
			port = 0;
			int st = 0;
			for (auto it = s.begin(); it != s.end(); it++) {
			    auto c = *it;
				if (st == 0) {
					if (c == ':') {
						st = 1;
					} else {
						scheme.push_back(c);
					}
				} else if (st == 1) {
					if (c != '/') {
						st = 2;
					}
				}
				if (st == 2) { // host
					if (c == '/') {
						st = 4;
					} else if (c == ':') {
						st = 3;
						continue;
					} else {
						host.push_back(c);
					}
				}
				if (st == 3) { // port
					if (c == '/') {
						st = 4;
					} else if (c >= '0' && c <= '9') {
						port = port * 10 + (c - '0');
					}
				}
				if (st == 4) { // path
					if (c == '?') {
						st = 5;
						continue;
					}
					if (c == '#') {
						st = 6;
						continue;
					}
					path.push_back(c);
				}
				if (st == 5) { // query
					if (c == '#') {
						st = 6;
						continue;
					}
					query.push_back(c);
				}
				if (st == 6) { // fragment
					fragment.push_back(c);
				}
			}
			valid = st >= 2; // scheme://host
		}
		std::string str(bool usefrag = true) {
			std::stringstream ss;
			ss << scheme << "://" << host;
			if (port) ss <<  ":" << port;
			ss << path;
			if (query.size()) ss << "?" << query;
			if (usefrag && fragment.size()) ss << "#" << fragment;
			return ss.str();
		}
	};

	inline static Url parse_url(const std::string &s) {
		return Url(s);
	}

};
