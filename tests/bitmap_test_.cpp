#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/imageex.h"
using namespace std;

void test()
{
	// !! deprecate class
	ImageFilter<Bitmap> filter;
	filter.load("test.bmp");
	filter.flip();
	filter.save("test_out.bmp");

	assert(true);

	cout << "ok." << endl;
}

int main(int argc,int argv[])
{
	test();
	return 0;
}

