#undef NDEBUG
#include <cmath>
#include <cassert>
#include <iostream>
#include <sstream>
#include "../include/json.h"
#include <iomanip>
using namespace std;

#define EPSILON 1e-8

void test() {
	json::jsvalue v = json::JSON_CONST([
		1,null,
		[
		  31 , 32, "asdfg\"hjk" ,
		  {
			"hoge" : -123e6,
			"fuga" : 3.14159
		  },123
		],
		null, true, false, -1, 0, 0.5, {}, [], ""
	]);

	// jsvalues
	cout << (int)v[2][1] << endl;
	cout << (string)v[2][2] << endl;
	cout << v[2][3]["hoge"].to_i() << endl;
	cout << v[2][3]["fuga"].to_f() << endl;
	cout << v[1].is_null() << endl;

	// stringify
	stringstream ss;
	ss << std::setprecision( 10 );
	ss << v;
	cout << ss.str() << endl;

	assert(ss.str() == "[1,null,[31,32,\"asdfg\\\"hjk\",{\"fuga\" : 3.14159,\"hoge\" : -123000000},123],null,true,false,-1,0,0.5,{},[],\"\"]");
	assert((int)v[2][1] == 32);
	assert((string)v[2][2] == "asdfg\"hjk");
	assert(v[2][3]["hoge"].to_i() == -123000000);
	assert(fabs(v[2][3]["fuga"].to_f() - 3.14159) < EPSILON);
	assert(v[1].is_null());

	cout << "ok." << endl;
}

int main(int argc, char * argv[])
{
	test();
	return 0;
}
