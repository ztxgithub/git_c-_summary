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

#include <set>
#include <iostream>

class B
{
    int b;
public:
    virtual ~B(){ cout <<"B::~B()"<<endl; }
//    virtual C(){ cout <<"B::C()"<<endl; }
     cc(){ cout <<"B::C()"<<endl; }
};

int main( )
{
    printf("sizeof(B):%d\n", sizeof(B));
    return 0;
}
