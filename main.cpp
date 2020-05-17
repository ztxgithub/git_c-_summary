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

    bool operator ==(const Teacher& rh)
    {
        if((this->id == rh.id) && (this->age == rh.age))
            return true;
        else
            return false;
    }

    void printT()
    {
        cout << "member fun id: " << this->id << " age:" << this->age << endl;
    }
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
 * 普通函数
 * */
void NormalFun(int i, int bindArg)
{
    if(i > bindArg)
    {
        cout << " i: " << i << endl;
    }
}

/*
 * 如果需要外部再传入一个参数 a, 使得遍历 vector 的元素大于 a 时, 才打印
 * 第一步: 自定义函数对象去继承父类 binary_function(2 个参数) unary_function(1 个参数)
 *
 * */
class print{
public:
    void operator()(int i)
    {
        cout << " i: " << i << endl;
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
//    auto itor = find_if(v.begin(), v.end(), not1(compareDefNot()));
//    if(itor != v.end())
//    {
//        cout << "find_if true " << *itor << endl;
//    } else{
//        cout << "find_if not true " << endl;
//    }

    /*　
     * not2 二元函数对象取反
     * 对元素进行从大到小,
     * */
    sort(v.begin(), v.end(), not2(less<int>()));
    for_each(v.begin(), v.end(), print());
}

/*
 *  ptr_func 普通函数适配器
 *  如果对普通函数进行绑定参数
 *  1. 需要将普通函数转化为函数对象 ptr_fun
 *  2. 再使用　Bind 适配器(例如 bind2nd()) 进行参数绑定
 * */
void testPtrFun()
{
    vector<int> v;
    for(auto i = 0; i < 10; i++)
    {
        v.push_back(i);
    }
    for_each(v.begin(), v.end(), bind2nd(ptr_fun(NormalFun), 5));
}

/*
 *  mem_fun 修饰成员函数
 *  希望用类的普通成员函数进行打印
 *  1.使用　mem_fun_ref
 * */
void testMemFun()
{
//    vector<Teacher> vTs;
//    Teacher t1(1, 2), t2(3, 4), t3(5, 6);
//    vTs.push_back(t1);
//    vTs.push_back(t2);
//    vTs.push_back(t3);
//    for_each(vTs.begin(), vTs.end(), mem_fun_ref(&Teacher::printT));

    vector<Teacher*> vTs2;
    Teacher* tp1 = new Teacher(1, 2);
    Teacher* tp2 = new Teacher(2, 3);
    Teacher* tp3 = new Teacher(3, 4);
    Teacher* tp4 = new Teacher(4, 5);
    vTs2.push_back(tp1);
    vTs2.push_back(tp2);
    vTs2.push_back(tp3);
    vTs2.push_back(tp4);
    for_each(vTs2.begin(), vTs2.end(), mem_fun(&Teacher::printT));

}

class myPlus100
{
public:
    int operator()(int i)
    {
        return i + 100;
    }

};

/*
 *
 * */
void testTransform()
{
    vector<int> v;
    for(auto i = 0; i < 10; i++)
    {
        v.push_back(i);
    }

    vector<int> vDst;
    vDst.resize(10);
    transform(v.begin(), v.end(), vDst.begin(), myPlus100());
    for_each(vDst.begin(), vDst.end(), print());

}

void testFind()
{
    vector<Teacher> vTs;
    Teacher t1(1, 2), t2(3, 4), t3(5, 6);
    vTs.push_back(t1);
    vTs.push_back(t2);
    vTs.push_back(t3);

    auto iter = find(vTs.begin(), vTs.end(), Teacher(1, 1));
    if(iter != vTs.end())
    {
        cout << "find " << endl;
    } else{
        cout << "not find " << endl;
    }
}

void test_adjacent_find()
{
    vector<int> v;
    v.push_back(0);
    for(auto i = 0; i < 10; i++)
    {
        v.push_back(i);
    }


    auto iter = adjacent_find(v.begin(), v.end());
    if(iter != v.end())
    {
        cout<< "adjacent_find *iter " << *iter << endl;
    }
}

void test_count()
{
    vector<int> v;
    v.push_back(0);
    for(auto i = 0; i < 10; i++)
    {
        v.push_back(i);
    }


    int countNum = count(v.begin(), v.end(), 0);

    cout<< "countNum: " << countNum << endl;

}

void merge_test()
{
    vector<int> v;
    v.push_back(1);
    v.push_back(2);

    vector<int> v1;
    v1.push_back(1);

    vector<int> v3;
    v3.resize(v.size() + v1.size());
    merge(v.begin(), v.end(), v1.begin(), v1.end(), v3.begin());
    for_each(v3.begin(), v3.end(), print());

}

void modify(int &v)
{
    v += 10;
}
void for_each_modify_test()
{
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    for_each(v.begin(), v.end(), modify);
    for_each(v.begin(), v.end(), print());
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

//    testNot();
//    testPtrFun();
//    testMemFun();
//    testTransform();
//    testFind();
//    test_adjacent_find();
//    test_count();
//    merge_test();
    for_each_modify_test();
    return 0;
}
