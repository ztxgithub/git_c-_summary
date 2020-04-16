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
#include <vector>
#include <map>

#include <function.hpp>  // 预定义函数对象
#include <set>
#include <algorithm>
using namespace std;

#include <set>
#include <iostream>

class B
{
    int b;
public:
    virtual ~B(){ cout <<"B::~B()"<<endl; }
//    virtual C(){ cout <<"B::C()"<<endl; }
};

class GreaterSimu
{
public:
    bool operator()(int v1, int v2)
    {
        return v1 > v2;
    }
};

class Teacher{
public:
    Teacher(int tId, int tAge):id(tId), age(tAge){}
    int id;
    int age;
};

class CmpTeacher
{
public:
    bool operator()(Teacher v1, Teacher v2)
    {
        return v1.id > v2.id;
    }
};

/*
 * 如果需要外部再传入一个参数 a, 使得遍历 vector 的元素大于 a 时, 才打印
 * 第一步: 自定义函数对象去继承父类 binary_function(2 个参数) unary_function(1 个参数)
 *
 * */
class print{
public:
    void operator()(int i)
    {
        cout << " i " << i << endl;
    }
};

/*
 * 如果需要外部再传入一个参数 a, 使得遍历 vector 的元素大于 a 时, 才打印
 * 第一步: 自定义函数对象去继承父类 binary_function(2 个参数) unary_function(1 个参数)
 *        其中　binary_function<int, int, void>　第一个 int 代表 int v1, 第二个 int 代表 int v2, 第三个 void 代表返回值
 * */
class printEx1:public binary_function<int, int, void>{
public:
    void operator()(int v1, int v2) const  // 这里一定要加上 const
    {
        if(v1 > v2)
        {
            cout << " v1 " << v1 << endl;
        }

    }
};


class compareDef{
public:
    bool operator()(int i)
    {
        return i > 5;
    }
};

/*
 * not1 是一元函数对象取反
 * not2 是二元函数对象取反
 * */
class compareDefNot:public unary_function<int, bool>{
public:
    bool operator()(int i) const
    {
        return i > 5;
    }
};


void testNot()
{
    vector<int> v;
    for(auto i = 0; i < 10; i++)
    {
        v.push_back(i);
    }

//    auto itor = find_if(v.begin(), v.end(), compareDef());
    /*
     * 找到第一个小于等于 5
     * */
    auto itor = find_if(v.begin(), v.end(), not1(compareDefNot()));
    if(itor != v.end())
    {
        cout << "find_if true " << *itor << endl;
    } else{
        cout << "find_if not true " << endl;
    }
}

int main( )
{
//    set<int> iVector;  // 从大到小
    set<int, GreaterSimu > iVector;  // 从大到小
//    set<int, greater<int> > iVector;  // 从大到小
//    set<int, less<int> > iVector;  // 从小到大
    iVector.insert(1);
    iVector.insert(2);
    iVector.insert(3);
    iVector.insert(4);
    iVector.insert(5);


//    for(auto iter = iVector.begin(); iter != iVector.end(); iter++)
//    {
//        cout << *iter << " ";
//    }
//
//
//    set<Teacher, CmpTeacher> teachers;  // 从大到小
//    teachers.insert(Teacher(1, 2));
//    auto iterTeacher = teachers.find(Teacher(1, 3));  // 因为是按照　CmpTeacher　函数对象,　Teacher.id 进行排序的,所以可以查找到
//
//    map<int, int> imap;
//    imap.insert(make_pair(1, 1));


    vector<int> iVecs;
    iVecs.push_back(1);
    iVecs.push_back(2);
    iVecs.push_back(3);
    iVecs.push_back(4);
    iVecs.push_back(5);
    iVecs.push_back(6);
//    for_each(iVecs.begin(), iVecs.end(), print());

    /*
     * bind1st, bind2nd 调用后, 返回值变为一元函数对象.
     *
     * */
//    printEx1 pEx;
//    // bind1st 把输入的参数绑到第一个位置 v1
//    for_each(iVecs.begin(), iVecs.end(), bind1st(pEx, 10));
//    // bind2nd 把输入的参数绑到第二个位置 v2
//    for_each(iVecs.begin(), iVecs.end(), bind2nd(pEx, 3));

    testNot();
    return 0;
}
