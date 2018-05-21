# C++ 闭包

## 简介

```c++
    1.闭包
        子函数可以使用父函数中的局部变量，这种行为就叫做闭包！
        
    2.函数是代码, 状态是一组变量,将代码和一组变量捆绑 (bind) ,就形成了闭包,内部包含 static 变量的函数, 不是闭包, 
      因为这个 static 变量不能捆绑. 你不能捆绑不同的 static 变量. 这个在编译的时候已经确定了.
      闭包的状态捆绑, 必须发生在运行时.
      
    3.C++ 里使用闭包有3个办法
        (1) 重载 operator()
        (2) lambda
        (3) boost::bind/std::bind
                    
```

## 重载 operator()

```shell
    1.因为闭包是一个函数+一个状态,这个状态通过隐含的 this 指针传入.所以闭包必然是一个函数对象. 
      因为成员变量就是极好的用于保存状态的工具, 因此实现 operator() 运算符重载, 该类的对象就能作为闭包使用. 
      默认传入的 this 指针提供了访问成员变量的途径
      例如:
            class MyFunctor
            {
                public:
                    MyFunctor(float f) : round(f) {}
                    int operator()(float f) { return f + round; }
                private:
                    float round;
            };
            
            float round = 0.5;
            MyFunctor f(round);

```

## C++ lambda 

```c++
        1.lambda 的好处
            标准 C++ 库中有一个常用算法的库，其中提供了很多算法函数，比如 sort() 和 find()．
            这些函数通常需要提供一个“谓词函数 predicate function”(用于进行条件判断的函数，所谓谓词函数,
            就是进行一个操作用的临时函数,使用临时的匿名函数，既可以减少函数数量，又会让代码变得清晰易读
            例如:
                #include <algorithm>
                #include <cmath>
                 
                void abssort(float *x, unsigned N)
                {
                  std::sort(x,
                            x + N,
                            [](float a, float b) { return std::abs(a) < std::abs(b); });
                }
                
                lambda 表达式使用一对方括号作为开始的标识,类似于声明一个函数,只不过这个函数没有名字,也就是一个匿名函数。
                这个匿名函数接受两个参数，a和b；其返回值是一个 bool 类型的值,注意,返回值是自动推断的,
                不需要显式声明，不过这是有条件的！条件就是，lambda 表达式的语句只有一个 return.
                函数的作用是比较 a,b 的绝对值的大小.然后，在此例中，这个 lambda 表达式作为一个闭包被传递给 std::sort() 函数
                
        2.匿名函数声明之后马上就调用
            否则的话，如果这个匿名函数既不调用，又不作为闭包传递给其它函数，那么这个匿名函数就没有什么用处
            例如
                std::cout << [](float f) { return std::abs(f); } (-3.5);
                
        3.不想让lambda 表达式自动推断类型 (显式进行类型转化)
                [](参数) -> 返回值的数据类型 { 具体实现内容 }
                
                例如:
                        stds ::cout << [](float f) -> int { return std::abs(f); } (-3.5);
                        
        4.要引用一个 lambda 表达式时，我们可以使用auto关键字，
            例如
                auto lambda = [](参数) -> 返回值的数据类型 { 具体实现内容 };
                
        5.lambda前导符: [] 
             (1)引入lambda 表达式的前导符是一对方括号,称为 lambda 引入符（lambda-introducer.lambda 引入符是有其自己的作用的,
                不仅仅是表明一个 lambda 表达式的开始那么简单.lambda 表达式可以使用与其相同范围 scope 内的变量.
                这个引入符的作用就是表明,其后的 lambda 表达式以何种方式使用（正式的术语是“捕获”）这些变量
                (这些变量能够在 lambda 表达式中被捕获，其实就是构成了一个闭包).我们看到的仅仅是一个空的方括号
                 ,其实，这个引入符是相当灵活的。例如：
                    float f0 = 1.0;
                    std::cout << [=](float f) { return f0 + std::abs(f); } (-3.5);
                    
                    [=] 意味着，lambda 表达式以传值的形式捕获同范围内的变量
                
             (2) mutable 关键字
                    例如:
                        float f0 = 1.0;
                        std::cout << [=](float f) mutable { return f0 += std::abs(f); } (-3.5);
                        std::cout << '\n' << f0 << '\n';
                        
                    如果以传值的形式捕获外部变量,那么,lambda 体不允许修改外部变量,
                    对 f0 的任何修改都会引发编译错误。我们在 lambda 表达式前声明了mutable关键字,
                    这就允许了 lambda 表达式体修改 f0 的值, 但出了匿名函数,f0还是原来的值
                    
             (3) lambda 引入符
                    []        // 不捕获任何外部变量
                    [=]       // 以值的形式捕获所有外部变量
                    [&]       // 以引用形式捕获所有外部变量
                    [x, &y,&z]   // x 以传值形式捕获，y 和 z以引用形式捕获
                    [=, &z]   // z 以引用形式捕获，其余变量以传值形式捕获
                    [&, x]    // x 以值的形式捕获，其余变量以引用形式捕获
                    
                    例如:
                        float f0 = 1.0f;
                        float f1 = 10.0f;
                        std::cout << [=, &f0](float a) { return f0 += f1 + std::abs(a); } (-3.5);
                        std::cout << '\n' << f0 << '\n';
                        
        6.对于[=]或[&]的形式,lambda 表达式可以直接使用 this 指针。但是，对于[]的形式，
           如果要使用 this 指针，必须显式传入：
                [this]() { this->someFunc(); }();
```

## boost::bind/std::bind

```shell
    1.int boost_func(float f, float round)
      { return f + round; }
      
      float round = 0.5;
      boost::function<int(float)> f = boost::bind(boost_func, _1, round);

```

## 简单创建匿名函数

```c++
      1.Lambda表达式的一个好处就是让开发人员可以在需要的时候临时创建函数，便捷.在创建闭包(或者说Lambda函数)的语法上,
        C++ 11采用的是配对的方括号[]      
```