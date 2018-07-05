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


template<class T>
class Functor
{
public:
    enum Type{Plus,Sub};
    Functor(Type t = Plus) :type(t){}
    T operator()(T a, T b)
    {
        if (type == Plus) return a + b;
        return a - b;
    }
private:
    Type type;
};
int main()
{
    //同时定义了一个加法器和一个减法器。
    Functor<int> plus(Functor<int>::Plus);
    Functor<int> sub(Functor<int>::Sub);
    int a = 5, b = 3;
    cout << plus(a, b) << endl;
    cout << sub(a, b) << endl;
    cout << Functor<int>::Sub << endl;
    return 0;
}
