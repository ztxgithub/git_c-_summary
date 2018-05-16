## C++ 对象的内存布局

### 对象影响因素
```c++
    以下因素通常是C++这门语言在语义方面对对象内部的影响因素
        (1) 成员变量
        (2) 虚函数（产生虚函数表）
        (3) 单一继承（只继承于一个类）
        (4) 多重继承（继承多个类）
        (5) 重复继承（继承的多个父类中其父类有相同的超类）
        (6) 虚拟继承（使用virtual方式继承，为了保证继承后父类的内存布局只会存在一份）
```

### 单一的一般继承

```shell
    1. 实例代码:
        class Parent {
        public:
            int iparent;
            Parent ():iparent (10) {}
            virtual void f() { cout << " Parent::f()" << endl; }
            virtual void g() { cout << " Parent::g()" << endl; }
            virtual void h() { cout << " Parent::h()" << endl; }
         
        };
         
        class Child : public Parent {
        public:
            int ichild;
            Child():ichild(100) {}
            virtual void f() { cout << "Child::f()" << endl; }
            virtual void g_child() { cout << "Child::g_child()" << endl; }
            virtual void h_child() { cout << "Child::h_child()" << endl; }
        };
         
        class GrandChild : public Child{
        public:
            int igrandchild;
            GrandChild():igrandchild(1000) {}
            virtual void f() { cout << "GrandChild::f()" << endl; }
            virtual void g_child() { cout << "GrandChild::g_child()" << endl; }
            virtual void h_grandchild() { cout << "GrandChild::h_grandchild()" << endl; }
        };
        
     具体分布情况:
        虚函数表->Parent.iparent -> Child.ichild -> GrandChild.igrandchild

    子类实例的内存分布:
        (1) 虚函数表在最前面的位置。
        (2) 成员变量根据其继承和声明顺序依次放在后面。
        (3) 在单一的继承中，被overwrite的虚函数在虚函数表中得到了更新
```

### 继承相关的内存分布

```shell
    基类 class B
       {
           int b;
       public:
           virtual ~B(){ cout <<"B::~B()"<<endl; }
           virtual C(){ cout <<"B::C()"<<endl; }
       };
       
       sizeof(B) = 16, 因为在64为linux系统,sizeof(b) 为4字节,实例对象开始地址是虚函数表,是一个指针指向虚函数表,
       所以不管虚函数有多少个都对应一个指针,再根据字节对齐.对象中定义成员函数sizeof是不计算在内的.
       
       class D: public B
       {
           int i;
       public:
           virtual ~D() { cout <<"D::~D()"<<endl; }
       };
       
       如果 定义了 B *pb = new D[2];, 如果定义了继承数组,一定要确保基类和子类的内存实例大小相同(sizeof(B) == sizeof(D))
       因为当取第二个对象时,虚表找到了一个错误的内存上，内存乱掉了
       
       delete [] pb (如果sizeof(B) != sizeof(D),则会出现段错误)

```
