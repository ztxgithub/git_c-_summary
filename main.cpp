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

class ASingleton
{
public:
    static ASingleton* getInstance()
    {
        static ASingleton instance;
        return &instance;
    }
    void do_something()
    {
        cout<<"ASingleton do_something!"<<endl;
    }
protected:
    struct Object_Creator
    {
        Object_Creator()
        {
            ASingleton::getInstance();
        }
    };
    static Object_Creator _object_creator;

    ASingleton();
    ~ASingleton() {}
};


class BSingleton
{
public:
    static BSingleton* getInstance()
    {
        static BSingleton instance;
        return &instance;
    }
    void do_something()
    {
        cout<<"BSingleton do_something!"<<endl;
    }
protected:
    struct Object_Creator
    {
        Object_Creator()
        {
            BSingleton::getInstance();
        }
    };
    static Object_Creator _object_creator;

    BSingleton();
    ~BSingleton() {}
};


ASingleton::Object_Creator ASingleton::_object_creator;
//BSingleton::Object_Creator BSingleton::_object_creator;



ASingleton::ASingleton()
{
    cout<<"ASingleton constructor!"<<endl;
    BSingleton::getInstance()->do_something();
}
BSingleton::BSingleton()
{
    cout<<"BSingleton constructor!"<<endl;
}

int main()
{
    return 0;
}