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

### 内存对齐

```shell
    1.采用内存对齐的原因:
        (1) 在不同的平台上便于移植
        (2) 性能问题: 便于处理器更方便的取数据
                现在考虑 4字节存取粒度的处理器取 int类型变量（32位系统）,该处理器只能从地址为4的倍数的内存
                开始读取数据. 假如没有内存对齐机制，数据可以任意存放,
                现在一个int变量存放在从地址非4倍数开始的连续四个字节地址中,　需要取多次内存地址才能把数据取完全
                
    2.对齐规则
        (1) 如果设置了内存对齐为 i 字节(#pragma pack(i)), 类中最大成员对齐字节数为j, 那么整体对齐字节n = min(i, j) 
            (类中成员数据的对齐字节数定义：如果该成员是c++自带类型如int、char、double等,
             那么成员数据的其对齐字节数=该类型在内存中所占的字节数； 如果该成员是自定义类型如某个class或者struct,
             那个该成员的对齐字节数 = 该类型内最大的成员对齐字节数
             
             例如:
                class temp 
                { 
                    char c; 
                    int i; 
                    short s1; 
                };
                
                class node
                
                {
                
                    char c; //放在位置0，位置区间[0]
                    
                    temp t; //4（temp的对齐字节数） = n, 那么放置起始位置应该是4的倍数，即4，位置区间为[4~15]
                    
                    short s; //2 < n，那么放置起始位置应该是2的倍数，即16，位置区间为[16~17]
                
                }
                
                class node 的对齐字节是4字节(因为编译器没有显式设置内存对齐), node.c 字节对齐后为 4字节, 
                node.t 字节对齐后为　12, node.s 字节对齐后为　4, 所以sizeof(node) 为　20字节
                
        (2) 每个成员对齐规则：类中第一个数据成员放在offset为0的位置；对于其他的数据成员(假设该数据成员对齐字节数为k),
            他们放置的起始位置offset应该是 min(k, n) 的整数倍, 其中 k 为这个数据成员的对齐字节数, 而　n 则为整个数据结构
            整体的对齐字节数
            
            例如:
            
                class node
                
                {
                
                    char c;   //放在位置0，位置区间[0]
                    short s; //min(2,n = 4) -> 2, 那么short s的放置起始位置应该是2的倍数                
                    int i;      //4 = n, 那么放置起始位置应该是4的倍数，即4，位置区间为[4~7]
                 
                }
                
        (3) 整体对齐规则：最后整个类的大小应该是n的整数倍,  n 则为整个数据结构体的对齐字节数(参照规则1)
        (4)当设置的对齐字节数大于类中最大成员对齐字节数时,这个设置实际上不产生任何效果；
           当设置对齐字节数为1时，类的大小就是简单的把所有成员大小相加
           
    3.类继承时的内存对齐
        class A
        {
        
            int i;    
            char c1;
        
        }
        
        class B:public A
        
        {
            char c2;
        
        }
        
        不同的编译器结果不一样：
        
            gcc中：C相当于把所有成员i、c1、c2当作是在一个class内部，，(先继承后对齐)
        
        　　 vs中：对于A，对齐后其大小是8；对于B, c2加上对齐后的A的大小是9, 对齐后就是12(先对齐后继承)
                

```
