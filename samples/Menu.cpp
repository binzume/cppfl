#include "../include/all.h"

bool onMenu(Event &e)
{
	if (e.id==109) {
		Application.exit(0);
	} else {
		msgBox(STR "MENU: "+e.id);
	}
	return true;
}

int main(int argc,char **argv)
{
	Form myForm("cppfl ƒTƒ“ƒvƒ‹", 320, 240);

	MenuItem mi1("test",108);
	mi1.checked(true);
	mi1.defaultItem(true);
	MenuItem mi2("&Help",901);
	
	myForm.menu(MainMenu()
		.add("&File",Menu()
			.add("Open",101)
			.add(mi1)
			.add(MenuItem("-"))
			.add("Exit",109)
		)
		.add("&Edit",Menu()
			.add("&Copy",201)
			.add("Cu&t",202)
			.add("&Paste",202)
		)
		.add(mi2)
	);

	myForm.onMenu(onMenu);

	myForm.menu().get(mi1,101);
	mi1.enabled(false);

	wait(myForm);

	return 0;
}

