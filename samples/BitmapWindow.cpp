#include "../include/all.h"

int main()
{
	BitmapWindow bw("sample", 640, 480);

	bw.print("Hello, world!");

	bw.color(Color::red);
	bw.circlef(320, 240, 100);

	wait(bw);

	return 0;
}
