// �{�^���ƃ��b�Z�[�W�{�b�N�X
#include "../include/all.h"
using namespace std;

int button_pushed(Event &e)
{
	msgBox(e.target,"Hello, world!");
	return true;
}

int main(int argc,char **argv)
{
	Form myForm("�{�^���ƃ��b�Z�[�W�{�b�N�X");
	Button myButton("Push me");
	myForm.size(320,240);
	myButton.size(300,40);
	myButton.font(new Font("�l�r �S�V�b�N",40));
	myButton.onClick(button_pushed);
	myForm.add(myButton,10,80);

	wait(myForm);

	return 0;
}
