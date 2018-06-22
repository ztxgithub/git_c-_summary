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


class Parent;
typedef boost::shared_ptr<Parent> ParentPtr;

class Child : boost::noncopyable
{
    public:
        explicit Child(const ParentPtr& myMom_,
                       const ParentPtr& myDad_)
                : myMom(myMom_),
                  myDad(myDad_)
        {
        }

    private:
        boost::weak_ptr<Parent> myMom;
        boost::weak_ptr<Parent> myDad;
};

typedef boost::shared_ptr<Child> ChildPtr;

class Parent : boost::noncopyable
{
    public:
        Parent()
        {
        }

        void setSpouser(const ParentPtr& spouser)
        {
            mySpouser = spouser;
        }

        void addChild(const ChildPtr& child)
        {
            myChildren.push_back(child);
        }

    private:
        boost::weak_ptr<Parent> mySpouser;
        std::vector<ChildPtr> myChildren;
};

int main()
{
    ParentPtr mom(new Parent);
    ParentPtr dad(new Parent);
    mom->setSpouser(dad);
    dad->setSpouser(mom);
    {
        ChildPtr child(new Child(mom, dad));
        mom->addChild(child);
        dad->addChild(child);
    }
    {
        ChildPtr child(new Child(mom, dad));
        mom->addChild(child);
        dad->addChild(child);
    }
}