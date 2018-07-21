# c++ 模板

## 概要

```shell
    1.模板的历史, 87年的时候, 泛型(Generic Programming)便被纳入了C++的考虑范畴,并直接导致了后来模板语法的产生.
      可以说模板语法一开始就是为了在C++中提供泛型机制.92年的时候, Alexander Stepanov开始研究利用模板语法制作 程序库,
      后来这一程序库发展成STL,并在93年被接纳入标准中
      
    2.实际上C++模板远没有想象的那么复杂.我们只需要换一个视角：在C++03的时候,模板本身就可以独立成为一门“语言”.
      它有“值”,有“函数”,有“表达式”和“语句”.除了语法比较蹩脚外,它既没有指针也没有数组,更没有C++里面复杂的继承和多态。
      可以说，它要比C语言要简单的多。如果我们把模板当做是一门语言来学习，那只需要花费学习OO零头的时间即可掌握。
      按照这样的思路，可以说在各种模板书籍中出现的多数技巧，都可以被轻松理解
      
    3.泛型编程作用
        C++是一门强类型语言,所以无法做到像动态语言(python)那样子,编写一段通用的逻辑,可以把任意类型的变量传进去处理.
        泛型编程弥补了这个缺点,通过 把通用逻辑设计 为模板, 摆脱了类型的限制, 提供了继承机制以外的另一种抽象机制,
        极大地提升了代码的可重用性.
        
        注意：
            模板定义本身不参与编译,而是编译器根据 使用模板时提供的具体类型参数生成代码, 再进行编译,这一过程被称为模板实例化.
            用户提供不同的类型参数,就会实例化出不同的代码.    
```

## 函数模板定义

```shell

    1.概要
          A. 函数模板可以声明为inline或者constexpr的,将它们放在template之后,返回值之前即可
    2.普通函数模板
        template<typename T>
        int compare(const T& left, const T& right) {
            if (left < right) {
                return -1; 
            }
            if (right < left) {
                return 1; 
            }
            return 0;
        }
        
    3.成员函数模板
        class Printer {
        public:
            template<typename T>
            void print(const T& t) {
                cout << t <<endl;
            }
        };
        
        Printer p;
        p.print<const char*>("abc"); //打印abc
        
        A. 成员函数模板 不能是虚函数(virtual)
                编译器在处理 类的定义的时候 要确定确定这个类的虚函数表的大小, 如果允许有类的虚成员模板函数,
                那么就必须要求编译器提前知道程序中 所有对该类的该 虚成员模板函数 的调用，而这是不可行的
        B. 模板类中可以使用虚函数
                template class<T>
                class A
                {
                    public:
                      virtual void f1(){cout<<"A is called"<<endl;}  //虚函数
                      virtual void f2()=0{cout<<"A=0"<<endl;}  //纯虚函数
                };
                
    4.实参推断
        为了方便使用, 除了直接为函数模板指定类型参数之外(compare<int>(1,2)),
        还可以从传递给函数的　实参推断类型参数,这一功能被称为模板实参推断.
        
    5.当返回值类型也是模板参数时
        当一个模板函数的返回值类型需要模板参数表示时,你无法利用实参推断获取全部的类型参数,
        这时有两种解决办法
            A. 办法一:返回值类型与参数类型完全无关,那么就需要显示的指定返回值类型,其他的形参类型交给实参推断
                    例如：　template<typename T1, typename T2, typename T3>, 指定参数类型需要从左到右逐一指定
                            
                        情况一:   
                            template<typename T1, typename T2, typename T3>
                            T1 sum(T2 v2, T3 v3) {
                                return static_cast<T1>(v2 + v3);
                            }
                            
                            /*
                             * 这里　T1是作为模板函数的返回值,所以显式指定为 long, 
                             * 而 T2,T3 是作为函数的形参,可以通过实参推测
                             */
                            auto ret = sum<long>(1L, 23); //正确
                            
                        情况二:
                            template<typename T1, typename T2, typename T3>
                            T3 sum(T1 v2, T2 v3) {
                                return static_cast<T3>(v2 + v3);
                            }
                            
                            /*
                             * 这里 long 指定的是　T1, 而 T1代表的是模板函数的参数(这个可以通过实参推断,不是必须的)
                             * 但是 T3 是必须指定的
                             */
                            auto ret = sum<long>(1L, 23); //错误
                            auto ret = sum<long,int,int>(1L, 23); //正确
                            
            B. 方法二: 返回值类型可以从模板形参类型中获得(可以通过函数写成尾置返回类型的形式 "->"
            
                        template<typename It>
                        auto sum(It beg, It end) -> decltype(*beg) {
                            decltype(*beg) ret = *beg;
                            for (It it = beg + 1; it != end; it++) {
                               ret  = ret + *it;
                            }
                            return ret;
                        }
                        
                        vector<int> v = {1, 2, 3, 4};
                        auto s = sum(v.begin(), v.end()); //s = 10
                        
    6.函数模板重载
        重载: 函数名一样, 对应的形参类型以及个数不相同
        函数模板, 普通函数　它们可以共存(它们可以定义同一个函数名)
        **********这个很重要***********
        具体实际的调用方式(不仅仅针对模板函数,也针对模板类),顺序如下:
            1.普通函数 -> 2.殊模板(限定了T的类型的,例如　T为指针,引用,容器等) -> 3.普通模板(对T没有任何限制的)
            
            template<typename T>
            void func(T t) { //通用模板函数
                cout << "In generic version template " << t << endl;
            }
            
            template<typename T>
            void func(T *t) { //指针版本
                cout << "In pointer version template "<< *t << endl;
            }
            
            void func(string* s) { //普通函数
                cout << "In normal function " << *s << endl;
            }
            
            int i = 10;
            func(i); //调用通用版本，其他函数或者无法实例化或者不匹配
            func(&i); //调用指针版本，通用版本虽然也可以用，但是编译器选择最特殊的版本
            string s = "abc";
            func(&s); //调用普通函数，通用版本和特殊版本虽然也都可以用，但是编译器选择最特化的版本
            func<>(&s); //调用指针版本，通过<>告诉编译器我们需要用template而不是普通函数
            
                            
                            
    7.模板函数特化
        有时 通用的函数模板 不能解决个别类型的问题,我们必须对此进行定制，这就是函数模板的特化。
        函数模板的特化必须把所有的模版参数全部指定.
        
            template<>
            void func(int i) {
                cout << "In special version for int "<< i << endl; 
            }
            
            int i = 10;
            func(i); //调用特化版本
                             
```

## template class 

```shell
    1.
        (1) Template Class声明：
        
                template <typename T> class ClassA;
                
        (2) Template Class定义：
        
                template <typename T> class ClassA
                {
                    T member;
                };
                
            在定义完模板参数T之后,便可以定义你所需要的类. 不过在定义类的时候,除了一般类可以使用的类型(int,char)外,
            你还可以使用模板参数中使用的类型 T.可以说，这个 T是模板的精髓,因为你可以通过指定模板实参,
            将T替换成你所需要的类型.
            通过模板参数替换类型,可以获得很多形式相同的新类型,有效减少了代码量。这种用法,
            我们称之为“泛型”(Generic Programming),它最常见的应用,即是STL中的容器模板类.
            
        (3) 模块的使用
                A.
                    模块变量定义的过程可以分成两步来看：
                        第一步：vector<int>将int绑定到模板类vector上,获得了一个“普通的类vector<int>”；
                        第二步通过“vector<int>””定义了一个变量.
                         
                        与“普通的类”不同，模板类是不能直接用来定义变量的。例如
                                vector unknownVector; // 错误示例
                B.            
                    我们把通过 类型绑定 将模板类 变成“普通的类”的过程,称之为模板实例化(Template Instantiate)
                    实例化的语法是：
                            模板名 < 模板实参1 [，模板实参2，...] >
                            template <typename T0, typename T1>
                             class ClassB
                            {
                                // Class body ...
                            };
                            
                            模板实例化:
                                ClassB<int, float>
                                
        (4)  模板类的成员函数定义
                A.方法一(常用,推荐使用)
                    定义　模板类的成员函数　通常都是以内联的方式实现
                        template <typename T>
                        class vector
                        {
                            public:
                                /*
                                 * 实现成员函数也在　类的定义　中进行, 以内联的方式
                                 */
                                
                                void clear()
                                {
                                    // Function body
                                }
                                
                            private:
                                T* elements;
                        };
                        
                B. 方法二　
                     定义实现部分放在类型之外
                        template <typename T>
                        class vector
                        {
                            public:
                                void clear(); // 注意这里只有声明
                            private:
                                T* elements;
                        };
                        
                        // 模板类成员函数的实现
                        template <typename T>
                        void vector<T>::clear()		// 函数的实现放在这里
                        {
                        	// Function body
                        }
                        
                        其中在成员函数实现的时候,必须要提供模板参数,类型名是用　vector<T>　成员函数的偏特化

```

## 整型也可是Template参数

```shell
    1.
        整型作为模板参数,其中这里的整型比较宽泛, 包括基本类型(int ,char,bool),同时也包括指针, 但是不能是 float
            template <int V> class TemplateWithValue;
            template <void *ptr> class TemplateWithValue;
        
        类型作为模板参数
            template <typename T> class TemplateWithType;
            
    2. 按照C++ Template最初的想法,模板不就是为了提供一个类型安全、易于调试的宏吗？有类型就够了,
       为什么要引入整型参数呢？考虑宏，它除了代码替换，还有一个作用是作为常数出现.
       所以整型模板参数最基本的用途,也是定义一个常数(很关键)
       例如这段代码的作用：
       
            template <typename T, int Size> 
            struct Array
            {
                T data[Size];
            };
            
            Array<int, 16> arr;
            
            便相当于下面这段代码：
            
            class IntArrayWithSize16
            {
                int data[16]; // int 替换了 T, 16 替换了 Size
            };
            
            IntArrayWithSize16 arr;
            
    3.注意
        模板的匹配是在编译的时候完成的,所以实例化模板的时候所使用的参数,也必须要在编译期就能确定
        
        template <int i> class A {};
        
        void foo()
        {
            int x = 3;
            A<5> a; // 正确！
            
            /* 因为x不是一个编译期常量,所以 A<x> 就会告诉你,
             * x是一个局部变量，不能作为一个模板参数出现
             */
            A<x> b; // error C2971: '_1_3::A' : template parameter 'i' : 'x' : a local variable cannot be used as a non-type argument
        }
```

# 模板元编程

```shell
    1.
    我们以数据结构举例。在程序里，你需要一些堆栈。这个堆栈的元素可能是整数、浮点或者别的什么类型。
    一份整型堆栈的代码可能是：
        class StackInt
        {
            public:
                void push(Int v);
                Int pop();
                Int Find(Int x)
                {
                    for(Int i = 1; i <= size; )
                    {
                        if(data[i] == x) { return i; }
                    }
                }
            // ... 其他代码 ...
        };
        
    如果你要支持浮点了，那么你只能将代码再次拷贝出来，并作如下修改：
    
    class StackFloat
    {
    public:
        void push(Float v);
        Float pop();
        Int Find(Float x)
        {
            for(Int i = 1; i <= size; )
            {
                if(data[i] == x) { return i; }
            }
        }
        // ... 其他代码 ...
    };
    
    当然也许你觉得这样做能充分体会代码行数增长的成就感。但是有一天，你突然发现：呀，Find 函数实现有问题了。
    怎么办？这个时候也许你只有两份这样的代码，那好说，一一去修正就好了。如果你有十个呢？二十个？五十个？
    时间一长，你就厌倦了这样的生活。你觉得每个堆栈都差不多，但是又有点不一样。
    
    于是便诞生了新的技术,来消解我们的烦恼.
    
    这个技术的名字,并不叫“模板”，而是叫“元编程”.
    元(meta) 无论在中文还是英文里,都是个很“抽象(abstract)”的词.因为它的本意就是“抽象”.元编程,
    也可以说就是“编程的抽象”.用更好理解的说法,元编程意味着你撰写一段程序A,程序A会运行后生成另外一个程序B,
    程序B才是真正实现功能的程序.那么这个时候程序A可以称作程序B的元程序，撰写程序A的过程，就称之为“元编程”
    
    找出程序之间的相似性,进行“元编程”.而在C++中,元编程的手段,可以是宏,也可以是模板
    template<typtname T>
       class StackFloat
       {
       public:
           void push(T v);
           T pop();
           Int Find(T x)
           {
               for(Int i = 1; i <= size; )
               {
                   if(data[i] == x) { return i; }
               }
           }typeid 
           // ... 其他代码 ...
       };
       
    typedef Stack<int>   StackInt;
    typedef Stack<float> StackFloat;
    
    通过模板,我们可以将形形色色的堆栈代码分为两个部分,一个部分是不变的接口,以及近乎相同的实现；
    另外一部分是元素的类型,它们是需要变化的。因此同函数模板类似，需要变化的部分，由模板参数来反应；
    不变的部分，则是模板内的代码。可以看出使用模板的代码，要比不使用模板的代码简洁许多
    
```

## 类模板的特化与偏特化

```shell
    1.根据类型执行不同的代码
            (1) 根据类型执行不同代码
                A. 模板
                B. 重载
                C. 虚函数 (dynamic_cast)
                
            (2) 在C语言时代，也会有一些技法来达到这个目的,比如下面这个例子,
                我们需要对两个浮点做加法， 或者对两个整数做乘法：
                   方法一:
                            struct Variant
                            {
                                union
                                {
                                    int x;
                                    float y;
                                } data;
                                uint32 typeId;
                            };
                            
                            Variant addFloatOrMulInt(Variant const* a, Variant const* b)
                            {
                                Variant ret;
                                assert(a->typeId == b->typeId);
                                if (a->typeId == TYPE_INT)
                                {
                                    ret.x = a->x * b->x;
                                }
                                else
                                {
                                    ret.y = a->y + b->y;
                                }
                                return ret;
                            }
                            
                   方法二:
                        更常见的是 void*:
                        
                        #define BIN_OP(type, a, op, b, result) (*(type *)(result)) = (*(type const *)(a)) op (*(type const*)(b))
                        void doDiv(void* out, void const* data0, void const* data1, DATA_TYPE type)
                        {
                            if(type == TYPE_INT)
                            {
                                BIN_OP(int, data0, *, data1, out);
                            }
                            else
                            {
                                BIN_OP(float, data0, +, data1, out);
                            }
                        }
                        
    2.特化
        特化(specialization)是根据一个或多个特殊的整数或类型,给出模板实例化时的一个指定内容, 特化是在编译期间执行
        
        
       A. 如果要实现类模板中　根据不同的数据类型调用不用的代码业务逻辑
         
             float f_a, f_b, f_c;
             f_c = AddFloatOrMulInt<float>::Do(f_a, f_b); //实现的是　f_c = f_a + f_b
             
             int i_a, i_b, i_c;
             i_c = AddFloatOrMulInt<int>::Do(i_a, i_b); //实现的是　i_c = i_a * i_b
             
             // 首先，要写出模板的一般形式（原型）
             template<typename T>
             class AddFloatOrMulInt
             {
                    static T Do(T a, T b)
                    {
                        // 在这个例子里面一般形式里面是什么内容不重要，因为用不上
                        // 这里就随便给个0吧。
                        return T(0);
                    }
             };
             
             // 其次，我们要指定T是int时候的代码，这就是特化：
             template <> class AddFloatOrMulInt<int>
             {
             public:
                 static int Do(int a, int b) // 
                 {
                     return a * b;
                 }
             };
             
             // 再次，我们要指定T是float时候的代码：
             template <> class AddFloatOrMulInt<float>
             {
             public:
                 static float Do(float a, float b)
                 {
                     return a + b;
                 }
             };
             
             
             类模板特化的演变方式:
             
             // 我们这个模板的基本形式是什么？
             template <typename T> class AddFloatOrMulInt;
             
             // 但是这个类，是给T是Int的时候用的，于是我们写作
             class AddFloatOrMulInt<int>
             // 当然，这里编译是通不过的。
             
             // 但是它又不是个普通类，而是类模板的一个特化（特例）。
             // 所以前面要加模板关键字template，
             // 以及模板参数列表
             template </* 这里要填什么？ */> class AddFloatOrMulInt<int>;
             
             // 最后，模板参数列表里面填什么？因为原型的T已经被int取代了。所以这里就不能也不需要放任何额外的参数了。
             // 所以这里放空。
             template <> class AddFloatOrMulInt<int>
             {
                 // ... 针对Int的实现 ... 
             }
             
       B.当模板实例化时提供的模板参数不能匹配到任何的特化形式的时候，它就会去匹配类模板的“原型”形式。
       C. 和继承不同,类模板的“原型”和它的特化类在实现上是没有关系的,并不是在类模板中写了 ID 这个Member,
          那所有的特化就必须要加入 ID 这个Member，或者特化就自动有了这个成员。完全没这回事
          
              template <typename T> class TypeToID
              {
              public:
                  static int const NotID = -2;
              };
              
              template <> class TypeToID<float>
              {
              public:
                  static int const ID = 1;
              };
              
              void PrintID()
              {
                  cout << "ID of float: " << TypeToID<float>::ID << endl; // Print "1"
                  /*
                   * 错误! TypeToID<float>使用的特化的类，这个类的实现没有NotID这个成员。
                   */
                  cout << "NotID of float: " << TypeToID<float>::NotID << endl; 
          　　　　 /*
                   * 错误!  TypeToID<double>是由模板类实例化出来的,它只有NotID，没有ID这个成员。
                   */
                  cout << "ID of double: " << TypeToID<double>::ID << endl; //
              }
              
          类模板和类模板的特化的作用,仅仅是指导编译器选择哪个编译,但是特化之间、特化和它原型的类模板之间,是分别独立实现的.
          所以如果多个特化、或者特化和对应的类模板有着类似的内容，很不好意思，你得写上若干遍
          
       D. 对所有的指针类型特化
       
                //模板原型
                template <typename T> class TypeToID
                {
                public:
                    static int const NotID = -2;
                };
                
                // 特化 float 数据类型
                template <> class TypeToID<float>
                {
                public:
                    static int const ID = 1;
                };
                
                //　特化所有类型的指针
                template <typename T> // 嗯，需要一个T,指代“任意类型
                class TypeToID<T*> // 我要对所有的指针类型特化，所以这里就写T*
                {
                public:
                    typedef T		 SameAsT;
                    static int const ID = 38;	// 用最高位表示它是一个指针
                };
                
                //输出结果:  ID of float*: 38
                cout << "ID of float*: " << TypeToID<float*>::ID << endl;  
                //输出结果:  ID of int*: 38
                cout << "ID of int*: " << TypeToID<int*>::ID << endl;    
                //输出结果:  ID of int*: 1
                cout << "ID of float: " << TypeToID< TypeToID<float*>::SameAsT >::ID << endl; 
                
       E.将指定的指针类型解引用
            template <typename T>
            class RemovePointer
            {
            };
            
            template <typename T>
            class RemovePointer<T*>	 // 特化各种指针类型
            {
            public:
                typedef T Result;
            };
            
            void Foo()
            {
            　　　// 喏，用RemovePointer后，那个Result就是把float*的指针处理掉以后的结果：float啦。
                RemovePointer<float*>::Result x = 5.0f; 
                std::cout << x << std::endl;
            }
        
       F. 对　int *　提供更加特殊的特化
                 //模板原型
                template <typename T> class TypeToID
                {
                public:
                    static int const NotID = -2;
                };
                template <typename T> // 嗯，需要一个T
                class TypeToID<T*>    // 我要对所有的指针类型特化，所以这里就写T*
                {
                public:
                    typedef T SameAsT;
                    static int const ID = 0x80000000; // 用最高位表示它是一个指针
                };
                
                template <> // 嗯，int* 已经是个具体的不能再具体的类型了，所以模板不需要额外的类型参数了
                class TypeToID<int*> // 嗯，对int*的特化。在这里呢，要把int*整体看作一个类型。
                {
                public:
                    static int const ID = 0x12345678; // 给一个缺心眼的ID
                };
                
                void PrintID()
                {
                    cout << "ID of int*: " << TypeToID<int*>::ID << endl;
                }
                
                
    3.名称查找
        A. 名称解析(Name resolution)
                (1) 模板定义中 能够出现 三类名称：
                        (A) 模板名称、或模板实现中所定义的名称
                                template <typename T> 
                                struct X 
                                {
                                    X* anthor;  //可以定义　模板名称
                                    X a;
                                };
                                
                        (B) 和 模板参数有关 <typename T> 的名称
                        
                                template <typename T> 
                                struct X 
                                {
                                    T t1;
                                    T* t2;
                                };
                                
                        (C) 模板定义所在的定义域内 能看到的名称
                                struct B { int v; };
                                 template <typename T> 
                                    struct X 
                                    {
                                       B b;
                                    };
                                    
                        错误定义:
                            template <typename T> 
                            struct X 
                            {
                               C c;
                            };
                                                                
                                    
        B. 在语法分析中, 如果名字查找和模板参数<typename T>有关,那么查找会延期到模板参数全都确定的时候,
           如果(模板定义内出现的)名字和模板参数无关,那么在模板定义处,就应该找得到这个名字的声明
           
        C. 依赖性名称(Dependent names)
                模板定义中的表达式和类型可能会依赖于模板参数<typename T>,并且模板参数会影响到名称查找的作用域和查找时机,
                 如果表达式中有操作数依赖于模板参数,那么整个表达式都依赖于模板参数,
                 名称查找延期到模板实例化时进行。并且定义时和实例化时的上下文都会参与名称查找.
                 依赖性表达式可以分为类型依赖（类型 指模板参数的类型）或值依赖
                 
        D. 按照标准的意思,名称查找会在模板定义和实例化时各做一次,分别处理非依赖性名称和依赖性名称的查找.
           这就是“两阶段名称查找”这一名词的由来
           
        E .实例代码分析
                template <typename T> struct Y
                {
                    // X可以查找到原型；
                    // X<T>是一个依赖性名称，模板定义阶段并不管X<T>是不是正确的。
                    typedef X<T> ReboundType;
                	
                    // X可以查找到原型；
                    // X<T>是一个依赖性名称，X<T>::MemberType也是一个依赖性名称；
                    // 所以模板声明时也不会管X模板里面有没有MemberType这回事。
                    typedef typename X<T>::MemberType MemberType2;
                	
                    // UnknownType 不是一个依赖性名称
                    // 而且这个名字在当前作用域中不存在，所以直接报错。
                    typedef UnknownType MemberType3;				
                };
                
                其中 　typedef typename X<T>::MemberType MemberType2, 
                加上 typename 原因:
                    对于用户来说,这其实是一个语法噪音.也就是说,其实就算没有它,语法上也说得过去.
                    事实上,某些情况下MSVC的确会在标准需要的时候,不用写typename.
                    但是标准中还是规定了形如 T::MemberType 这样的qualified id 在默认情况下不是一个类型,
                    而是解释为T的一个成员变量MemberType,只有当typename修饰之后才能作为类型出现
```

