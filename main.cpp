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

using namespace std;




class RVO
{
public:
    RVO(){printf("I am in constructor\n");}
    RVO (const RVO& c_RVO) {printf ("I am in copy constructor\n");}
    ~RVO(){printf ("I am in destructor\n");}
    int mem_var;
};
RVO MyMethod (int i)
{
    RVO rvo;
    rvo.mem_var = i;
    return (rvo);
}
int main()
{
    RVO rvo;
    rvo=MyMethod(5);
}
