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
        
        A. 成员函数模板不能是虚函数(virtual)
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
        具体实际的调用方式,顺序如下:
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

