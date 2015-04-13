// オブジェクト指向っぽい例(仮)
#include <iostream>
#include "../cppfl/all.h"

class MyForm: public Form{
	Button myButton;
public:
	MyForm(){
		text("test");
		myButton.text("Click Me");
		myButton.onClick(&MyForm::clicked,this);
		myButton.location(100,100);
		myButton.parent(this);
	}
	bool clicked(Event e){
		msgBox("Button clicked");
		close();
		return true;
	}
};

int main(int argc,char **argv) {
	MyForm f;
	wait(f);
	return 0;
}

