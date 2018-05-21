#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stdlib.h>
#include <memory>

using namespace std;

class Test
{
public:
    Test(string s)
    {
        str = s;
        cout<<"Test creat\n";
    }
    ~Test()
    {
        cout<<"Test delete:"<<str<<endl;
    }
    string& getStr()
    {
        return str;
    }
    void setStr(string s)
    {
        str = s;
    }
    void print()
    {
        cout<<str<<endl;
    }
private:
    string str;
};

int main(int argc, char *argv[]) {

    auto_ptr<Test> ptest(new Test("123"));
    auto_ptr<Test> ptest2(new Test("456"));
    ptest2 = ptest;
    ptest2->print();
    if(ptest.get() == NULL)cout<<"ptest = NULL\n";
    return 0;
    return 0;
}