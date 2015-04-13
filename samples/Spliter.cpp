#include "../cppfl/all.h"

int main(int argc, char **argv)
{
	Form myForm("cppfl サンプル", 640, 480);
	myForm.fixsize(false); // サイズ変更可能

	Spliter spl(0);
	Spliter spl2(0, false); // 横分割

	TextBox t1("あああ");
	TextBox t2("test2",0,true);
	TextBox t3("test3",0,true);
	ListView lv;
	char *cols[] = {"name","aaa2"};
	lv.addCol(cols,2);
	char *s[] = {"abc","222"};
	lv.add(s,2);
	spl.add(&t1);
	spl.add(&spl2);
	spl2.add(&t2);
	spl2.add(&lv);
	spl.size(320,240);
	spl.align(Widget::FIT);

	myForm.add(spl,0,30);


	// 検索ボックス
	TextBox w("");
	w.size(240,24);
	myForm.add(w,4,2);

	// 検索ボタン
	Button findb("find");
	myForm.add(findb,250,2);

	wait(myForm);

	return 0;
}

