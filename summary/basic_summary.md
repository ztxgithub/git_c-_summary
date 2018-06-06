## this 指针

```c++
    通常在class定义时要用到类型变量自身时，因为这时候还不知道变量名（为了通用也不可能固定实际的变量名），
    就用this这样的指针来使用变量自身。
    
    一个对象的this指针并不是对象本身的一部分，不会影响sizeof(对象)的结果。
    this作用域是在类内部，当在类的非静态成员函数中访问类的非静态成员的时候，编译器会自动将对象本身的地址作为一个隐含参数传递给函数。
    也就是说，即使你没有写上this指针，编译器在编译的时候也是加上this的，它作为非静态成员函数的隐含形参，
    对各成员的访问均通过this进行。
    
    this 的使用
        (1) 在类的非静态成员函数中返回类对象本身的时候，直接使用 return *this
        (2) 当参数与成员变量名相同时，如this->n = n （不能写成n = n)
```

# c++11 新特性

## 自动化推导 decltype

```shell
    1.可以评估括号内表达式的类型,其规则如下：
        (1) 如果表达式e是一个变量，那么就是这个变量的类型。
        (2) 如果表达式e是一个函数，那么就是这个函数返回值的类型。
        (3) 如果不符合1和2，如果e是左值，类型为T，那么decltype(e)是T&；如果是右值，则是T
        
    2.
      const vector<int> vec;
      typedef decltype(vec.begin()) CIT;
      CIT iter;
      
    3.decltype与auto关键字一样,用于进行编译时类型推导,decltype(expression),decltype 仅仅“查询”表达式的类型，
      并不会对表达式进行“求值”。
    4.推导出表达式类型
        int i = 4;
        decltype(i) a; //推导结果为int。a的类型为int
        
        const int&& foo();
        int i;
        struct A { double x; };
        const A* a = new A();
         
        decltype(foo())  x1;  // const int&&      (1)
        decltype(i)      x2;  // int              (2)
        decltype(a->x)   x3;  // double           (3)
        decltype((a->x)) x4;  // double&    当出现 (a->x),则代表是左值
        
    5.重用匿名类型
        我们有时候会遇上一些匿名类型,如:
            struct 
            {
                int d ;
                doubel b;
            }anon_s;
            
        通过decltype,我们可以重新使用这个匿名的结构体：
            decltype(anon_s) as ;//定义了一个上面匿名的结构体
```

## 统一的初始化语法

```shell
    1.
        C c {0,0}; //C++11 only. 相当于: C c(0,0);
         
        int* a = new int[3] { 1, 2, 0 }; /C++11 only
         
        class X {
            int a[4];
            public:
                X() : a{1,2,3,4} {} //C++11, member array initializer
        };
        
    2.容器的初始化
        // C++11 container initializer
        vector<string> vs={ "first", "second", "third"};
        map singers =
        { {"Lady Gaga", "+1 (212) 555-7890"},
        {"Beyonce Knowles", "+1 (212) 555-0987"}};
        
    3.成员初始化
        class C
        {
           int a=7; //C++11 only
         public:
           C();
        };
```

## delete 和 default 函数

```shell
    1.default告诉编译器产生一个默认的,只要你定义了一个构造函数,编译器就不会给你生成一个默认的了。所以,
      为了要让默认的和自定义的共存，才引入这个参数
            struct SomeType
            {
             SomeType() = default; // 使用编译器生成的默认构造函数
             SomeType(OtherType value);
            };
            
    2.阻止函数的其形参的类型调用：(若尝试以 double 的形参调用 f(),将会引发编译期错误,
                              编译器不会自动将 double 形参转型为 int 再调用f(),
                              如果传入的参数是double，则会出现编译错误）

      void f(int i);
      void f(double) = delete;

```

## nullptr

```shell
    1. 一定要以　nullptr 初始化指针
        void f(int); //#1
        void f(char *);//#2
        //C++03
        f(0); //二义性
        //C++11
        f(nullptr) //无二义性，调用f(char*)
        
```

## 左值和右值

```shell
    1.左值（lvalue,locator value）是一个表达式,它表示一个可被标识的（变量或对象的）内存位置,
      并且允许使用&操作符来获取这块内存的地址.
      如果一个表达式不是左值,那它就被定义为右值(无法确定该object的地址)
      (1) 一个表达式是右值,而不是左值(因为它没有可定位识别的内存地址)
      
    2.左值引用
        (1) C++中可以使用&符定义引用,如果一个左值同时是引用，就称为“左值引用”,如
                std::string s;
                std::string& sref = s;  //sref为左值引用
                
        (2) 非const左值引用不能使用右值对其赋值
                引用是可以后续被赋值的,右值连可被获取的内存地址都没有,也就谈不上对其进行赋值。
                std::string& r = std::string(); //错误！std::string（）产生一个临时对象，为右值
                
        (3) const左值引用可以用右值对其赋值,因为常量不能被修改
                const std::string& r = std::string(); //可以,正确
                
    3.带CV限定符（CV-qualified）的右值
        CV限定符: 变量声明时类型前带有const或volatile
        在C中,右值永远没有CV限定符,而C++中的类类型的右值可以有CV限定符,例如:
            #include <iostream>
            
            class A {
            public:
                void foo() const { std::cout << "A::foo() const\n"; }
                void foo() { std::cout << "A::foo()\n"; }
            };
            
            A bar() { return A(); }           //返回临时对象，为右值
            const A cbar() { return A(); }    //返回带const的右值（带CV限定符）
            
            
            int main()
            {
                bar().foo();  // 非const对象调用A::foo()的非const版本
                cbar().foo(); // const对象调用A::foo()的const版本
            }
            
    4.右值引用
            class Intvec
            {
            public:
                explicit Intvec(size_t num = 0)
                    : m_size(num), m_data(new int[m_size])
                {
                    log("constructor");
                }
            
                ~Intvec()
                {
                    log("destructor");
                    if (m_data) {
                        delete[] m_data;
                        m_data = nullptr;
                    }
                }
            
                Intvec(const Intvec& other)
                    : m_size(other.m_size), m_data(new int[m_size])
                {
                    log("copy constructor");
                    for (size_t i = 0; i < m_size; ++i)
                        m_data[i] = other.m_data[i];
                }
            
                Intvec& operator=(const Intvec& other)
                {
                    log("copy assignment operator");
                    Intvec tmp(other);
                    std::swap(m_size, tmp.m_size);
                    std::swap(m_data, tmp.m_data);
                    return *this;
                }
            private:
                void log(const char* msg)
                {
                    cout << "[" << this << "] " << msg << "\n";
                }
            
                size_t m_size;
                int* m_data;
            };
            
            Intvec v1(20);
            Intvec v2;
            // 有普通的重载赋值函数(Intvec& operator=(const Intvec& other)和
            // 右值引用重载赋值函数,优先调用普通的重载赋值函数
            v2 = v1    
            
       
            cout << "assigning rvalue...\n";
            v2 = Intvec(33);
            cout << "ended assigning rvalue...\n";
            
            结果:
                assigning rvalue...
                [0x28ff08] constructor  Intvec(33) 临时对象调用了构造函数
                [0x28fef8] copy assignment operator　Intvec v2 调用了重载赋值函数
                [0x28fec8] copy constructor　　　　　　在重载赋值函数中定义了tmp的临时变量
                [0x28fec8] destructor                 重载赋值函数调用完,tmp的临时变量出了作用域,自动调用析构函数
                [0x28ff08] destructor                  v2 = Intvec(33);　Intvec(33)临时变量没有用处,自动调用析构函数
                ended assigning rvalue...
                
            这就出现了2次构造和析构函数,极大的影响效率
            这时就可以通过右值引用来优化
                Intvec& operator=(Intvec&& other)
                {
                    log("move assignment operator");
                    std::swap(m_size, other.m_size);
                    std::swap(m_data, other.m_data);
                    return *this;
                }
                
                 cout << "assigning rvalue...\n";
                 v2 = Intvec(33);
                 cout << "ended assigning rvalue...\n";
                 使用右值引用,则结果:
                    assigning rvalue...
                    [0x28ff08] constructor
                    [0x28fef8] move assignment operator
                    [0x28ff08] destructor
                    ended assigning rvalue...
```

# 其他技巧

## RAII 机制

```shell
    1.概念
        (1) Resource Acquisition Is Initialization 机制　
            A.为了解决的一个问题 在这个程序段结束时需要完成一些资源释放工作，那么正常情况下自然是没有什么问题，
              但是当一个异常抛出时，释放资源的语句就不会被执行.  
              
            B. RAII 是C++语言的一种管理资源、避免泄漏的机制.C++标准保证任何情况下,已构造的对象最终会销毁,
               即它的析构函数最终会被调用(不管是正常情况还是异常炮出) 
               RAII 机制就是利用了C++的上述特性,在需要获取使用资源RES(如内存、文件句柄、网络连接、互斥量等等)的时候,
               构造一个临时对象(T),
               在其构造T时获取资源,在T生命期控制对RES的访问使之始终保持有效,最后在T析构的时候释放资源.
               以达到安全管理资源对象,避免资源泄漏的目的。
            C. 更加深层次理解
                函数内部的一些成员是放置在栈空间上的,当函数返回时,这些栈上的局部变量就会立即释放空间,
                于是Bjarne Stroustrup就想到确保能运行资源释放代码的地方就是在这个程序段（栈）中放置的对象的析构函数了,
                因为stack winding会保证它们的析构函数都会被执行。RAII就利用了栈里面的变量的这一特点
        
        (2) 将初始化和资源释放都移动到一个包装类中的好处：
                A. 保证了资源的正常释放
                B. 省去了在异常处理中冗长而重复甚至有些还不一定执行到的清理逻辑，进而确保了代码的异常安全。
                C. 简化代码体积。
                
    2.应用场景
        (1) 文件操作
            class SafeFile {
            public:
                SafeFile(const char *filename) : fileHandler(fopen(filename, "w+")) {
                    if (fileHandler == NULL) {
                        throw runtime_error("fopen");
                    }
                }
                ~SafeFile() { fclose(fileHandler); }
            
                void write(const char* str) {
                    if (fputs(str,fileHandler) == EOF) { throw runtime_error("fputs"); }
                }
            
                void write(const char* buffer, size_t num) {
                    if (num != 0 && fwrite(buffer,num,1,fileHandler) == 0) {
                        throw runtime_error("fwrite");
                    }
                }
            private:
                FILE *fileHandler;
                SafeFile(const SafeFile&);
                SafeFile & operator=(const SafeFile&);
            };
            
            int main(int argc, char *argv[]) {
                SafeFile testFile("foo.test");
                testFile.write("Hello RAII");
            }
            
            这里当实例化　SafeFile对象时其打开　foo.test　文件,获得了文件描述符,这时候无需显式释放文件描述符,只要当该实例
            出了作用域,则会调用析构函数进行释放.

```