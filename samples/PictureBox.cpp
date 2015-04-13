#include "../cppfl/all.h"


int main(int argc,char **argv) {
	Form myForm("cppfl ƒTƒ“ƒvƒ‹");
	myForm.size(320,240);

	Label myLabel("Hello, world!");
	myLabel.size(200,40);
	myForm.add(myLabel,10,80);

	PictureBox pic(Resources::LoadBitmap("test.bmp"));
	myForm.add(pic,10,2);

	wait(myForm); 

	return 0;
}

