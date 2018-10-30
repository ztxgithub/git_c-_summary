## 基本技巧

```shell
    1.c++中　构造= 与　赋值= 是不一样的
        构造= 
            Person a = getAlice();
            
        赋值=
             Person a;
             a = getAlice();
             
    2.特殊类型的成员函数
        (1) 默认构造函数
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
            
        (3) 析构函数   X::~X();                     // destructor
            
        (4) 赋值操作符  X& X::operator=(const X&);   // copy assignment operator
        
        C++11 标准
        
        (5) 转移构造函数　 X::X(X&&);                   // move constructor
        (6) 转移赋值操作符  X& X::operator=(X&&);       // move assignment operator
        
    3. 如果想要一个无效对应的类型的指针(不需要进行复杂的构造)
           Timer *invalid_ptr = reinterpret_cast<Timer*>(UINTPTR_MAX)
           
    4. 需要传入函数指针
            void print(const char* msg)
            {
                printf("msg %s %s\n", muduo::Timestamp::now().toString().c_str(), msg);
            }
            typedef boost::function<void()> Callback;
            Callback = boost::bind(print, "once1") 
            
    5. 一般在多线程中对于临界区中尽量减少操作
            {
                MutexLockGuard lock(mutex_);
                functors.swap(pendingFunctors_);  // 在临界区内只需将资源拷贝到临时变量中，减少对互斥锁的占用
            }
            
            for (size_t i = 0; i < functors.size(); ++i)
            {
                functors[i]();  // 在释放互斥锁情况下才进行相应的操作
            }
            
            
    6. iter_swap() 函数: 进行容器内 iter 之间的交换
            iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);       

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

## 基于对象和面向对象的区别

```shell
    1.只有完全具有封装,继承,多态三大特点的才能够叫做面向对象,其他的只能是基于对象
    2.基于对象一般没有继承的特点
      基于对象是采用对象封装技术,将数据和操作捆绑在一起，但是并没有使用多态、继承等面向对象技术进行软件设计
```

## 三五原则

```shell
    1.在定义一个类时的拷贝控制操作, 在c++89标准中有　拷贝构造函数,赋值运算符,析构函数　---> 三原则,
       而在c++11中,又新加了　移动构造函数　和　移动赋值运算符　---> 五原则, 统称为为　三五原则
       
    2. 需要自定义析构函数则一定需要　拷贝构造函数　和　赋值运算符
        如果需要析构函数,类中必然出现了指针类型的成员（否则不需要我们写析构函数，默认的析构函数就可以用了）
    　　所以，我们需要自己写析构函数来释放给指针所分配的内存来防止内存泄露,那么对应一定需要拷贝构造函数和赋值操作符
    　　原因是类中出现了指针类型的成员,有指针类型的成员,我们必须防止浅拷贝问题,所以,一定需要拷贝构造函数和赋值操作符,
        这两个函数是防止浅拷贝问题所必须的

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
        (1) C++11的右值引用允许我们对右值进行修改(class &&var),但之前的标准中右值是不允许被改变的,
            实践中也通常使用const T&的方式传递右值,然而这是效率低下的做法
            
            右值引用　X && 对应的函数中　Intvec& operator=(const Intvec&& other),传入的参数只能是右值
            
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
                            v2 = Intvec(10); // 正确, 调用右值引用, 赋值操作符"=" 右侧只能是右值
                            
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


# 其他技巧

## RAII 机制(资源获取即初始化)

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
                
        (3) 方法
                A.利用“函数的 局部对象 无论函数以何种方式(包括因异常)结束都会被析构”这一特性,
                  将“一定要释放的资源”放进局部对象的析构函数
                
                B.使用智能指针
                
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
            
        (2) 多线程情况下很容易使用 socket 很容易导致串话，例如第一个线程 read 某个 socket　文件描述符,　第二个线程
         　　恰巧 close 该 socket　文件描述符，而第三个线程则 open 另外一个不同的 socket(但该 fd(文件描述符)　与　上一个
         　　关闭的一样)，因为 open 的返回值永远是内核中可使用的最小值．这个时候第一个线程 read 的数据就是错误的．
         
         　　解决方法:
                使用 RAII 机制定义一个 Socket 对象来封装文件描述符，所有对文件描述符的读写操作都是通过该　Socket 对象，
                在该　Socket 对象析构时释放该文件描述符

```

# 值语义 与　引用语义

## 值语义

```shell
    1.概念
        值语义：对象的拷贝与原对象无关,c++中将基础类型(int,double等)都定义为值语义
        
    2.
        标准库里的 complex<>, pair<>, vector<>, map<>, string 等等类型也都是值语义
        
    3.值语义　与　C++ 关系
        (1) class 的 layout(内存分布) 与 C struct 一样,没有额外的开销. 
            定义一个“只包含一个 int 成员的 class ”的对象开销 和定义一个 int 一样。
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

## Copy-and-swap

```shell
    1.概要
        用 copy-and-swap 主要用于 实现拷贝赋值运算符, 这极大的缩减了代码的冗余(实现拷贝赋值运算符要考虑到自赋值的情况
        以及异常安全),用了 copy-and-swap 后可以将这些情况交给编译器来处理.
        
    2.
        (1) 失败的实现:
        
            dumb_array& operator=(const dumb_array& other)  
            {  
                dumb_array temp(other);  // 这边通过按引用传递参数,并且在函数体内进行拷贝构造临时变量
                swap(*this, temp);  
              
                return *this;  
            }  
            
        (2) 成功的实现:
         
            dumb_array& operator=(dumb_array other) // 
            {  
                swap(*this, other); //  这时通过按值传递参数,在调用赋值符"=" 时,申请临时变量进行赋值
              
                return *this;  
            }   
           
            上面一旦进入函数体,所有新数据都已经被分配、拷贝，可以使用了。这就提供了强烈的异常安全保证：
            如果参数拷贝失败,我们不会进入到函数体内，那么 this 指针所指向的内容也不会被改变。（
            在前面我们为了实施强烈保证所做的事情，现在编译器为我们做了）
            
            注意到我们是直接对参数 other 传值,所以 other 会像其他任何对象一样被初始化,
            那么确切的说,other 是怎样被初始化的呢？
            对于C++ 98,答案是复制构造函数,但是对于C++ 11,编译器会依据参数是左值还是右值在
            复制构造函数(左值)和转移构造函数(右值)间进行选择。
            
            如果是 a=b,这样就会调用复制构造函数来初始化 other（因为b是左值）， 
            赋值操作符会与新创建的对象交换数据，深度拷贝。
            这就是copy and swap 惯用法的定义：构造一个副本，与副本交换数据，并让副本在作用域内自动销毁。
            
            如果是a = x + y,这样就会调用转移构造函数来初始化 other（因为x+y是右值）,所以这里没有深度拷贝,
            只有高效的数据转移。相对于参数，other 依然是一个独立的对象,但是他的构造函数是无用的（trivial）,
            因此堆中的数据没有必要复制，而仅仅是转移。没有必要复制他，因为x+y是右值，再次,
            从右值指向的对象中转移是没有问题的.
            
            总结一下：
               复制构造函数执行的是深度拷贝，因为源对象本身必须不能被改变。
               而转移构造函数却可以复制指针，把源对象的指针置空，这种形式下，这是安全的，因为用户不可能再使用这个对象了。
            
        

```

## 移动语义

```shell
    1.移动语义通常与　右值引用　相互联系
        class string
         {
             char* data;
             
             /* 在右值引用中,没有进行深度拷贝堆内存中的数据,而是仅仅复制了源指针, 并把源对象的指针置空(要考虑到自赋值问题)
              * 事实上，我们“偷取”了属于源对象的内存数据。再次，问题的关键变成：无论在任何情况下，
              * 都不能让客户觉察到源对象被改变了。在这里，我们并没有真正的复制，
              * 所以我们把这个构造函数叫做“转移构造函数”（move constructor），
              * 他的工作就是把资源从一个对象转移到另一个对象，而不是复制他们。
              */
             string(string&& that)   // string&& is an rvalue reference to a string
              {
                      data = that.data;
                      that.data = 0;
              }
         }
         
         例如:
            string b(x + y);   //临时对象调用了右值引用
            string c(some_function_returning_a_string());   // 临时对象调用了右值引用
            
    2.移动语义的使用场景
        (1) 将昂贵的拷贝运算变为转移
                如果一个对象没有保持至少一个额外的资源(该对象中没有申请内存等),
                通过move语义实现的转移构造函数就不会带来任何好处,在这种情况下,复制或转移一个对象代价是相同的
                
                例如：下面的类使用　转移构造函数是没有意义的
                        class cannot_benefit_from_move_semantics
                        {
                            int a;        // moving an int means copying an int
                            float b;      // moving a float means copying a float
                            double c;     // moving a double means copying a double
                            char d[64];   // moving a char array means copying a char array
                        
                            // ...
                        };
                        
        (2) 用于实现“安全转移类型”
                这个类型不能复制,只能转移.例如 锁,文件句柄和唯一拥有性（unique ownership semantics）的智能指针.
                
                这里我们要讨论废弃C++ 98中的智能指针模板std::auto_ptr，在C++ 11标准中被std::unique_ptr代替。
                中级C++程序员可能都会对std::auto_ptr有所了解，因为他所表现出的“转移语义”。
                这似乎是讨论C++ 11 中move语义的一个不错的起点
                
    3.转移构造函数
        使用右值引用X&&作为参数 的最有用的函数之一就是 转移构造函数X::X(X&& source),
        它的主要作用是把源对象的本地资源转移给当前对象.
        
        例如, C++ 11中, std::auto_ptr<T>已经被 std::unique_ptr<T>所取代, std::unique_ptr<T>利用的右值引用,
             传入的参数只能是右值
             
             移动语义的　移动构造函数
             unique_ptr(unique_ptr&& source)   // note the rvalue reference
                  {
                      ptr = source.ptr;
                      source.ptr = nullptr;
                  }
                  
                  
             unique_ptr<Shape> a(new Triangle);
             unique_ptr<Shape> b(a);                 // error,编译错误,转移构造函数只能接受 右值
             unique_ptr<Shape> c(make_triangle());   // okay
             
    4.转移赋值操作符
        它的作用是释放旧资源,并从参数中获取新资源
         例子:
                 unique_ptr& operator=(unique_ptr source)   // note the missing reference
                     {
                         std::swap(ptr, source.ptr);
                         return *this;
                     }
                     
                  这里赋值操作符(=) ,用参数类型用　unique_ptr source　是根据编译器来决定的,在c++11中,如果参数为左值
                  则编译器调用拷贝构造函数,如果参数是右值,则编译器调用转移构造函数.
                  
                  
                  
    以下是不常用的知识点:
    
        1.转移左值
            有时候我们想让编译器把左值当作右值对待,以便能使用转移构造函数,即便这有点不安全。
            出于这个目的，C++ 11在标准库的头文件<utility>中提供了一个模板函数std::move
                例子:
                     unique_ptr<Shape> a(new Triangle);
                     unique_ptr<Shape> b(a);              // still an error, 编译错误,转移构造函数只能接受 右值
                     unique_ptr<Shape> c(std::move(a));   // okay ,正确可以通过　move(a)来将左值(a)当成　右值传入
                     
                    第三行之后.a不在拥有Triangle对象。不过这没有关系,因为通过明确的写出std::move(a),
                    转移构造函数可以对a做任何想要做的事情来初始化c；以后不再需要需要a
                    
        2.永远不要使用std::move把自动释放类型的对象转移出函数内部
            例如：
                    (1) 下面是错误的代码
                             unique_ptr<Shape>&& flawed_attempt()   // DO NOT DO THIS!
                            {
                                 unique_ptr<Shape> very_bad_idea(new Square);
                                 return std::move(very_bad_idea);   // WRONG!　编译错误
                            }
                            
                            move虽然能够将　左值　当成　右值使用,但是终将还是左值, very_bad_idea还是函数内部的局部变量
                            再调完该函数后,very_bad_idea将释放其内存,还没来得及进行移动语义(转移构造函数),资源就被释放了
                            
                    (2) 正确的做法:
                             unique_ptr<Shape> make_square()
                             {
                                 unique_ptr<Shape> result(new Square);
                                 return result;   // note the missing std::move
                             }
                            
        3.转移进成员变量
                (1) 错误的代码
                          class Foo
                          {
                              unique_ptr<Shape> member;
                          
                          public:
                          
                              Foo(unique_ptr<Shape>&& parameter)
                              : member(parameter)   // error,不能通过右值引用作为实参来传递　转移构造函数　
                              {}
                         };
             　　　　　　　
                         编译器肯定会“抱怨”parameter 本身是左值。如果你查看它的类型,它是右值引用,
                         但是右值引用仅仅是指向右值的引用,并不意味着右值引用本身是右值.事实上,
                         parameter 仅仅是一个普通变量名而已,在构造函数内部，你可以想怎么用就怎么用，它永远指向同一个对象.
                         对它进行隐式的转移是危险的，因此C++从语言层面上禁止这样使用。
                          
                         一个命名的右值引用本身是一个左值，跟其他普通左值一样
                         
                 (2) 解决方案
                          class Foo
                          {
                              unique_ptr<Shape> member;
                          
                          public:
                          
                              Foo(unique_ptr<Shape> parameter) //直接值传递算了
                              : member(std::move(parameter))   
                              {}
                         };

```

## RTTI 机制

```shell
    1.简介
        (1) RTTI(Run-time Type Identification),代表运行时类型识别,C++引入这个机制是为了让程序在运行时能根据
            基类的指针或引用来获得该指针或引用所指的对象的实际类型.
            
        (2) C++通过以下的两个操作提供RTTI：
                A. typeid运算符,该运算符返回其表达式或类型名的实际类型(比如使用typeid(a).name()就能知道变量a是什么类型的)
                B. dynamic_cast运算符,该运算符将 基类的指针或引用 安全地转换为派生类类型的指针或引用
                
    2.typeid 运算符
        typeid　运算符后接一个类型名(或则变量名)或一个表达式, 该运算符返回一个类型为std::type_info的
        对象的const引用,class type_info　的定义大概是:
            
                class type_info
                {
                    public:
                        virtual ~type_info();
                        bool operator==(const type_info&)const;
                        bool operator!=(const type_info&)const;
                        bool before(const type_info&)const;
                        const char* name()const;
                    private:
                        type_info(const type_info&);
                        type_info& operator=(const type_info&);
                       
                        // data members 
                }
                以上的　构造函数和赋值运算符　定义为私有的, 可以看出禁止用户对type_info对象的定义和复制操作，
                用户只能通过指向type_info的对象的指针或引用来使用该类(而获取 type_info的方法就是通过 typeid运算符)
                
        (1) typeid识别 静态类型
                静态类型在程序的运行过程中并不会改变,所以并不需要在程序运行时计算类型,
                在编译时就能根据操作数的静态类型,推导出其类型信息,如下的代码片断,
                typeid中的操作数均为静态类型：
                    class X  {  ...... // 具有virtual函数 }; 
                    class XX : public X  { ...... // 具有virtual函数}; 
                    class Y  { ...... // 没有virtual函数}; 
                     
                    int main()
                    {
                        int n = 0;
                        XX xx;
                        Y y;
                        Y *py = &y;
                     
                        // int和XX都是类型名
                        cout << typeid(int).name() << endl;
                        cout << typeid(XX).name() << endl;
                        // n为基本变量
                        cout << typeid(n).name() << endl;
                        
                        // xx所属的类虽然存在virtual,但是xx为一个具体的对象
                        cout << typeid(xx).name() << endl;
                        // py为一个指针，属于基本类型
                        cout << typeid(py).name() << endl;
                        // py指向的Y的对象，但是类Y不存在virtual函数
                        cout << typeid(*py).name() << endl;
                        return 0;
                    }
                    
        (2) typeid识别 多态类型
                多态的类型是可以在运行过程中被改变的,例如,一个基类的指针,在程序运行的过程中,
                它可以指向一个基类对象,也可以指向该基类的派生类的对象,
                而 typeid运算符 需要在运行过程中识别出该基类指针所指向的对象的实际类型
                
                情况一:
                    class A
                    {
                    public:
                        /*******没有定义虚函数**********/
                         void Print() { cout<<"This is class A."<<endl; }
                    };
                    
                    class B : public A
                    {
                    public:
                         void Print() { cout<<"This is class B."<<endl; }
                    };
                    
                     A *pA = new B();
                     cout<<typeid(pA).name()<<endl; // 这边输出的是　class A *
                     cout<<typeid(*pA).name()<<endl; // 这边输出的是　class A
                     
                     RTTI机制　导致当基类不存在虚函数时, typeid(*pA) 识别的静态类型
                     
                情况二:
                    class A
                    {
                    public:
                        /********定义为虚函数**********/
                         virtual void Print() { cout<<"This is class A."<<endl; }
                    };
                    
                    class B : public A
                    {
                    public:
                         void Print() { cout<<"This is class B."<<endl; }
                    };
                    
                    当类中存在虚函数时, typeid 识别是　动态类型
                    
                情况三: == 运算符用法
                        class A
                        {
                        public:
                             virtual void Print() { cout<<"This is class A."<<endl; }
                        };
                        
                        class B : public A
                        {
                        public:
                             void Print() { cout<<"This is class B."<<endl; }
                        };
                        
                        class C : public A
                        {
                        public:
                             void Print() { cout<<"This is class C."<<endl; }
                        };
                        
                        void Handle(A *a)
                        {
                             if (typeid(*a) == typeid(A))
                             {
                                  cout<<"I am a A truly."<<endl;
                             }
                             else if (typeid(*a) == typeid(B))
                             {
                                  cout<<"I am a B truly."<<endl;
                             }
                             else if (typeid(*a) == typeid(C))
                             {
                                  cout<<"I am a C truly."<<endl;
                             }
                             else
                             {
                                  cout<<"I am alone."<<endl;
                             }
                        }
                        
                        int main()
                        {
                             A *pA = new B();
                             Handle(pA);  //输出　I am a B truly.
                             delete pA;
                             pA = new C();
                             Handle(pA);　//输出 I am a C truly.
                             delete pA;
                             return 0;
                        }
                        
                        结果:
                            I am a B truly.
                            I am a C truly.
                
        注意:
            在C++中即使一个类不具有多态的性质(即使该类是派生类,但基类中没有定义虚函数virtual)
            ,仍然允许把一个派生类的指针赋值给一个基类的指针,所以这个错误比较隐晦. 多态的含义是一个基类的指针
            指向了派生类,　并且该指针调用了虚函数virtual,　在运行时确定调用哪个派生类的虚函数
            
    3.dynamic_cast 运算符
        (1) 把一个基类类型的指针或引用转换 至 继承架构的末端(某一个派生类类型的指针或引用) 被称为向下转型(downcast).
            dynamic_cast运算符的作用是安全而有效地进行向下转型.
            
            把一个派生类的指针或引用 转换成 其基类的指针或引用总是安全的,因为通过分析对象的内存布局可以知道,
            派生类的对象中必然存在基类的子对象,所以通过 基类的指针或引用 对派生类对象进行的 所有基类的操作 都是合法和安全的.
            而向下转型有潜在的危险性,因为基类的指针可以指向 基类对象或其任何派生类的对象,
            而该对象并不一定是向下转型的类型的对象. 所以向下转型遏制了类型系统的作用,
            转换后对指针或引用的使用可能会引发错误的解释或腐蚀程序内存等错误(错误得引用了派生类特有的方法)
            
            要想 dynamic_cast 转化成功, 则必须要 类中要有虚函数（转换信息都在虚函数表里面）,
            由于运行时类型检查需要运行时类型信息，而这个信息存储在类的虚函数表,只有定义了虚函数的类才有虚函数表,
            没有定义虚函数的类是没有虚函数表的.
                        
         (2) 
                class X
                {
                    public:
                        X()
                        {
                            mX = 101;
                        }
                        virtual ~X()
                        {
                        }
                    private:
                        int mX;
                };
                
                class XX : public X
                {
                    public:
                        XX():
                                X()
                        {
                            mXX = 1001;
                        }
                        virtual ~XX()
                        {
                        }
                    private:
                        int mXX;
                };
                
                class YX : public X
                {
                    public:
                        YX()
                        {
                            mYX = 1002;
                        }
                        virtual ~YX()
                        {
                        }
                    private:
                        int mYX;
                };
                
                int main()
                {
                    X x;
                    XX xx;
                    YX yx;
                
                    X *px = &xx;
                    cout << px << endl;  // 输出结果: 0x7fff58268170
                
                    /*
                     * 向下转型成功, 因为px指向的对象确实为XX的对象
                     */
                    XX *pxx = dynamic_cast<XX*>(px); // 转换1
                    cout << pxx << endl; // 输出结果: 0x7fff58268170
                
                    /*
                     * 向下转型失败, pyx被赋值为NULL,
                     * 因为px指向的对象并不是一个YX对象
                     */
                    YX *pyx = dynamic_cast<YX*>(px); // 转换2
                    cout << pyx << endl;  // 输出结果:0
                
                    /*
                     * C风格的类型转换没有安全型, 继续通过指针使用该对象必然会导致错误
                     */
                    pyx = (YX*)px; // 转换3
                    cout << pyx << endl;  
                
                    /*
                     * C++静态类型转换没有安全型, 继续通过指针使用该对象必然会导致错误
                     */                    
                    pyx = static_cast<YX*>(px); // 转换4
                    cout << pyx << endl;
                    
                    /*
                     * 将本身是基类的地址 转换为 派生类指针是不成功的
                     */                         
                    XX *pxx_second = dynamic_cast<XX*>(&x); // 转换5
                    cout << pxx_second << endl;

                
                    return 0;
                
                }
                
                只有dynamic_case才能实现安全的向下转型
                实现原理:
                    1.计算指针或引用变量所指的对象的虚函数表的type_info信息,如下：
                        *(type_info*)px->vptr[-1]
                    2.静态推导向下转型的目标类型的type_info信息,即获取类XX的type_info信息
                    3.比较1和2中获取到的type_info信息,若2中的类型信息与1中的类型信息相等或是其基类类型,
                      则返回相应的对象或子对象的地址,否则返回NULL.

        
        
```

## 类型转换机制

```shell
    1.基本概念
        (1) 上行转换：子类指针或引用 转换成 基类 ——安全
        (2) 与 c-like 转换的区别
                C-Like转换没有static_cast, dynamic_cast分别提供的编译时类型检测和运行时类型检测
    
    1.static_cast
        (1) 静态类型转换,用于基本的数据类型转换(char，int等等),及指针之间的转换
        (2) 
            A 代表基类, B 继承于 A, C 继承于 A
            
            objB = static_cast<B>(objA);       //错误 不能用static_cast进行对象向下转换
            objB=objA;                         //错误 不能将基类对象转化为派生类
            
            pObjA = static_cast<A*>(pObjB);    //正确 可以将基类指针指向子类
            pObjB = static_cast<B*>(pObjA);    //正确(但不推荐) 强制转换 OK基类到子类,只不过不安全
            //pObjB->f();                        //right 打印A 因为pObjA所指向的空间中虚函数表f没有被覆盖
        
            pObjC = static_cast<C*>(pObjB);    //错误 继承于同一基类的派生指针之间不能相互转换
            
            int n = 6;
            double *d = static_cast<double*>(&n)   //错误: 无关类型指针转换，编译错误
            
            void *p=pObjC;                       //正确 C类型被强制转换为void*类型
            pObjD = static_cast<D*>(p);          //正确 void*转换为D类型
            pObjD->f();                          //打印C 因为pObjC所指向的空间中虚函数表中的f就是C::f
            
        (3) 无条件转换,静态类型转换.用于：
                A.基类和子类之间转换
                    其中子类指针转换成父类指针是安全的;
                    但父类指针转换成子类指针是不安全的。(基类和子类之间的动态类型转换建议用dynamic_cast)
            
                B.基本数据类型转换
                    enum, struct, int, char, float等。
                    static_cast不能进行无关类型(如非基类和子类)指针之间的转换。
            
                C. 可以把空指针转换成目标类型的指针
            
                D. static_cast不能去掉类型的const、volitale属性(用const_cast)。
                
        (4) 注意:
                static_cast字面意思是静态转换,编译期间就能判断是否可以转换成功,但是无法识别兄弟类指针之间的转换,
                而dynamic_cast是运行时转换,可以编译通过,但是可以与NULL指针的比较来判断是否转换成功！
                
    2.const_cast
        (1) 去掉类型的const或volatile属性, const_cast<类型>中类型必须是指针、引用或指向对象成员的指针
        (2) 使用背景
                在C++当中一直提倡将常量声明为const, 这样一旦常量变得多了起来,
                在与其他软件组件或者第三方库进行衔接的时候就难免会碰到需要cast const属性的问题
                
        (3) 疑问
        
            const A ra;
            //ra.i = 10;                     //直接修改const类型，编译错误
            A &rb = const_cast<A&>(ra);
            rb.i = 10;
            printf("ra.i[%d], rb.i[%d]\n", ra.i, rb.i);  // 结果: ra.i[10], rb.i[10]
            printf("ra_addr[%p]  rb_addr[%p] \n",&ra,&rb);  // 结果 ra_addr[0x7ffce21906a0]  rb_addr[0x7ffce21906a0] 
            
            
            const int a=1;
            int &j=const_cast<int&>(a);
            j=3;
            printf("a_addr[%p]  j_addr[%p]\n", &a, &j);  // 结果: a_addr[0x7ffec12030dc]  j_addr[0x7ffec12030dc]
            printf("a[%d] j[%d]\n", a, j);  //结果: a[1] j[3]
            
            
    3.reinterpret_cast
        (1) 用法:
                 reinpreter_cast<type-id> (expression)
        (2) 
           A. type-id必须是一个指针、引用、算术类型、函数指针或者成员指针。
           B. 它可以把一个指针转换成一个整数,也可以把一个整数转换成一个指针,但不能将非32bit的实例转成指针
           C. 最普通的用途就是在函数指针类型之间进行转换
           D. 很难保证移植性
           
        (3) 
            int doSomething(){return 0;};
            
            //FuncPtr是一个指向函数的指针，该函数没有参数，返回值类型为 void
            typedef void(*FuncPtr)();
            
            //10个FuncPtrs指针的数组 让我们假设你希望把一个指向下面函数的指针存入funcPtrArray数组：
            FuncPtr funcPtrArray[10];
            
            //编译错误！类型不匹配(doSomething 是一个返回值为int, 但是形参为NULL)，
            // reinterpret_cast可以让编译器以你的方法去看待它们：funcPtrArray
            funcPtrArray[0] = &doSomething;
            
            //不同函数指针类型之间进行转换
            funcPtrArray[0] = reinterpret_cast<FuncPtr>(&doSomething);
            
    4.总结
        (1) 去const属性用const_cast；
        (2) 基本类型转换用static_cast；
        (3) 多态类之间的类型转换用daynamic_cast；
        (4) 不同类型的指针类型转换用reinterpret_cast
         
```

## 内存问题

```shell
    1. 缓冲区溢出
        解决方法用 std:vector<char>/ std:string 来管理缓冲区，自动计算缓冲区的长度，修改缓冲区最好通过成员函数
        而不是裸指针
        
    2. 空悬指针/野指针
            解决方法用 shared_ptr/weak_ptr
            
    3.重复释放
        用 scoped_ptr,只在对象的析构函数释放一次
    4.内存泄露
        用 scoped_ptr
    5.不配对的 new[] / delete
            把 new[] 统统替换为 std:vector
            
    总结:
        现代 c++ 一般不会出现 delete 语句，资源都是通过智能指针来管理
    
```