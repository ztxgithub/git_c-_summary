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

template<typename It>
auto sum(It beg, It end) -> decltype(*beg) {
    decltype(*beg) ret = *beg;
    for (It it = beg + 1; it != end; it++) {
        ret  = ret + *it;
    }
    return ret;
}

int main()
{
    auto sum_value = sum<long,int,int>(1,2);
    printf("sum_value[%d]\n", sum_value);
//    auto ret = sum<long>(1L, 23); //错误
//    auto ret = sum<long,int,int>(1L, 23); //正确
}

