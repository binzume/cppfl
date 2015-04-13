#include "../cppfl/all.h"

bool onMenu(Event &e)
{
	if (e.id==109) {
		Application.exit(0);
	} else {
		msgBox(STR "MENU: "+e.id);
	}
	return true;
}
bool onMenuExit(Event &e)
{
	Application.exit(0);
	return true;
}

int main(int argc,char **argv)
{
	Form myForm("cppfl ƒTƒ“ƒvƒ‹", 320, 240);

	MenuHandler menuhandler;

	MenuItem mi1("test",menuhandler.add(onMenu));
	mi1.checked(true);
	mi1.defaultItem(true);
	MenuItem mi2("&Help",menuhandler.add(onMenu));
	
	myForm.menu(MainMenu()
		.add("&File",Menu()
			.add("Open",menuhandler.add(101,onMenu))
			.add(mi1)
			.add(MenuItem("-"))
			.add("Exit",menuhandler.add(onMenuExit))
		)
		.add("&Edit",Menu()
			.add("&Copy",menuhandler.add(onMenu))
			.add("Cu&t",menuhandler.add(onMenu))
			.add("&Paste",menuhandler.add(onMenu))
		)
		.add(mi2)
	);

	myForm.onMenu(menuhandler,1);

	myForm.menu().get(mi1,101);
	mi1.enabled(false);

	wait(myForm);

	return 0;
}

