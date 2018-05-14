#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;
typedef void(*Fun)(void);

class Base {
public:
    virtual void f() { cout << "Base::f" << endl; }
    virtual void g() { cout << "Base::g" << endl; }
    virtual void h() { cout << "Base::h" << endl; }
};

int main(int argc, char *argv[]) {

    Base b;

    Fun pFun = NULL;

    cout << "虚函数表地址：" << (int*)(&b) << endl;                    //# 强行把&b转成int*，取得虚函数表的地址
    cout << "虚函数表 — 第一个函数地址：" << (int*)*(int*)(&b) << endl;  //# 虚函数表的地址取值就可以得到第一个虚函数的地址

// Invoke the first virtual function
    pFun = (Fun)*((int*)*(int*)(&b));
    pFun();
    pFun = (Fun)*((int*)*(int*)(&b) + 1);
    pFun();
}