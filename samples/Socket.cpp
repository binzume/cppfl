#include <iostream>
#include "../cppfl/socket.h"
using namespace std;

int main(){
	Socket soc("www.binzume.net",80);
	soc.write("GET /software/cppfl/socket.h HTTP/1.0\n");
	soc.write("Host: www.binzume.net\n\n");
	while(!soc.error()) {
		cout << soc.read() << flush;
	}
	soc.close();
	
	cout << sizeof(Socket) << endl;
	cout << sizeof(SocketServer) << endl;
	return 0;
}

