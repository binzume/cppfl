// �F�X�ȃR���g���[��
#include "../cppfl/all.h"

TextBox edit("aaa");
Label label("label");
ComboBox cb;

int button_pushed(Event &e)
{
	label.text("text:"+edit.text());
	cb.add(edit.text());
	msgBox(e.target,"���͂��ꂽ�����F"+edit.text());
	return 1;
}

int main(int argc,char **argv)
{
	Form myForm("cppfl �T���v��");
	Button button("OK");
	myForm.size(320,240);
	button.onClick(button_pushed);
	myForm.add(label,10,10);
	myForm.add(edit);

	cb.add("item1");
	cb.add("item2");
	cb.sel(0);

	myForm.add(cb);

	myForm.add(button);

	wait(myForm);
	return 0;
}

