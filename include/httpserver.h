#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#include "http.h"
#include "socket.h"
#include <string>
#include <vector>
#include <map>

namespace net
{

typedef std::pair<string,string> HTTPHeader;

class HttpRequest
{
public:
	std::string method;
	std::string path;
	std::string query_string;
	std::vector< HTTPHeader > headers;

	std::string content;
	std::string get_header(const std::string &name) {
		for (int i=0;i<headers.size();i++) {
			if (headers[i].first == name) {
				return headers[i].second;
			}
		}
		//if (headers.count(name)) {
		//	return headers.find(name)->second[0];
		//}
		return "";
	}

	void initialize() {
		clear();
		method = "GET";
	}
	void clear() {
		headers.clear();
		content = "";
	}
};

class HTTPResponse
{
public:
	int status;
	std::string body;
	std::string contentType;
	bool sendheader;
	Socket soc;
	HTTPResponse() {
		status = 200;
		contentType = "text/plain";
		sendheader = false;
	}

	void send_header() {
		if (!sendheader) {
			soc.write("HTTP/1.1 200 OK\r\n");
			soc.write("Content-Type: " + contentType + "\r\n");
			soc.write("\r\n");
			sendheader = true;
		}
	}

	void write(std::string s) {
		if (!sendheader) {
			send_header();
		}

		soc.write(s);
	}

};

class HandlerFuncBase
{
public:
	virtual bool operator() (HTTPResponse&,HttpRequest&)=0;
};

template<typename F>
class HandlerFunc : public HandlerFuncBase {
public:
	F func;
	HandlerFunc(F f): func(f){}
	bool operator() (HTTPResponse &res,HttpRequest &req){
		return func(res,req);
	}
};


class HttpServer
{

	SocketServer *ss;

	std::map<std::string,HandlerFuncBase*> handlers;

	void client_proc(Socket soc) {
		cout << "accepted" << endl;

		HttpRequest req;

		// request
		std::string line = soc.readLine();

		int p=line.find(" ",0);
		if (p != std::string::npos) {
			req.method = line.substr(0,p);

			int p2=line.find(" ",p+1);
			if (p2 != std::string::npos) {
				cout << ">"<< line.substr(p+1,p2-p-1) << ";" << endl;

				req.path = line.substr(p+1,p2-p-1);

				p=req.path.find("?",0);
				if (p != std::string::npos) {
					req.query_string = req.path.substr(p+1);
					req.path = req.path.substr(0,p);
				}
			}
		}

		// headers
		while(!soc.error()) {
			std::string headerstr = soc.readLine();
			cout << "[" << headerstr << "]" << endl;
			if (headerstr == "") break;

			int p=headerstr.find(":",0);
			if (p != std::string::npos) {
				req.headers.push_back(HTTPHeader(headerstr.substr(0,p), headerstr.substr(p+1)));
			}
		}

		// create response
		HTTPResponse res;
		res.soc = soc;
		std::string dir;

		HandlerFuncBase *handler = NULL;


		for (int i=0; i<req.path.size(); i++) {
			if (req.path[i] == '?') {
				break;
			}
			dir.push_back(req.path[i]);
			if (req.path[i] == '/') {
				if (handlers.count(dir)) {
					handler = handlers[dir];
				}
			}
		}

		if (handler == NULL && handlers.count(dir)) {
			handler = handlers[dir];
		}
		if (handler != NULL) {
			(*handler)(res,req);
		} else {
			res.status = 404;
			res.write("Not Found.");
		}

		res.send_header();
		soc.write(res.body);

		soc.close();
		cout << "close" << endl;
	}

public:

	template <typename T>
	void handler(std::string path,T handler){
		handlers[path] = new HandlerFunc<T>(handler);
	}

	void start(int port = 80) {

		ss = new SocketServer(port);
	}

	void stop() {
		if (ss != NULL) {
			ss->close();
		}
		ss = NULL;
	}

	void wait() {
		for (;;) {
			if (ss->accepted()) {
				Socket s = ss->accept();
				client_proc(s);
			}

			Sleep(10);
		}
	}
};

}
#endif //HTTP_SERVER_H
