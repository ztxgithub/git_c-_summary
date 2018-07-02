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
