#include "../cppfl/all.h"

int main()
{
	Form myForm("Form�T���v��",320,240);

	Label myLabel("Hello, world!");
	myLabel.size(300,40);
	myLabel.font(Font("�l�r �S�V�b�N",40));

	myForm.add(myLabel,10,80);

	wait(myForm);
	return 0;
}
