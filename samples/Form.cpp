#include "../include/all.h"

int main()
{
	Form myForm("Formサンプル",320,240);

	Label myLabel("Hello, world!");
	myLabel.size(300,40);
	myLabel.font(Font("ＭＳ ゴシック",40));

	myForm.add(myLabel,10,80);

	wait(myForm);
	return 0;
}
