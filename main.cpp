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

class base{
public :
    static base base_object1;//正确，静态数据成员
//    base _object2;//错误
    base *pObject;//正确，指针
    base &mObject;//正确，引用
};



int main()
{
//
    return 0;
}
