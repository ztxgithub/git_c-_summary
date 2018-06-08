#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stdlib.h>
#include <memory>
#include <string>
#include <iostream>
#include <sys/time.h>
using namespace std;

class Person{
public:
    char* name;
    Person(const char* p){
        log("constructor");
        size_t n = strlen(p) + 1;
        name = new char[n];
        memcpy(name, p, n);
    }
    Person(const Person& p){
        log("copy constructor");
        size_t n = strlen(p.name) + 1;
        name = new char[n];
        memcpy(name, p.name, n);
    }

    const Person& operator=(const Person& p){
        printf("=const Person& p[%s]\n", p.name);
        log("copy assignment operator");
        size_t n = strlen(p.name) + 1;
        name = new char[n];
        memcpy(name, p.name, n);
        return *this;
    }

    ~Person(){
        log("destructor");
        delete[] name;
    }

private:
    void log(const char* msg)
    {
        cout << "[" << this << "] " << msg << "\n";
    }
};

Person getAlice(){
    Person p("alice"); return p;
}

int main(){
    cout<<"______构造函数start________________"<<endl;
    Person a = getAlice();
    cout<<"______构造函数end________________"<<endl;

    cout<<"______赋值函数= start________________"<<endl;
    a = getAlice();
    cout<<"______赋值函数= end________________"<<endl;
}