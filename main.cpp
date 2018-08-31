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

template <typename T>
class RemovePointer
{
};

template <typename T>
class RemovePointer<T*>	// 祖传牛皮藓，专治各类指针
{
public:
    typedef T Result;
};

int a;
struct B { int v; };
template <typename T> struct X {
    B b;                  // B 是第三类名字，b 是第一类
    T t;                  // T 是第二类
    T* t2;
    X* anthor;            // X 这里代指 X<T>，第一类
    typedef int Y;        // int 是第三类
    Y y;                  // Y 是第一类
//    C c;                  // C 什么都不是，编译错误。
    void foo() {
        b.v += y;          // b 是第一类，非依赖性名称
        b.v *= T::s_mem;   // T::s_mem 是第二类
        // s_mem的作用域由T决定
        // 依赖性名称，类型依赖
    }
};

template <typename T1, typename T2, typename T3>
T1 foo(T2 a, T3 b)
{
    return static_cast<T1>(a + b);
}

//template <typename T2, typename T3>
//int foo<int, T2, T3>(T2 a, T3 b)
//{
//    return static_cast<int>(a + b);
//}

/*
 * 函数模板的全特化
 * */
template <>
int foo<int, int, int>(int a, int b)
{
    return static_cast<int>(a + b);
}

template <typename T0, typename T1 = void> struct DoWork;

template <typename T> struct DoWork<T> {};
template <>           struct DoWork<int> {};
template <>           struct DoWork<float> {};
template <>           struct DoWork<int, int> {};

DoWork<int> i;
DoWork<float> f;
DoWork<double> d;
DoWork<int, int> ii;

void Foo()
{
    RemovePointer<float*>::Result x = 5.0f; // 喏，用RemovePointer后，那个Result就是把float*的指针处理掉以后的结果：float啦。
    std::cout << x << std::endl;
}

int main()
{
    unique_ptr<int> pInt(new int(5));
    cout << *pInt;
}

