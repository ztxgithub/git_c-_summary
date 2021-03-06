## i++ 和 ++i的区别

```shell
    1.理论上++i更快
    2.
        (1) i++ 返回的是临时变量(右值)
            //i++实现代码为：                                    
            int operator++(int)                                  
            {
                int temp = *this;                                     
                ++*this;                                             
                return temp;                                    
            }//返回一个int型的对象本身
            
        (2) ++i 返回的是引用本身(左值)
            // ++i实现代码为：
            int& operator++()
            {
                *this += 1;
                return *this;
            }//返回一个int型的对象引用

           
```

## vector 相关属性的概念

```shell
    1.size是当前vector容器真实占用的大小
    2.capacity是指在发生realloc前能允许的最大元素数，即预分配的内存空间
    3.reserve只是预留空间大小,并没有实际创建元素,所以不能引用其中的元素
        vector<int> a;
        a.reserve(10);
        cout << "a.capacity():"<< a.capacity() << "a.size():"<< a.size() << endl;
        
        结果:
            a.capacity():10 ;a.size():0
            
      注意:
        reserve(10) 预留了10对象空间,但是容器内并没有实际的对象,直接a[i] 访问会发送数组越界(异常)
        
    4.resize 分配空间，同时改变capacity和size,容器内的对象内存空间是真正存在的
            

```

## static const 概念

```shell
    1.即使没有具体对象,也能调用类的静态成员函数和成员变量.一般类的静态函数几乎就是一个全局函数,
      只不过它的作用域限于包含它的文件中.
      
    2.static 静态成员变量不能在类的内部初始化
    3.const 成员变量也不能在 类定义处初始化 ,只能通过构造函数初始化列表进行,并且必须有构造函数
    4.const 数据成员 只在某个对象生存期内是常量,而对于整个类而言却是可变的.因为类可以创建多个对象,
      不同的对象其 const 数据成员的值可以不同
      
        class Test{
                public:
                    Test():a(0){}
                    enum {size1=100,size2=200};
                private:
                    const int a;//只能在构造函数初始化列表中初始化
                    static int b;//在类的实现文件中定义并初始化, 源文件的类外进行初始化
                };
         
        类内static成员变量的初始化(定义)
        数据类型 类名::静态数据成员名=值； 
        int Test::b = 0;//static成员变量不能在构造函数初始化列表中初始化，因为它不属于某个对象。
        
        注意:
            1.类的静态成员变量的定义　 int Test::b; 只能在一个文件内进行定义,不能在main()函数内和其他普通的函数内定义
            2. 类的静态成员变量的在定义的同时也可以进行初始化
            
        
    5.初始化在类体外进行,而前面不加 static,以免与一般静态变量或对象相混淆
    6.在静态成员函数的实现中不能直接引用 类内非静态成员(因为不同的对象其非静态成员值不同),可以引用类内静态成员.
    7.普通的类的非静态成员可以在声明时就进行初始化
            class Test{
            public:
                int var12 = 4; //正确
              
            private:
                int var11 = 4; //正确
            
            };
            
    8.静态数据成员被类的所有对象所共享,包括该类派生类的对象,即派生类对象与基类对象共享基类的静态数据成员
    9.静态数据成员的类型可以是 所属类的类型, 而普通数据成员则不可以,普通数据成员的只能声明为所属类类型的指针或引用
            
            class base{ 
            public : 
                static base base_object1;//正确，静态数据成员 
                base _object2;//错误 
                base *pObject;//正确，指针 
                base &mObject;//正确，引用 
            };
            
    10.静态成员函数的地址可用普通函数指针储存, 而 普通成员函数地址 需要用类成员函数指针来储存
            
            class base{ 
                static int func1(); 
                int func2(); 
            }; 
             
            int (*pf1)()=&base::func1;//普通的函数指针 
            int (base::*pf2)()=&base::func2;//成员函数指针
            
    11.静态成员函数不可以同时声明为 virtual、const、volatile函数
            
            class base{ 
                virtual static void func1();//错误 
                static void func2() const;//错误 
                static void func3() volatile;//错误 
            };
            
    12. 类内的非静态成员函数也是只有一份, 但它可以处理多个不同值的对象, 是因为编译器中隐含 this 指针, 指向对象的地址. 
        而类内的静态对象也只有一份, 但只能用于类内静态变量, 不能用于类内的非静态变量
    13. 可以用于单例模式
            class A{
                public:
                    static A& GetInstance(){return a;}
                    setup() {....}
                    
                private:
                    A();
                    A(const A& rhs);
                    static A a;
                    .....
            }

            A::GetInstance().setup();
```

## 前向声明(Forward Declaration)

```shell
    1. 前向声明的定义：有些时候我们可以声明一些类但是并不去定义它
        (1) 使用前置声明(forward declaration)可以减少编译依赖，这个技巧告诉compile指向 class/struct的指针，
            而不用暴露struct/class的实现
    2. 在声明完这个foo类之后，定义完这个foo类之前的时期，foo类是一个不完全的类型(incomplete type)，
       也就是说foo类是一个类型，但是这个类型的一些性质(比如包含哪些成员，具有哪些操作)都不知道。
       因此这个类的作用也很有限,即使用前向声明需要注意:
            (1) 不能定义foo类的对象
            (2) 可以用于定义指向这个类型的指针或引用 (很有用)
            (3) 用于声明(不是定义)使用该类型作为形参或者返回类型的函数。
            
    3. 原因:
            (1) 避免头文件的循环引用
            (2) 避免引入头文件
            
    4. 使用规则： 
            在c++中，如果要为类编写头文件的话，一般是要#include一堆头文件的，但利用前向声明和c++编译器的特性可以
            避免 include 头文件
            
            (1) 由于所有对象类型的所占用的空间都是相同的,只需要做一个string的前向声明就可以了，
                不需要#include <string>
                
                    class string;
                    class Sample
                    
                    {
                    
                    	private:
                    		string &s;
                    };
                    
            (2) 由于所有类型的指针也是相同大小的,也可以只做前向声明就好。
                
            (3) 声明成员函数的形参或者是返回类型，也可以利用前向声明的性质
                    class string;
                    class foo;
                    class Sample 
                    {
                    
                    	public:
                    		foo foo_test(foo &);
                    	private:
                    		string &s;
                    		foo *f;
                    };
                    
    5. 例子:
            (1) 两个类之间的相互引用问题,定义了类A类B，A中使用了B定义的类型，B中也使用了A定义的类型,
                那么就得使用前向申明(针对头文件而言(.h))，
                一般的做法是：
                    两个类的头文件之中，选一个包含另一个类的头文件，但另一个头文件中只能采用class *的申明形式，
                    而在实现文档中（*.cpp）中包含头文件
                    
                    在　A.h　头文件中
                    
                        #include "B.h"
                        class A
                        {
                             int i;
                             B b;　　// 因为 include "B.h" 可以知道 类型B 具体包含那些内容，所以可以定义变量
                        }　
                        
                    在　B.h 头文件中
                    
                        class A;
                        class B
                        {
                            int i;
                            A *a;  // 没有　include "A.h" ,只能采用前向申明，只能定义指向这个类型的指针
                        }
                        
                    在 B.cpp 源文件中
                    
                        #include "A.h"
                        B::B()
                        {
                            …　　// 因为要使用对象 A 的值，只能 include "A.h"　
                        }
                        
    6. 不推荐使用前向申明
            (1) delete 一个不完整类型的指针时，如果这个类型有 non-trivial 的析构函数，那么这种行为是未定义的。
                把前置声明换成 #include 则能保证消除这种风险
                
                
```

## 在构造函数和析构函数中抛出异常会发生什么？

```shell
    1.构造函数抛异常：(构造函数可以抛异常)
        C++ 里面当构造函数抛出异常时,其会调用构造函数里面已经创建对象的析构函数(如果创建了其他类的对象,会调用对应类的析构函数),
        但是对以自己的析构函数没有调用,就可能产生内存泄漏，比如自己new 出来的内存没有释放
        
    2.析构函数抛异常：(析构函数的执行不应该抛出异常)
      
      若析构函数因异常退出，情况会很糟糕(all kinds of bad things are likely to happen)
         不应该在析构函数中抛异常的原因:
            (1) 如果析构函数抛出异常,则异常点之后的程序不会执行,如果析构函数在异常点之后执行了某些必要的动作比如释放某些资源,
                则这些动作不会执行,会造成诸如资源泄漏的问题.
                
            (2)  析构函数中抛出异常还要注意栈展开带来的程序崩溃
                通常异常发生时,c++的异常处理机制在异常的传播过程中会进行栈展开（stack-unwinding）,
                因发生异常而逐步退出复合语句和函数定义的过程,
                被称为栈展开(抛出异常时,将暂停当前函数的执行,开始查找匹配的catch子句.
                首先检查throw本身是否在try块内部如果是,检查与该try相关的catch子句,看是否可以处理该异常,
                如果不能处理,就退出当前函数,并且释放当前函数的内存并销毁局部对象,继续到上层的调用函数中查找,
                直到找到一个可以处理该异常的catch。).
                
                在栈展开的过程中就会调用已经在栈构造好的对象的析构函数来释放资源,此时若其他析构函数本身也抛出异常,
                则前一个异常尚未处理,又有新的异常，会造成程序崩溃.
                
         一定要在析构函数中抛异常的解决方案:
            把异常完全封装在析构函数内部,决不让异常抛出析构函数之外
                ~ClassName()
                {
                     try{
                          do_something();
                      }
                      catch(…){  //这里可以什么都不做，只是保证catch块的程序抛出的异常不会被扔出析构函数之外
                       }
                }
                
    3.异常
        (1) 异常调用流程
                 A方法掉调用--> B方法调用 -->C方法, 然后在B和C方法里定义了throws Exception,A方法里定义了Try Catch。
                 那么调用A方法时,在执行到C方法里出现了异常,那么这个异常就会从C抛到B,再从B抛到A.
                 在A里的try catch就会捕获这个异常，然后你就可以在catch写自己的处理代码.
                 
                 那么为什么当时出现了异常不去处理呢？ 因为你业务逻辑调用的是A方法,你执行了A方法,当然要在A里得到异常,
                 然后来处理.如果在C里面就处理异常,这就破坏程序结构了. 另外,A调用了C方法,假如还接着也调用了D,E,F方法,
                 假如他们都有可能抛出异常,你说是在A里面获得处理一次好,还是在C,D,E,F得到了异常,每个都当时处理一下的好？
                 当时就处理异常 理论上也是可以的,而且大多数时候,到底在哪处理异常，是要根据需求和项目的具体情况的。
     

```

## 常用的实现 隐式类类型转换 的方式

```shell
    隐式类类型转换:
    
        (1) 使用单参数的构造函数 或 N个参数中有N-1个是默认参数的构造函数，如
                class A
                {
                    public：
                          A(stirng s);
                          A(string s,int a = 0);
                };
                
                A A_object = "str" ; // 正确  直接将string类型隐式转化为　类A
                A B_object("str") ;  // 正确　标准的构造函数
                
        (2) 使用operator目标类型() const
                class Rational {
                public:
                  ...
                  operator double()const;                  // 转换Rational类成double类型
                };
            
    优点:
        代码简单
        
    缺点:
        容易隐藏类型不匹配的错误；
        代码更难阅读,不易维护
        
    规范:
        可接收单个参数的构造函数必需要加上explicit标记，禁止隐式类类型转换,例如:
            class A
            {
                public：
                       explicit A(stirng s);
                       explicit A(string s,int a = 0);
            };
            
             A A_object = "str" ; //错误　编译不通过，不允许隐式的转换
            

```

## new 和 malloc的区别

```shell
    1.区别
        (1) new 不仅分配了内存空间, 而且调用了构造函数. 而 malloc 仅仅分配了内存空间
        (2) 返回类型安全性
                new操作符内存分配成功时,返回的是对象类型的指针,无须进行类型转换，故new是符合类型安全性的操作符
                malloc内存分配成功则是返回void * ,需要通过强制类型转换将void*指针转换成我们需要的类型
                
        (3) 内存分配失败时的返回值
                new内存分配失败时,会抛出bac_alloc异常,它不会返回NULL；
                malloc分配内存失败时返回NULL.
                
                    try
                    {
                        int *a = new int();
                    }
                    catch (bad_alloc)
                    {
                        ...
                    }
                    
        (4) operator new /operator delete的实现可以基于malloc，而 malloc 的实现不可以去调用 new
        
    2. 三种 new
    
        1.最常用的 new 表达式, 
          new operator : 指 C++ 语法中 new 关键字
            A *obj = new A();  //使用 new 创建对象　new 表达式
            
            使用 new 表达式会做以下 2 件事：
                (1) 调用函数 operator new 来分配空间
                (2) 调用 placement new 来进行构造函数
                
        2. 全局 operator new  --> ::operatro new(size_t size)
                只是用来分配内存空间
                
            class A
            {
            public:
                A()
                {
                    std::cout<<"call A constructor"<<std::endl;
                }
             
                ~A()
                {
                    std::cout<<"call A destructor"<<std::endl;
                }
            }
            
            例如: A* p = (A*)::operator new(sizeof(A)); //只是分配内存,不进行构造
            
         2.1 如果类 A 重载了 operator new, 就使用该重载版本, 否则使用全局版本 ::operatro new(size_t size)
                class A
                {
                public:
                    A()
                    {
                        std::cout<<"call A constructor"<<std::endl;
                    }
                 
                    ~A()
                    {
                        std::cout<<"call A destructor"<<std::endl;
                    }
                    
                    void* operator new(size_t size)  // operator new 操作符
                    {
                        std::cout<<"call A::operator new"<<std::endl;
                        return malloc(size);
                    }
                 
                    void* operator new(size_t size, const std::nothrow_t& nothrow_value)
                    {
                        std::cout<<"call A::operator new nothrow"<<std::endl;
                        return malloc(size);
                    }
                };
          
                A* p1 = new A;
          
                A* p2 = new(std::nothrow) A;
              
  

        3. placement new
            placement new 的功能就是 在一个 已经分配好的空间上，调用构造函数，创建一个类。
            已经分配好的空间” 可以是任何的空间，比如说 可以是栈上的空间！
            
            例如:
                class A {...}  //声明一个 类 A
                   
                void *buf =  malloc(sizeof(A));   //简单地分配空间。
                // placement new 本身只是返回指针, new(buf) A() 表示调用 placement new 之后, 还会在 buf 上调用 A:A(),
                // buf 可以是动态分配的内存, 也可以是栈中缓冲, 如char buf[100]; new(buf) A();
                A *ojb = new(buf) A();    // 在分配的空间上调用构造函数
                A *ojb = new (buf)A();    // 在分配的空间上调用构造函数
                
            placement new 本身就是 operator new 的一个重载, 不需也尽量不要对它进行改写, 一般是搭配 new(p) A(); 只需简单返回指针
            就可以了
            
        4. operator new 运用技巧
                (1) operator new 重载运用于调试, 可以增加 2 个输入参数,文件名和所在的行数
                        
                        class A
                        {
                        public:
                            A()
                            {
                                std::cout<<"call A constructor"<<std::endl;
                            }
                         
                            ~A()
                            {
                                std::cout<<"call A destructor"<<std::endl;
                            }
                         
                            void* operator new(size_t size, const char* file, int line)
                            {
                                std::cout<<"call A::operator new on file:"<<file<<"  line:"<<line<<std::endl;
                                return malloc(size);
                                return NULL;
                            }
                         
                        };
                        
                        
                        A* p1 = new(__FILE__, __LINE__) A;
                        
                (2)  内存池优化
                        内存池的一个常见策略就是分配一次性分配一块大的内存作为内存池(buffer 或 pool)，然后重复利用该内存块，
                        每次分配都从内存池中取出，释放则将内存块放回内存池。在客户端调用的是 new 关键字, 
                        可以改写 operator new 函数，让它从内存池中取出(当内存池不够时, 再从系统堆中一次性分配一块大的),
                        这样构造和析构则在取出的内存上进行, 然后再重载 operator delete, 它将内存块放回内存池
                        
        5. 其他
                (1) set_new_handler(int arg)  : arg 为 0 禁用, 1: 启用
                    set_new_handler 可以在 malloc(需要调用set_new_mode(1))或 operator new 内存分配失败时指定一个
                    入口函数 new_handler, 这个函数完成自定义处理(继续尝试分配，抛出异常，或终止程序),
                    如果 new_handler 返回，那么系统将继续尝试分配内存，如果失败，将继续重复调用它，直到内存分配完毕或
                     new_handler不再返回(抛出异常，终止)
                     
                     
                    #include <iostream>
                    #include <new.h>// 使用_set_new_mode和set_new_handler
                    void nomem_handler()
                    {
                        std::cout<<"call nomem_handler"<<std::endl;
                    }
                    int main()
                    {
                        _set_new_mode(1);  //使new_handler有效
                        set_new_handler(nomem_handler);//指定入口函数 函数原型void f();
                        std::cout<<"try to alloc 2GB memory...."<<std::endl;
                        char* a = (char*)malloc(2*1024*1024*1024);
                        if(a)
                            std::cout<<"ok...I got it"<<std::endl;
                        free(a);
                    
                    }
                    
                 (2) new 分配数组
                        class A
                        {
                        public:
                            A()
                            {
                                std::cout<<"call A constructor"<<std::endl;
                            }
                         
                            ~A()
                            {
                                std::cout<<"call A destructor"<<std::endl;
                            }
                         
                            void* operator new[](size_t size)
                            {
                                std::cout<<"call A::operator new[] size:"<<size<<std::endl;
                                return malloc(size);
                            }
                            
                            void operator delete[](void* p)
                            {
                                std::cout<<"call A::operator delete[]"<<std::endl;
                                free(p);
                            } 
                            void operator delete(void* p)
                            {
                                free(p);
                            } 
                        };
                  
                            
                    A* p1 = new A[3];  // 不是简简单单只分配了 sizeof(A) * 3, 在(32 bit)情况下还加了 4 字节,用来
                                       // 用于编译器区分对象数组指针和对象指针以及对象数组大小
                   
                  

                
        6. 参考资料 : https://blog.csdn.net/WUDAIJUN/article/details/9273339
            
```

## 函数指针 和　指针函数

```shell
    1.函数指针
        函数指针：指向函数的指针变量
        作用:
            (1) 可以用来调用函数
            (2) 做函数的参数
            
                int(*func)(int a, int b);
                
    2.指针函数
        指针函数本质上是一个函数,是指函数的返回值为指针 的函数
            int* func(int x,int y);
```

## C/C++的参数入栈顺序是从右向左

```shell
    1. 原因:
          使用C/C++的“函数参数长度可变”这个特性时更方便
           printf("%d %d %d",1,2,3）, 在采用从右向左的参数入栈顺序时, 参数出栈顺序时"%d %d %d",1,2,3.
           由于可变参的函数第一步就需要解析可变参数表的各参数类型(将后续保存的数据按什么类型进行解析), 
           这样可以按顺序的解析出各个参数的类型,比较方便操作
           如果采用从左向右的入栈顺序,则出栈顺序变为3,2,1,"%d %d %d",不便于解析.
```

##  拷贝构造函数用途, 什么时候需要自定义拷贝构造函数

```shell
    1.需用使用拷贝构造函数
        A. 一个对象以值传递的方式传入函数体
        B. 一个对象以值传递的方式从函数返回
        C. 一个对象需要通过另外一个对象进行初始化
        
    2.当类内的　数据成员变量　的内容是动态内存申请, 则需要自定义拷贝构造函数进行　深拷贝, 因为编译器默认的拷贝构造函数
    　是进行浅拷贝的
```

## 构造函数是否可以调用虚函数?

```shell
    不能,语法上通过(编译器通过), 但是　语义上有问题．
    在构造一个子类对象的时候,首先会构造它的基类,如果有多层继承关系,实际上会从最顶层的基类逐层往下构造
   　即 derived class对象内的 base class　对象会在derived class自身构造之前构造完毕。因此，
   　在base class的构造函数中执行的virtual函数将会是base class的版本, 决不会是derived class的版本。
```

## virtual函数能声明为内联函数码?

```shell
    不能, inline是编译期决定, 意味着在执行前就将 调用动作 替换 为被调用函数的本体；
         virtual是运行期决定，他意味着直道运行期才决定调用哪个函数.
```

## 哪些类型的对象不可以作为union的成员？为什么有这种限制？

```shell
    自定义 拷贝构造函数, 赋值操作符, 析构函数, 派生类　都不能作为　union的成员.
    
    主要是为了兼容C
```

## POD(plain old data) 类型

```shell
    1.定义:
        将对象的各字节拷贝到一个字节数组中，然后再将它重新拷贝到原先的对象所占的存储区中，此时该对象应该具有它原来的值。
        
        其实POD本质就是与c兼容的数据类型
        
    2.特点:
        所有POD类型都可以作为union的成员
```

## C++ 经验

```shell
    1. 用异或来交换变量是不可取的
            原因:
                (1) 丑化代码，不易阅读和维护
                (2) 效率与用 stl 相关的函数相差不大
                
    2. 用清晰的代码表达功能，让编译器容易优化
    3. 不要重载全局 operator new 
    4. 
        (1) [检测内存错误]使用 vargrind, dmalloc 
        (2) [统计内存使用数据] 使用 backtrace()
        (3) [内存性能优化] 使用 tcmalloc
        
    5. 在单元测试中代替(mock)系统调用
            方法一:
                通过运行时动态绑定虚函数,通过写一个基类 System_interface, 这里面定义了虚函数 open, close, 
                accept, getpeername. 正常程序则定义派生类重写虚函数 open()(这里的实现方式是用系统调用 open()),
                而在单元测试中如果要控制系统调用的返回值, 则新写一个派生类,该派生类重写虚函数 open() 等系统调用.
                那么在单元测试时,只要基类的指针指向该派生类,则在运行时就可以动态调用自己定义的系统函数了.
                
            方法二:
                通过编译时绑定不同的源文件,在原来的函数的基础上再封装一层.例如
                 SocketsOps.h
                    namespace sockets
                    {
                        int connect(int sockfd, const struct sockaddr_in& addr);
                    }
                    
                 SocketsOps.cpp (正式上线使用的源文件)
                 int sockets::connect(int sockfd, const struct sockaddr_in& addr)
                 {
                    return ::connect(sockfd, addr, sizeof(addr));   // 放回系统函数 connect
                 }
                 
                 MockSocketOps.cpp (单元测试时编译用该源文件)
                 int sockets::connect(int sockfd, const struct sockaddr_in& addr)
                  {
                     errno = EAGAIN;
                     return -1;
                  }
                  
                  这个时候如果要正式发布,则编译用 SocketsOps.cpp, 如果要进行单元测试,则编译用 MockSocketOps.cpp.
            上述两种方法好处是我们可以通过修改代码,只 mock 要测试的代码,对于第三方库(例如 sqlite)会用到系统调用,则不
            影响,还是会正常使用系统调用.
            
            方法三:
                和方法二类似,只不过将该源文件改为编译成动态链接库
                
    6. 慎用匿名 namespace
            1. namespace中的成员(变量和函数),具有独一无二的全局名称,避免名称碰撞.
            2. 不好的原因:
                    (1) 匿名 namespace 中函数是"匿名", 所以在使用 gdb 调试时,不方便在其中设置断点,
                        因为如果 a.cc 和 b.cc 在各自的文件中定义的匿名空间中的 foo() 函数，
                        gdb 则无法区分这两个函数
                            错误的方法:
                                namesapce             // 其中 只有 namesapce 是匿名
                                {
                                    void foo();
                                }
                                
                            正确的方法: 使用普通具名 namespace
                                 namesapce detail      // namesapce + 变量名(detail) : 具名
                                    {
                                        void foo();
                                    }
                                
                    (2) 在某些编译器中(g++4.2.4),编译包含匿名空间的源文件，其每次编译后的二进制文件的 md5 是不同的
                     　　因为编译器会随机给匿名 namespace 生成一个唯一的 ID
                     
            3. 解决方案:
                    使用普通具名 namespace, 例如 
                         namesapce detail      // namesapce + 变量名(detail) : 具名
                        {
                            void foo();
                        }
                        
     
    7. 采用有利于版本管理的代码格式
            1. 对 diff 友好的代码格式
                    (1) 即使是多行注释，也要用 "//" , 而不能用 "/**/"
                        因为一般用 beyond compare, 或则是版本管理时进行代码比较是逐一按行比较，如果
                        你用 "/**/" 进行多行注释，则比较的时候无法看到是注释里的代码改动还是正常的代码改动．
                        
                 　 (2) 一行只定义一个变量更有利于版本管理，这个规则同样也适用与 enum 成员的定义
                    (3) 对于 namespace ,　class, 不进行缩进，这样对源代码进行 diff 时, chunk name
                        就是函数名,让人一看就知道哪个函数进行修改
                        例如:
                            namespace detail // 顶格,不加空行,不缩进
                            {
                            
                            class A()  // 顶格,不加空行,不缩进
                            {
                                .....
                            }
                            
                            }
                            
                        这样 diff 后会得到
                        --- a/test.cc
                        +++ b/test.cc
                        @@......@@ class A()    //这里 chunk name 就是 class 名,很容易就可以看到哪个 class 进行修改
                        -   .......
                        +   ......
                                  
```
 


