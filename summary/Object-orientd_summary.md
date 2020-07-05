# C++　面向对象开发

## c++ 介绍

```shell
    1. c++ 98(1.0), c++11(2.0)
    2. 学习 c++ 分语言和标准库, 在工程应用中更倾向与使用标准库
    3. 阅读的书籍:
           (1) Effective C++ 
           (2) THE C++ STANDARD LIBRARY(A Tutorial and Reference)
           (3) STL 源码剖析
```

## c 与 c++ 比较

```shell
    1. 对于 c 层面, 数据与函数没有关联
    2. .h 防卫式的声明
        #ifndef __COMPLEX__
        #define __COMPLEX__
        
        .....
        
        #endif
    3. 如果成员函数的定义直接在 .h 的 class 编写了, 这类函数就是 inline 函数, 运行效率较高　
```

## 类的实现
```shell
    1. 构造函数内最好使用初始化列表, 而不是赋值, 因为一个成员变量值的设定可以在两个阶段, 一个是初始化阶段, 一个
    　　是之后的赋值阶段. 最好在初始化阶段就设定值, 这样效率高一些
    2.  常量成员函数
            class complex
            {
                public:
                    /*
                        这里 const 修饰成员函数, 表示调用该成员函数不会改变成员变量 re
                    */
                    double real() const   
                    {
                        return re;
                    }
                private:
                    double re;
            }
```

