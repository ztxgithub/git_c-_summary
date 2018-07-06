#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stdlib.h>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <noncopyable.hpp>
#include <iostream>
#include <functional>
using namespace std;
using namespace std::placeholders;


class C{
    int m;
public:
    C(){cout<<"in C constructor"<<endl;}
    ~C(){cout<<"in C destructor"<<endl;}
};

class A{
public:
    A(){cout<<"in A constructor"<<endl;}
    ~A(){cout<<"in A destructor"<<endl;}
};

class B:public A{
public:
    C c;
    char* resource;

    B(){
        resource=new char[100];
        cout<<"in B constructor"<<endl;
        throw -1;
    }
    ~B(){
        cout<<"in B destructor"<<endl;
        delete[]  resource;
    }
};

int main(){
    try{
        B b;
    }
    catch(int){
        cout<<"catched"<<endl;
    }
}
