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

class Fraction
{
public:
    //non-explicit-one-argument ctor
    //one-argument:只要一个实参就够了，给两个也可以。two-parameter
    //non-explicit:没有添加explicit修饰
    //可以把int隐式的转换为Fraction
    explicit Fraction(int num, int den = 1)//这种默认是符合数学上的规定
            :m_numerator(num), m_denominator(den) {}
    Fraction operator+(const Fraction& f) {
        return Fraction(1,2);
    }


private:
    int m_numerator;	//分子
    int m_denominator;	//分母
};

int main()
{
    Fraction f = 3;
}

