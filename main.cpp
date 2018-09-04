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

class Bad {
public:
    Bad() { std::cout << "Bad()" << std::endl; }
    ~Bad() { std::cout << "~Bad()" << std::endl; }
    std::shared_ptr<Bad> getPtr() {
        return std::shared_ptr<Bad>(this);
    }
};

int main()
{
    std::shared_ptr<Bad> bp1(new Bad);
//    std::shared_ptr<Bad> bp2 = bp1->getPtr();
    std::shared_ptr<Bad> bp2 = bp1;
    std::shared_ptr<Bad> bp3 = bp2;
    std::cout << "bp2.use_count: " << bp2.use_count() << std::endl;
    std::cout << "bp1.use_count: " << bp1.use_count() << std::endl;
    std::cout << "bp3.use_count: " << bp3.use_count() << std::endl;
    return 0;
}

