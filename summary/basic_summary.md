## 基本技巧

```shell
    1.c++中　构造= 与　赋值= 是不一样的
        构造= 
            Person a = getAlice();
            
        赋值=
             Person a;
             a = getAlice();
             
    2.构造函数类型
        (1) 构造函数
            Person(const char* p)
            {
                size_t n = strlen(p) + 1;
                name = new char[n];
                memcpy(name, p, n);
            }
            
        (2) 复制构造函数
             Person(const Person& p)
             {
                size_t n = strlen(p.name) + 1;
                name = new char[n];
                memcpy(name, p.name, n);
            　}

```

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
            
    3.1 右值的重复拷贝
            (1) 普通右值缺点
                    右值虽然是不被后续计算所需要的，但它仍然需要构造和析构。 这在C++中造成了不少的代价
                        class Person{
                      
                                public:
                                    Person(const char* p){
                                        size_t n = strlen(p) + 1;
                                        name = new char[n];
                                        memcpy(name, p, n);
                                    }
                                    
                                    Person(const Person& p){
                                        size_t n = strlen(p.name) + 1;
                                        name = new char[n];
                                        memcpy(name, p.name, n);
                                    }
                                    
                                    ~Person(){ delete[] name; }
                                    
                                private:
                                    char *name;
                          };
                    
                    当我们拷贝Person对象时，会有额外的不需要的内存分配过程，例如：
                    
                    Person getAlice(){
                        Person p("alice");      // 对象创建。调用构造函数，一次 new 操作
                        return p;              
                    }
                    int main(){
                        Person a = getAlice();  // 这里的"=" 是构造函数适用的"=",调用拷贝构造函数，一次 new 操作
                                                // 右值析构。一次 delete 操作
                        return 0;
                    }  
                    
            (2) 返回值优化(右值重复拷贝的优化)
                    
                  class Person{
                  public:
                      char* name;
                      Person(const char* p){
                          log("constructor");
                          size_t n = strlen(p) + 1;
                          name = new char[n];
                          memcpy(name, p, n);
                      }
                      Person(const Person& p){
                          log("copy constructor");
                          size_t n = strlen(p.name) + 1;
                          name = new char[n];
                          memcpy(name, p.name, n);
                      }
                  
                    /*
                     * 这里如果传实参为右值( getAlice() ), 则　形参必须定义为　const Person& p
                     * 
                     * const左值引用可以用右值对其赋值,因为常量不能被修改
                     *                 const std::string& r = std::string(); //可以,正确
                     */
                      const Person& operator=(const Person& p){
                          printf("=const Person& p[%s]\n", p.name);
                          log("copy assignment operator");
                          size_t n = strlen(p.name) + 1;
                          name = new char[n];
                          memcpy(name, p.name, n);
                          return *this;
                      }
                  
                      ~Person(){
                          log("destructor");
                          delete[] name;
                      }
                  
                  private:
                      void log(const char* msg)
                      {
                          cout << "[" << this << "] " << msg << "\n";
                      }
                  };
                  
                  Person getAlice(){
                      Person p("alice"); return p;
                  }
                  
                  int main(){
                      cout<<"______构造函数start________________"<<endl;
                      Person a = getAlice();   //这里 "=" 代表　构造函数,实例化Person类为 a 
                      cout<<"______构造函数end________________"<<endl;
                  
                      cout<<"______赋值函数= start________________"<<endl;
                      /*
                       调用　a = getAlice(),包含很多步骤,
                            1.首先是getAlice()函数里面有构造函数Person p("alice");
                            2.紧接者　是　调用类的赋值重载函数, const Person& operator=(const Person& p)
                               其中是实例a 的　赋值重载函数,
                      */
                      a = getAlice();
                      cout<<"______赋值函数= end________________"<<endl;
                  }
                  
                  结果:
                        ______构造函数start________________
                        [0x1111111111] constructor
                        ______构造函数end________________
                        ______赋值函数= start________________
                        [0x2222222222] constructor
                        =const Person& p[alice]
                        [0x1111111111] copy assignment operator
                        [0x2222222222] destructor
                        ______赋值函数= end________________
                        [0x1111111111] destructor
                    
            
    4.右值引用
        (1) C++11的右值引用允许我们对右值进行修改,但之前的标准中右值是不允许被改变的,
            实践中也通常使用const T&的方式传递右值,然而这是效率低下的做法
            
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
            
                /*
                    传统的重载赋值函数
                */
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
            
            // 当赋值符 "=" 右边为　左值时(v1)
            // 有普通的重载赋值函数(Intvec& operator=(const Intvec& other)和
            // 右值引用重载赋值函数(Intvec& operator=(Intvec&& other)),优先调用普通的重载赋值函数
            v2 = v1    
            
       
            cout << "assigning rvalue...\n";
           　/*
              * 当赋值符 "=" 右边为　右值时(Intvec(33))
              * 有普通的重载赋值函数(Intvec& operator=(const Intvec& other)和
              * 右值引用重载赋值函数(Intvec& operator=(Intvec&& other)),优先调用右值引用重载赋值函数
              */
            v2 = Intvec(33);    // 重载赋值函数
            cout << "ended assigning rvalue...\n";
            
            结果:
                assigning rvalue...
                [0x28ff08] constructor                  Intvec(33) 临时对象调用了构造函数
                [0x28fef8] copy assignment operator　   Intvec v2 调用了重载赋值函数
                [0x28fec8] copy constructor　　　　　　  在重载赋值函数中定义了tmp的临时变量
                [0x28fec8] destructor                  重载赋值函数调用完,tmp的临时变量出了作用域,自动调用析构函数
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
                    
                    
                    
                    
        (2) 注意:
                1.对于右值引用(针对重载的赋值函数Intvec& operator=(Intvec&& other))　只能针对
                　赋值符 "=" 右边为　右值(Intvec(33)), 右值引用的形参　是　Intvec&&　可以不加 const,
                  需要注意的是　当右值Intvec&& other中的成员函数有指针时,记得防止该右值释放资源导致,
                  赋值对象的资源也被释放
                　例如:
                       (1):
                            Intvec v2;
                            v2 = Intvec(10); // 正确
                            
                       (2) 
                             /*
                              * 错误　
                              *  赋值符 "=" 右边为左值时(v1)只能
                              *   调用普通的重载赋值函数(Intvec& operator=(const Intvec& other))
                              */
                             Intvec v1(20);
                             v2 = v1;   
                              
                2.当类定义了普通的重载赋值函数(Intvec& operator=(const Intvec& other))　和　
                　右值引用(针对重载的赋值函数Intvec& operator=(Intvec&& other)),
                  当赋值符 "=" 右边为右值(Intvec(33)),优先调用右值引用
                  
                3.当类只定义了普通的重载赋值函数(Intvec& operator=(const Intvec& other)), 
                  赋值符 "=" 右边为右值(Intvec(33)) 和　赋值符 "=" 右边为左值时(v1)　都能被调用
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
                把资源用类进行封装起来,对资源操作都封装在类的内部,在析构函数中进行释放资源.当定义的局部变量的生命结束时,
                它的析构函数就会自动的被调用,这样就不用程序员显示的去调用释放资源的操作
        
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

## 返回值优化（Return value optimization,缩写为RVO）

```shell

    1.概要
        返回值优化(RVO) 是C++的一项编译优化技术,它好处是在于： 可以省略函数返回过程中 复制构造函数的多余调用,
        解决 “C++ 中长久以来为人们所诟病的临时对象的效率问题”.
        
    2.过程
        RVO MyMethod (int i)
        {
          RVO rvo;
          rvo.mem_var = i;
          return (rvo);
        }
        
         RVO rvo_instance;
         rvo_instance = MyMethod(5);
         
        (1) 非返回值优化
                A. 在函数的栈中创建一个名为rvo的对象 (rvo 构造函数)
                B. 用变量rvo来构造需要返回的临时对象　(return 的临时对象构造函数)
                C. 函数返回结束，析构掉在函数内建立的所有对象(rvo 析构函数)
                D. 继续表达式rvo=MyMethod(5);里的操作语句结束,临时对象析构函数
                
        (2) 采用返回值优化
                编译器识别出了 return后的返回对象rvo和函数的返回对象的类型一致,就会对代码进行优化 .
                编译器转而会将二者的直接关联在一起,意思就是,对rvo的操作就相当于直接对 临时对象的操作
                作用是：
                    消除函数返回时创建的临时对象
                    
    3.编译
        (1) 禁用返回值优化(RVO)
                > g++ -o rvo_test rvo_test.cc -fno-elide-constructors
    

```

# 值语义 与　引用语义

## 值语义

```shell
    1.概念
        值语义：对象的拷贝与原对象无关,c++中将基础类型(int,double等)都定义为值语义
        
    2.
        标准库里的 complex<>, pair<>, vector<>, map<>, string 等等类型也都是值语义
        
    3.值语义　与　C++ 关系
        (1) class 的 layout(内存分布) 与 C struct 一样,没有额外的开销. 定义一个“只包含一个 int 成员的 class ”的对象开销
            和定义一个 int 一样。
        (2) class data member(成员变量) 都默认是 uninitialized,因为函数局部的 int 是 uninitialized。
        (3) class 可以在 stack(栈) 上创建,也可以在 heap(堆上) 上创建.因为 int 可以是 stack variable。
        (4) class 的数组就是一个个 class 对象挨着,没有额外的 indirection。因为 int 数组就是这样。
        (5) 编译器会为 class 默认生成 copy constructor 和 assignment operator。
            其他语言没有 copy constructor 一说，也不允许重载 assignment operator。
            C++ 的对象默认是可以拷贝的，这是一个尴尬的特性。
        (6) 当 class type 传入函数时(函数的参数以　class),默认是 make a copy （除非参数声明为 reference 引用 &）.
            因为把 int 传入函数时是 make a copy。
        (7) 当函数返回一个 class type 时,只能通过 make a copy（C++ 不得不定义 RVO 来解决性能问题）。
            因为函数返回 int 时是 make a copy。
        (8) 以 class type 为成员时，数据成员是嵌入的。
            例如 pair<complex<double>, size_t> 的 layout 就是 complex<double> 挨着 size_t.
            
        总结:
            这些设计带来了性能上的好处,原因是 memory locality (内存分布的局部性)

```

## 引用语义 (对象语义,指针语义)

```shell
    1.概念
        引用语义: 一个对象被 系统标准的复制函数复制后,与 被复制的对象 共享底层资源,只要一个改变了另外一个就会改变.
        
    2.
        就像包含有 指针成员变量 的自定义类在默认拷贝构造函数下对其对象之间进行的拷贝.
        拷贝后目标对象和源对象的指针成员变量仍指向同一块内存数据.
        如果当其中一个被析构掉后,另一个对象的指针成员就会沦为名副其实的悬垂指针！
        
    3. 对于一些特定的对象　是不可复制的(拷贝一个线程, 复制一个 tcp连接), 对于这种对象应该禁止他们的复制操作. 
       最好的方法是继承 boost::noncopyable
       
       想把 tcp 连接放入 vector 或者在函数中传递怎么办? 我是用的方法是统一使用 shared_ptr, 这样资源泄漏的问题也就解决了. 
       所以, 在实现一个资源相关的类时, 用 RAII 方法封装一下资源, 然后继承 noncopyable 应该能解决大部分问题.
        

```

## 注意事项

```shell
    c++编译器会为类提供默认的拷贝构造函数和重载函数,一般我们不需要自己重写这些,
    因为只要每个数据成员都为 值语义, 编译器就可以调用默认的成员拷贝（浅拷贝）

```