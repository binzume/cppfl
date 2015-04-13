#include "../cppfl/all.h"

int main()
{
	Form myForm("FormƒTƒ“ƒvƒ‹",320,240);

	myForm.show();

	msgBox(&myForm, "Hello","title");
	myForm.msgBox("C++","title");
	msgBox("world!","title");

	wait(myForm);
	return 0;
}

