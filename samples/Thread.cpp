#include <iostream>
#include "../cppfl/thread.h"
using namespace std;

// 継承して使う例
class MyThread : public Thread {
	int num;
public:
	MyThread(int d) :num(d) {}
	int task(){
		int i;
		for (i=0;i<100;i++) {
			Sleep(10);
			cout << num ;
		}
		return 0;
	}
};

// コールバック関数の例
int func(Thread *t) {
	int i;
	for (i=0;i<100;i++) {
		Sleep(10);
		cout << "0";
	}
	return 0;
}


int main(int argc, char **argv){
	MyThread t1(1),t2(2),t3(3);
	ThreadFunc t0(func);

	// 開始
	t1.start();
	t2.start();
	t3.start();
	t0.start();

	// 待つ
	t1.join();
	t2.join();
	t3.join();
	t0.join();

	return 0;
}
