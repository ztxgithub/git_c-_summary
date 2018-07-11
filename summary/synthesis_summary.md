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
      
    2.static静态成员变量不能在类的内部初始化
    3.const成员变量也不能在 类定义处初始化 ,只能通过构造函数初始化列表进行,并且必须有构造函数
    4.const数据成员 只在某个对象生存期内 是常量,而对于整个类而言却是可变的.因为类可以创建多个对象,
      不同的对象其const数据成员的值可以不同
      
        class Test{
                public:
                    Test():a(0){}
                    enum {size1=100,size2=200};
                private:
                    const int a;//只能在构造函数初始化列表中初始化
                    static int b;//在类的实现文件中定义并初始化
                };
         
        类内static成员变量的初始化(定义)
        数据类型 类名::静态数据成员名=值； 
        int Test::b=0;//static成员变量不能在构造函数初始化列表中初始化，因为它不属于某个对象。
        
        注意:
            1.类的静态成员变量的定义　 int Test::b; 只能在一个文件内进行定义,不能在main()函数内和其他普通的函数内定义
            2. 类的静态成员变量的在定义的同时也可以进行初始化
            
        
    5.初始化在类体外进行,而前面不加static,以免与一般静态变量或对象相混淆
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
                    
        (4) operator new /operator delete的实现可以基于malloc，而malloc的实现不可以去调用new
        
    2. 三种new
    
        1.最常用的 new 表达式
            A *obj = new A();  //使用 new 创建对象　new 表达式
            
            使用 new 表达式会做以下2件事：
                (1) 调用函数 operator new 来分配空间
                (2) 调用 placement new 来进行构造函数
                
        2. 全局 operator new
                只是用来分配内存空间
                
        3. placement new
            placement new 的功能就是 在一个 已经分配好的空间上，调用构造函数，创建一个类。
            已经分配好的空间” 可以是任何的空间，比如说 可以是栈上的空间！
            
            例如:
                class A {...}  //声明一个 类 A
                   
                void *buf =  malloc(sizeof(A));   //简单地分配空间。
                A *ojb = new (buf)A();    // 在分配的空间上调用构造函数
            
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
