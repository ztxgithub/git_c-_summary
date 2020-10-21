# 现代　c++ 

## c++ 概要

```shell
    1. C++ 是一门多范式的通用编程语言, 多范式，是因为 C++ 支持面向过程编程，也支持面向对象编程，也支持泛型编程
    2. 栈满足　"后进先出"(LIFO, last-in-first-out)
    3. 栈展开（stack unwinding）, 就是程序发送异常时对析构函数的调用.
    4. 对于工厂方法返回的是基类的指针, 而不是基类的对象, 这会造成对象切片(object slicing), 这不是语法错误, 而是对象赋值相关的语义
    　　错误.
            
    6. 实现 class 类禁止拷贝和赋值
            template
            class smart_ptr 
            {
                    …
                smart_ptr(const smart_ptr&) = ;  // 禁止拷贝
                
                smart_ptr& operator=(const smart_ptr&) = ;  // 禁止赋值
                    …
            };
    7. 常见强异常安全性的赋值方法: 赋值的实现采用先拷贝构造, 再交换.异常只可能在第一步发生；而第一步如果发生异常的话，
    　　this 对象完全不受任何影响. 无论拷贝构造成功与否，结果只有赋值成功和赋值没有效果两种状态，而不会发生因为
       赋值破坏了当前对象这种场景(if (this != &rhs) 就会有这种问题), 这个具体的应用是 auto_ptr
       
       c++11 unique_ptr 实现.
       
        template
        class smart_ptr 
        {
            …
            smart_ptr(smart_ptr&& other)  // 这个是移动构造函数, 如果是传入的参数是 smart_ptr& other 则代表是拷贝构造函数
            {
                 ptr_ = other.release();
            }
            
            // 赋值的常用操作, 先进行传入参数就拷贝构造出变量(值拷贝), 在通过 swap 进行交换, 而不是 (if (this != &rhs)
            smart_ptr& operator=(smart_ptr rhs)
            {
                rhs.swap(*this);  
                return *this;
            }
            …
            
            T* release()
            {
                T* ptr = ptr_;
                ptr_ = nullptr;
                return ptr;
            }
            
            void swap(smart_ptr& rhs)
            {
                using std::swap;
                swap(ptr_, rhs.ptr_);
            }
            …
        };
        
      　实例:
            smart_ptr ptr1{create_shape(shape_type::circle)};
            smart_ptr ptr2{ptr1}; // 编译出错, 没有拷贝构造函数
            smart_ptr ptr3;
            ptr3 = ptr1; // 编译出错, 这就是 unique_ptr 的优点,编译时就报错, 而 auto_ptr 则不会报错
           
            ptr3 = std::move(ptr1); // OK，可以, 这里在
            smart_ptr ptr4{std::move(ptr3)}; // OK，可以
            
            
    8. 
        share_ptr 的实现:
        
        template
        class smart_ptr {
        public:
        
            // 这里用 friend 进行申明其实是想 拷贝构造函数或则移动构造函数进行直接用 object.shared_count_ 
            template
            friend class smart_ptr;
            
            
            explicit smart_ptr(T* ptr = nullptr)
                    : ptr_(ptr)
            {
                printf("explicit smart_ptr(): %p\n", this);
                if (ptr) {
                    printf("ptr not null explicit smart_ptr(): %p\n", this);
                    shared_count_ = new shared_count();
                }
            }
        
            ~smart_ptr()
            {
                printf("~smart_ptr(): %p\n", this);
                if (ptr_ &&
                    !shared_count_
                            ->reduce_count()) {
                    ptr_;
                    shared_count_;
                }
            }
        
           
            template
            smart_ptr(const smart_ptr& other) noexcept
            {
                ptr_ = other.ptr_;
                if (ptr_) {
                    other.shared_count_->add_count();
                    shared_count_ = other.shared_count_;
                }
            }
        
           
            template
            smart_ptr(smart_ptr&& other) noexcept
            {
                ptr_ = other.ptr_;
                if (ptr_) {
                    shared_count_ =
                            other.shared_count_;
                    other.ptr_ = nullptr;
                }
            }
        
           
            smart_ptr& operator=(smart_ptr rhs) noexcept
            {
                rhs.swap(*this);
                return *this;
            }
       
            void swap(smart_ptr& rhs) noexcept
            {
                using std::swap;
                swap(ptr_, rhs.ptr_);
                swap(shared_count_,
                     rhs.shared_count_);
            }

        private:
            T* ptr_;
            shared_count* shared_count_;
        };
```

## 自定义指针类
```shell
    1. 使用 RAII(通过析构函数和栈展开), 把基类指针赋值到栈变量(WrapperShaper)中
       主要是利用 RAII 机制, 让编译器自行管理指针的释放.
        class shape_wrapper {
               public:
               explicit shape_wrapper(shape* ptr = nullptr)　: ptr_(ptr) {}
               
               ~shape_wrapper()
               {
                    ptr_;
               }
               shape* get() const { return ptr_; }
               
               private:
                   shape* ptr_;
               };
               
               void foo()
               {
                    …
                    shape_wrapper ptr_wrapper(create_shape(…));
                    …
               }
               
    2. 指针类可以分为 unique_ptr 和 share_ptr
    3. unique_ptr 主要是全局只有一个指针指向一个对象, 同时需要移动语义来约束, 再编译时得用  ptr3 = std::move(ptr1)
       
```

## 右值和移动语义
```shell
    1.  
        (1) lvalue(左值)是有标识符、可以取地址的表达式, 
                a. 变量、函数或数据成员的名字
                b. 返回左值引用的表达式，如 ++x
                c. 常量字符串　"hello world"
                
        (2) prvalue(pure rvalue 纯右值) : 就是临时变量(没有标识符、不可以取地址的表达式)
                a. 返回非引用类型的表达式，如 x++
                b. 除了常量字符串的其他字面量, 例如 42, true
        (3) xvalue(expiring lvalue, 将亡值) 有名字(标识符),不能取地址的表达式
                a. std::move(ptr)
                
    2. 常左值引用(const lvalue reference) -> const T&
       非常左值引用(non-const lvalue reference)  -> T&
       右值引用　-> T&&, 一般不会使用 const 或则 volatile 来修饰右值引用
                    (std::move() 可以将左值转化为为右值引用, 也可以临时变量(prvalue) 转为右值引用, 这样临时变量的生命周期就会变长)
    3. 值类别(value category)和 “值类型”(value type) 是 2 个概念,
            (1) 值类别(value category) 是左值、右值相关的概念
            (2) 值类型(value type) : 表明一个变量是代表实际数值, 与之相对的是 "引用类型" ,表明该值是引用另外一个数值.
                                    C++ 里，所有的原生类型、枚举、结构、联合、类都代表值类型, 
                                    只有引用(&)和指针 (*)才是引用类型
    4. 一个临时对象会在包含这个临时对象的完整表达式(函数调用)完成后(在主程序下一个语句之前)、按生成顺序的逆序被销毁
    5. 有一种情况可以对临时变量的周期进行延长, 就是如果一个 prvalue(纯右值) 被绑定到一个引用上,
       它的生命周期则会延长到跟这个引用变量一样长
       例如:
            result process_shape()
            {
                return result();
            }
            
            // 这样　process_shape() 中的 result() 临时变量的生命周期变长, 根据引用变量一样长, 但前提条件是 prvalue(纯右值)
            result&& r = process_shape(); 
            
            如果是以下这个则出现未定义错误, 这条生命期延长规则只对 prvalue 有效，而对 xvalue 无效。如果由于
            某种原因，prvalue 在绑定到引用以前已经变成了 xvalue，那生命期就不会延长
            result&& r = std::move(process_shape());  // 错误的
            
    6. 移动语义
            (1) c++11 之后，使用容器类时, 通过移动语义可以减少临时对象的产生,  优化其性能.
                例如: string result = string("hello ") + name + ".";
                1. 调用构造函数 string(const *), 生成临时对象1, "hello " 复制 1 次
                
                2. 调用 operator+(string&&, const string&), 直接在临时对象1 上面执行追加
                   操作, 并把结果移动到临时对象 2；name 复制 1 次
                   
                3. 调用 operator+(string&&, const *), 直接在临时对象 2 上面执行追加操
                   作，并把结果移动到 result；"." 复制 1 次
                   
                4. 临时对象 2 析构，内容已经为空，不需要释放任何内存
                
                5. 临时对象 1 析构，内容已经为空，不需要释放任何内存
                
    7. 返回值优化（named return value optimization，或 NRVO）: 在函数返回该函数本地对象时, 直接将对象构造到调用者的栈上.
       编译器能先进行返回值优化, 先进行返回值优化, 如果不进行返回值优化的话, 也不会进行拷贝对象, 而是进行移动语义(本地对象移动出去).
       这一行为不需要程序员手工用 std::move 进行干预——使用std::move 对于移动行为没有帮助，反而会影响返回值优化.
       
    8. 引用折叠
            1. 有两个重载函数 
                    foo(const shape&) > 左值引用, 当传入的参数为左值时, 被匹配
                    foo(const shape&&) > 左值引用, 当传入的参数为纯右值(prvalue), 或则 std::move(ptr) 被匹配
                    
    9. 很多标准库里的函数，连目标的参数类型都不知道，但我们仍然需要能够保持参数的值类型：左值的仍然是左值，右值的仍然是右值   
       这时需要采用 std::forward(万能引用)
       
                void foo(const shape&)
                {
                    puts("foo(const shape&)");
                } 
                
                void foo(shape&&)
                {
                    puts("foo(shape&&)");
                }
                
                template
                void bar(T&& s)
                {
                    foo(std::forward(s));
                }
                
                circle temp;
                bar(temp);
                bar(circle());
                
                输出：
                    foo(const shape&)
                    foo(shape&&)           　　　　　　　
            
```

## 容器
```shell
    0. 
        (1) 关键字 noexcept, 修改的函数表明, 这个函数在任何情况下不会抛出异常．这个 noexcept 主要修饰 
            移动构造函数（move constructor）, 移动分配函数（move assignment）, 析构函数（destructor）
        (2) 异常安全, 当要老的容器拷贝到新的容易出现异常, 不能导致老容器数据丢失.
    1. 序列容器
        1. vector
           (1) vector 通常保证强异常安全性，如果元素类型没有提供一个保证不抛异常的移动构造函数，
               vector 通常会使用拷贝构造函数.对于拷贝代价较高的自定义元素类型，我们应当定义移动构造函数，并标其为 noexcept,
               或只在容器中放置对象的智能指针
           (2) vector 在 c++11 之后新增 emplace_back()方法, 与 push_back() 功能相似, 只不过性能上更好. emplace_back() 只需要传入
           　　 对象的初始化参数即可, 它在 vector 最后新建一个元素, 再原地构造，不需要触发拷贝构造和转移构造．
           　　　而 push_back() 功能传入的参数必须是对象, 如果是移动构造函数并且标记为 noexcept, 则使用移动语义.否则使用拷贝构造函数
           
        2. deque(double-ended queue 双端队列)
            (1) 只允许在头部和尾部都可以插入删除, 所以在具体的实现上可以采用申请几段连续的内存空间, 再用链表法进行关联.
            (2) 容器里的元素只是部分连续的（因而没法提供 data 成员函数
        
        3. list
            (1) 是一个双向链表, 优化了在容器中间的插入和删除
            (2) 有些标准库不适用与 list, list 自己提供算法, 例如 merge, remove, remove_if, reverse, sort, unique
            
        4. 类容器(容器适配器, 依赖于某个现有的容器)
            (1) queue : 默认是依赖与 dequeue, 只能在末端插入, 在头部删除. FIFO(first-in first-out)
            (2) stack: 默认是依赖与 dequeue, 只能在末端插入, 删除. FILO(first-in last-out)
            (3) stack 跟我们前面讨论内存管理时的栈有一个区别：在这里下面是低地址，向上则地址增大；
                而讨论内存管理时，高地址在下面，向上则地址减小，方向正好相反
                
    2. 关联容器
        (1) 如果在声明关联容器时没有提供比较类型的参数，缺省使用 less 来进行排序。如果键的类型提供了比较算符 < 的重载，
            则不需要做任何额外的工作。否则，就需要对键类型进行 less 的特化，或者提供一个其他的函数对象类型
    3. array 容器
            (1) 介于 C 数组格式的限制性(如果一个函数内的传入参数是数组, 则在这个函数内是无法确定这个数组的大小), 可以采用
                c++ 的 array 容器来代替 C 数组, array 数组提供了统一的 begin, end, size 等通用函数.
            (2) 
       　　
```

## 异常
```shell
    1. 异常安全的代码，可以没有任何 try 和 catch, 直接用 throw 就可以. 异常安全是指当异常发生时， 既不会发生资源泄漏，
       系统也不会处于一个不一致的状态
    2. C++ 的标准容器在大部分情况下提供了强异常保证，即，一旦异常发生，现场会恢复到调用函数之前的状态，容器的内容不会发生改变，
       也没有任何资源泄漏
```

## 迭代器
```shell
    1. 迭代器一般依托与容器类
            class containString
            {
                public:
                    class iterator
                    { // 实现 InputIterator
                        public:
                            typedef ptrdiff_t difference_type;  // 代表迭代器之间距离的类型
                            typedef string value_type;  // 迭代器指向的对象的值类型(string)
                            typedef const value_type* pointer;  // 迭代器指向的对象的指针类型(不可更改指针的内容)
                            typedef const value_type& reference;  // value_type 的常引用
                            typedef input_iterator_tag iterator_category;  // 标识这个迭代器的类型是 input iterator（输入迭代器）
                            …
                    };
            }
```

## 易用性
```shell
    c++11 特性
    1. 自动类型推断 auto
            (1) 使用 auto 的变量（或函数返回值）的类型仍然是编译时就确定，只不过编译器能自动填充
    2. decltype
            (1) 功能是获取表达式的类型.
            (2) 
                decltype(变量名) -> 可以获得变量的精确类型
    3. 初始化列表
            (1)  vector v{1, 2, 3, 4, 5};
    4. 统一初始化: 使用 {} 来代替 ()
        (1) 当一个构造函数没有标成 explicit 时，可以使用大括号不写类名来进行构造
                Obj getObj()
                {
                    return {1.0};
                }
        (2) 什么情况下使用统一初始化的语法
                a. 原因: 如果一个构造函数既有使用初始化列表的构造函数，又有不使用初始化列表的构造函数，
                        那编译器会千方百计地试图调用使用初始化列表的构造函数，导致各种意外
                b. 如果一个类没有使用初始化列表的构造函数时，初始化该类对象可全部使用统一初始化语法。
                   如果一个类有使用初始化列表的构造函数时，则只应用在初始化列表构造的情况
    5. 类数据成员的默认初始化, 允许在声明数据成员时直接给予一个初始化表达式
            class Complex {
                public:
                    Complex() {}
                    Complex(float re) : re_(re) 
                    {
                    
                    }
                    Complex(float re, float im) : re_(re) , im_(im) 
                    {
                    
                    }
                private:
                    float re_{0};
                    float im_{0};
            };
    6. 基于范围的 for 循环
            (1)  基于范围的 for 循环以统一、简洁的方式来遍历容器和数组, 冒号后面的表达式只会被执行一次
            (2) 
                    std::vector arr = { 1, 2, 3 };
                    // n 为 容器的对象而不是迭代器
                    for(auto n : arr)  //使用基于范围的for循环
                    {
                        std::cout << n << std::endl;
                    }
                    
                    // 可以修改容器中元素值
                    for(auto& n : arr)
                    {
                        std::cout << n++ << std::endl;
                    }
                    
    7. 成员函数说明符
            (1) default 成员函数
                    class my_array {
                        public:
                            // 使用编译器默认的构造函数
                            my_array() = default;
                            private:
                                T* data_{nullptr};
                                size_t size_{0};
                    };
                    
            (2) 成员函数
                    class shape_wrapper {
                        shape_wrapper(const shape_wrapper&) = ;  // 不允许外部使赋值操作符
                        shape_wrapper& operator=(const shape_wrapper&) = ;// 不允许外部使赋值操作符
                    };
    
            (3) override 说明符
                    class A {
                        public:
                            virtual void foo();
                            virtual void bar();
                            void foobar();
                    };
                    
                    class B : public A {
                        public:
                        void foo() override; // OK
                        
                    };
                    
                override 显式声明该函数为虚函数,并且覆盖写基类中的虚函数, 非虚函数不能被标识为 override
                
            (4) final 说明符
                    final 则声明了成员函数是一个虚函数(在类 A 中)，且该虚函数不可在派生类中被覆盖
                    (如果类 B 继承类 A, 则类 B 无法覆盖写这个标记为 final 的函数)
                    
                    final 也可以标记这个类或则结构体不能被派生(就是其他类不能继承这个类)
                    
                     class A {
                        public:
                            virtual void bar();
                            void foobar();
                    };
                    
                    class B : public A {
                        public:
                           void bar() override final; // OK        
                    };
                    
                    class C final : public B {
                        public:
                        void bar() override; // 错误, final 函数不可 override
                    };
    c++14              
    1. 字面量
            (1) 二进制字面量
                     unsigned mask = 0b111000000
                     cout << bitset<9>(mask) << endl;
                     输出为 111000000
            (2) 数字分隔符
                    为了增加程序的可读性, 允许在数字型字面量中任意添加  
                    例如:
                        unsigned mask = 0b111000000;
                        long r_earth_equatorial = 6378137;
                        const unsigned magic = 0x4442474E;
                
                
    c++17 特性
    1. 类模板的模板参数推导(c++17)
            (1) 不用再写 c++11 那样 auto ptr = make_pair(1, 2);
                可以直接写 pair pr{1, 2}
    2. 结构化绑定(c++17)
            (1) c++11/14 需要各自声明变量来接收返回的结果
                    例如
                            multimap::iterator lower, upper;
                            std::tie(lower, upper) = mmp.equal_range("four");
                            
                而 c++17 可以直接用 auto
                            auto [lower, upper] = mmp.equal_range("four");
    3. optional 
            (1) 背景:
                    在使用返回值对象时,想要表示没有找到需要的对象, c++ 17 的做法是空值 null 或则空对象来表示, 但是
                    空值或则空对象一般只能用在返回引用或则指针的场合, 需要在堆上分配空间, 会有额外的开销.
            (2) 所以引入了 optional 模板, 代表一个可选的对象, 它管理的对象是直接放在 optional 里的，没有额外的内存分配
            (3) 具体的用法
                   a. 空的　-> std::nullopt
                   b. 第一个参数为　std::in_place, 之后就是构造 T 的所需参数, 可以在 optional 对象上直接构造出 T 的有效值
                   c. 如果 T 类型支持拷贝构造或者移动构造，在构造 optional 时也可以传递一个 T 的左值或右值来将 T 对象拷贝或
                      移动到 optional 中
                   
                 调用者可以通过对返回值判断是否为 true 或则 false
```

## c++11 之并发编程
```shell
    1. std::thread (线程对象)使用(http://www.cplusplus.com/reference/thread/thread/thread/)
            (1) 例子:
                  a.
                    #include
                    void thread_task(void *param) {
                        printf("a[%d]\n", *((int*)param));
                        std::cout << "hello thread" << std::endl;
                    }
                    
                    int a = 10;
                    std::thread t(thread_task, &a);
                    t.join();
                    
                  b. 
                  
                    #include
                    #include
                    #include
                    #include
                    
                    void f1(int n)
                    {
                        for (int i = 0; i < 5; ++i) {
                            std::cout << "Thread " << n << " executing\n";
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        }
                    }
                     
                    void f2(int& n)
                    {
                        for (int i = 0; i < 5; ++i) {
                            std::cout << "Thread 2 executing\n";
                            ++n;
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        }
                    }
                    
                    std::thread t1; // t1 is not a thread
                    std::thread t2(f1, n + 1); // pass by value
                    std::thread t3(f2, std::ref(n)); // pass by reference
                    std::thread t4(std::move(t3)); // t4 is now running f2(). t3 is no longer a thread
                    t2.join();
                    t4.join()
                    
            (2) thread 线程对象的成员函数
                    a. thread::id get_id() const noexcept 
                    　　　　功能: 获取这个线程的 id 就等于在这个线程中调用　std::this_thread::get_id()
                    
                    b. bool joinable() const noexcept
                           功能: 获取这个线程是否具备 joinable(代表该线程是否为正在活动的线程)
                           线程不是 joinable 的三种情况
                                i. 调用的是默认构造函数,  std::thread foo;
                                ii. 本来这个线程对象 A 是 joinalbe, 但是通过 move 给其他的线程对象, 则现在线程对象 A 就不具备
                                    joinalble
                                iii. 如果调用了 join() 或则 detach() 成员函数, 就不具备 joinalble
                    c. void join();
                          功能: 调用 join() 会阻塞调用 thead.join() 所在的线程，　直到由 *this 所标示的线程执行完毕 join 才返回
                          注意: 调用此函数后，这个线程对象变得 not joinable, 并且可以安全地销毁
                    d. void detach();
                          功能: 调用 detach() 会从调用线程(调用 thead.detach())中分离出对象所代表的线程，从而使它们彼此独立执行。
                                两个线程继续运行，而不会阻塞或以任何方式进行同步(但是调用的线程不能是 main(), 因为 main() 退出后
                                所有的线程都无法正常工作)
                          注意: 调用此函数后，这个线程对象变得 not joinable, 并且可以安全地销毁   
                           
                    e. void swap (thread& x) noexcept;
                          功能: 两个线程对象的交换.
                          
                    f. hardware_concurrency(): 
                         功能: 检测硬件并发特性，返回当前平台的线程实现所支持的线程并发数目，但返回值仅仅只作为系统提示(hint)
                          
                                        
    2. this_thread
            (1) thread::id std::this_thread::get_id() noexcept;
            
                    功能: 获取线程 ID
                    std::thread::id main_thread_id = std::this_thread::get_id()
                    
            (2) template< class Rep, class Period >
                  void sleep_for( const std::chrono::duration& sleep_duration );
                  
                  功能: 线程休眠某个指定的时间片(time span, 休眠某个时间长度)，该线程才被重新唤醒，不过由于线程调度等原因，
                      实际休眠时间可能比 sleep_duration 所表示的时间片更长
                      
                  #include
                  this_thread::sleep_for(chrono::nanoseconds(1000));//阻塞当前线程1000纳秒
                  this_thread::sleep_for(chrono::microseconds(1000));//阻塞当前线程1000微妙
                  this_thread::sleep_for(chrono::milliseconds(1000));//阻塞当前线程1000毫秒
                  this_thread::sleep_for(chrono::seconds(20)+ chrono::minutes(1));//阻塞当前线程1分钟20秒
                  this_thread::sleep_for(chrono::hours(1));//阻塞当前线程1小时
                 
            (3) template< class Clock, class Duration >
                  void sleep_until( const std::chrono::time_point& sleep_time );
                  
                  功能: 线程休眠至某个指定的时刻(time point)，该线程才被重新唤醒
                  
                  chrono::system_clock::time_point until = chrono::system_clock::now();
                  until += chrono::seconds(5);
                  this_thread::sleep_until(until);//阻塞到5秒之后
               
            (4) void std::this_thread::yield() noexcept
                    功能: 阻塞当前线程, 切换到其他的线程
                    
    3. atomic 类型
            (1) atomic_flag 
                    概要: 是简单的原子 bool 类型
                    注意: std::atomic_flag 只有默认构造函数, 禁止拷贝构造函数, atomic_flag 不能被拷贝，也不能 move 赋值
                    
                    传入参数 memory_order
                        1. memory_order的作用是明确数据的读写顺序以及数据的写入对其它线程的可见性,常用的有 3 种模型,
                           第一种: sequentially-consistent ordering(memory_order_seq_cst, 默认情况下), 
                                    序列一致顺序(Sequentially consistent ordering ,默认的)　 是执行代价最大的,是 
                                    memory_order_acq_rel 的加强版, 全部读写操作顺序均一致.就是在 线程2 中拿到两个变量的
                                    顺序的值，是按照线程1 顺序执行的(前提是线程1 被先执行)
                                  
                           第二种: relaxed ordering (memory_order_relaxed)
                                    relaxed ordering 不会所有线程顺序同步，在单个线程类依然是遵循操作在前先执行的顺序，
                                    但是到其他线程里面则无法知道这种先后顺序即在1线程内A操作先于B操作,也就是
                                    即使线程 1 中变量 A 先于 变量 B 执行, 但在 线程 2　变量 B 已经被执行了, 但变量 A 不一定
                                    被执行.
                                  
                           第三种: acquire-release ordering(memory_order_consume, memory_order_acquire, memory_order_release, 
                                                            memory_order_acq_rel)
                                    在这种模式下 atomic 变量的 load 操作是 acquire(memory_order_acquire)的，
                                    而变量的 store 是 release(memory_order_release)的. C++ 规定 release 操作会同步到
                                    操作 acquire 操作 
                                    例如:
                                        
                                                std::atomic x,y;                                                 
                                                std::atomic z;          
                                                void write_x_then_y()
                                                {
                                                    // atomic 使用 std::memory_order_release 会同步到　std::memory_order_acquire　中
                                                    x.store(true,std::memory_order_release);             #1                                                 
                                                    y.store(true,std::memory_order_release);             #2
                                                 
                                                }
               
                                                void read_y_then_x()
                                                {                                                 
                                                    while(!y.load(std::memory_order_acquire));           #3    
                                                    // 因为 y 已经变为 true, 而 线程　write_x_then_y　的　x 的 store 用的是　std::memory_order_release, 所以 x 会被赋值
                                                    if(x.load(std::memory_order_acquire))                #4                                                 
                                                    ++z;                                                 
                                                }
                                
                                                int main()
                                                {                                                
                                                    x=false;
                                                    y=false;
                                                    z=0;
                                                    std::thread a(write_x_then_y);                                                 
                                                    std::thread b(read_y_then_x);                                                 
                                                    a.join();                                                 
                                                    b.join();                                                 
                                                    assert(z.load()!=0);                                #5                                                 
                                                }

                           
                           
                    成员函数:
                        a. bool test_and_set (memory_order sync = memory_order_seq_cst) noexcept;
                                功能: 设置 atomic_flag 为 true, 并且返回调用函数之前的 atomic_flag 的状态
                                
                                     
                        b. void clear (memory_order sync = memory_order_seq_cst) volatile noexcept;
                                功能: 清除 std::atomic_flag 标志使得下一次调用 std::atomic_flag::test_and_set 返回 false
                    例子:
                        std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
                        
            (2) atomic
                    a. std::atomic 对 int, , bool等数据结构进行原子性封装，在多线程环境中，对 std::atomic 对象的访问不会
                       造成竞争-冒险。利用 std::atomic 可实现数据结构的无锁设计
                    b. 成员函数
                            1. 
                                atomic (const atomic&) = ;　// 拷贝构造函数被禁止
                                atomic& operator= (const atomic&) = ;  // atomic 对象间赋值被禁止
                                T operator= (T val) noexcept;  // 赋值 = 右边可以是 T, 而不是 atomic
                                
                            2. bool is_lock_free()
                                    判断该 std::atomic 对象是否具备 lock-free 的特性, lock-free 表示多个线程访问该对象时不会导
                                    致线程阻塞
                                 
                            3. void store (T val, memory_order sync = memory_order_seq_cst)
                                    修改被封装的值
                                    
                            4. T load (memory_order sync = memory_order_seq_cst) 
                                    读取被封装的值
                                    
                            5. T exchange (T val, memory_order sync = memory_order_seq_cst)
                                    读取并修改被封装的值, 将 val 值进行设置, 并返回原来老的值, exchange 也被称为
                                     read-modify-write 操作
                                     
                            6. bool compare_exchange_weak (T& expected, T val, memory_order sync = memory_order_seq_cst)
                                    比较内部的 T 值与 expected 是否相等
                                        (1) 如果内部的 T 值与 expected 相等, 则将 val 赋值给内部的 T 值
                                        (2) 如果内部的 T 值与 expected 不相等, 则将 expected　赋值给内部的 T 值, 并且原来的
                                        　　T 值再赋值给 expected
                                        
                            对于整型或则指针   
                            7.　
                                T fetch_add (T val, memory_order sync = memory_order_seq_cst) 针对整型的
                                T fetch_add (ptrdiff_t val, memory_order sync = memory_order_seq_cst) 针对指针的
                                将　val 加到原来的保存的值中, 并返回原子对象的旧值
                                
                            8.
                                T fetch_sub (T val, memory_order sync = memory_order_seq_cst) 针对整型的
                                T fetch_sub (ptrdiff_t val, memory_order sync = memory_order_seq_cst) 针对指针的
                                将原子对象的封装值减 val，并返回原子对象的旧值
                                
                            9. T fetch_and (T val, memory_order sync = memory_order_seq_cst)
                                将原子对象的封装值按位与 val，并返回原子对象的旧值（只适用于整型的 std::atomic 特化版本）
                                
                            10. T fetch_or (T val, memory_order sync = memory_order_seq_cst)
                                将原子对象的封装值按位或 val，并返回原子对象的旧值（只适用于整型的 std::atomic 特化版本）
                                
                            11. T fetch_xor (T val, memory_order sync = memory_order_seq_cst)
                                将原子对象的封装值按位异或 val，并返回原子对象的旧值（只适用于整型的 std::atomic 特化版本）
                        
                    
                
                    
```
## boost
```shell
    1. Boost.TypeIndex(精准的直到表达式或则变量的类型), 不需要任何动态链接库
            (1) 标准库中有 typeid 这个关键字, 它会返回 std::type_info 的结构体, 其中 type_info.name() 是打印具体的结果,可读性较差.
                其忽略了 const 修饰，也不能输出变量的引用类型
            (2) type_id 则是 boost 的库,  其忽略了 const 修饰，也不能输出变量的引用类型
            (3) type_id_with_cvr 和 type_id 相似，但它获得的结果会包含 const/volatile 状态及引用类型,更全面一点.
                模板参数必须包含引用类型, 使用 decltype((v)) 这种写法，而不是 decltype(v)
                
            实例代码:
                    vector v;
                    auto it = v.cbegin();
                    cout << "*** Using typeid\n";
                    cout << typeid(const int).name() << endl;
                    cout << typeid(v).name() << endl;
                    cout << typeid(it).name() << endl;
                
                    cout << "\n\n\n*** Using boost type_id\n";
                    cout << type_id() << endl;
                    cout << type_id() << endl;
                    cout << type_id() << endl;
                
                    cout << "\n\n\n*** Using  boost type_id_with_cvr\n";
                    cout << type_id_with_cvr() << endl;
                    cout << type_id_with_cvr() << endl;
                    cout << type_id_with_cvr() << endl;
                    cout << type_id_with_cvr() << endl;
                    cout << type_id_with_cvr() << endl;
                    
                    输出:
                    *** Using typeid
                    i
                    St6vectorIiSaIiEE
                    N9__gnu_cxx17__normal_iteratorIPKiSt6vectorIiSaIiEEEE
                    
                    
                    
                    *** Using boost type_id
                    int
                    std::vector >
                    __gnu_cxx::__normal_iterator > >
                    
                    
                    
                    *** Using  boost type_id_with_cvr
                    int const
                    std::vector >
                    std::vector >&
                    std::vector >&&
                    __gnu_cxx::__normal_iterator > >&
                
    2. Boost.Core(通用工具), 不需要任何动态链接库
            (1) boost::core::demangle: 配合 typeid 使用, 将 typeid 返回的可读性较差的转化为可读性好的
                    cout << demangle(typeid(const int).name()) << endl;
    3. Boost::noncopyable : 类不可拷贝
            #include
            class shape_wrappe:private boost::noncopyable
            {
            
            }
    4. Boost.Conversion (数据类型的转化, int -> string, string -> int), 不需要任何动态链接库
    
            #include
            #include
            #include
            #include
            using namespace std;
            using boost::bad_lexical_cast;
            using boost::lexical_cast;
            
            // 整数到字符串的转换
            int d = 42;
            auto d_str = lexical_cast(d);
            
            // 字符串到浮点数的转换
            auto f = lexical_cast(d_str) /4.0;
            
            // 注意 float -> string 没法做到进度保留
            double srcf = 11111.22;
            auto sTof = lexical_cast(srcf);  // sTof 为"11111.2"
            
    5. Boost.Program_options(运行的选项处理库) 需要链接 libboost_program_options.so   动态链接库
            (1) 较简单的形式获取运行可执行文件传过来的参数(./example -l xxx) 要比 getopt 简单多了
            (2) 实例代码
                        #include
                        namespace po = boost::program_options;
                        
                        string localion;
                        string lang;
                        int width = 72;
                        bool keep_indent = false;
                        bool verbose = false;

                        po::options_description desc("Usage: breaktext [OPTION]...   [Output File] \n\n"
                                                             "Available options");
                        desc.add_options()
                                ("locale,L", po::value(&localion), "Locale of the console (system locale by default)")
                                ("lang,l", po::value(&lang), "Language of input (asssume no language by default)")
                                ("width,w", po::value(&width), "Width of output text (72 by default)")
                                ("help,h", "Show this help message exit")
                                (",i", po::bool_switch(&keep_indent), "Keep space indentation")
                                (",v", po::bool_switch(&verbose), "Be verbose");
                    
                        po::variables_map vm;
                        try {
                            po::store(po::parse_command_line(argc, argv, desc), vm);
                        }
                        catch (po::error& e) {
                            cout << e.what() << endl;
                        }
                    
                        vm.notify();  // notify 成员函数用来把变量映射表的内容实际传送到选项值描述里提供的那些变量里去
                        if (vm.("help")) {
                            cout << desc << "\n";
                        }
                    
                        printf("width[%d] lang[%s]\n", width, lang.c_str());
                        
                        输出:
                            (1) ./example_test -h
                                Usage: breaktext [OPTION]...   [Output File] 
                                
                                Available options:
                                  -L [ locale ] arg   Locale of the console (system locale by default)
                                  -l [ lang ] arg     Language of input (asssume no language by default)
                                  -w [ width ] arg    Width of output text (72 by default)
                                  -h [ help ]         Show this help message exit
                                  -i                    Keep space indentation
                                  -v                    Be verbose
                                  
                            (2)  ./example_test -l ssss
                                width[72] lang[ssss]
                                
    6. Boost::circular_buffer(循环缓冲区)
            (1) Boost.Circular_buffer 维护了一块连续内存块作为缓存区，当缓存区内的数据存满时,
                继续存入数据就覆盖掉旧的数据(可以是覆盖最新的, 也可以是覆盖最老的)
            (2) 支持 push(elem),pop 支持随机访问
            
```

## 单元测试库
```shell
    1. Boost.Test
    2. Catch2: 只需要单个头文件即可使用，不需要安装和链接
            (1) 测试流程使用的 BDD(Behavior-Driven Development): 行为驱动开发模式
            (2) BDD 结构
                    Scenario(场景, 要做的某某事)
                    {
                        Given(给定, 已有的条件)
                        {
                            When(当某件事发生时)
                            {
                                Then(那样, 会发生说明)
                            }
                            
                            When(当某件事发生时)
                            {
                                Then(那样, 会发生说明)
                            }
                        }
                    }
            (3) 例子
                    SCENARIO("Int container can be accessed modified [container]")
                    {
                        GIVEN("A container with initialized items")
                        {
                           
                            IntContainer c{1, 2, 3, 4, 5};
                            REQUIRE(c.size() == 5);
                            
                            WHEN("I access existing items")
                            {
                                THEN("The items can be retrieved intact")
                                {
                                    CHECK(c[0] == 1);
                                    CHECK(c[1] == 2);
                                    CHECK(c[2] == 3);
                                    CHECK(c[3] == 4);
                                    CHECK(c[4] == 5);
                                }
                            } 
                            
                            WHEN("I modify items")
                            {
                                c[1] = -2;
                                c[3] = -4;
                                THEN("Only modified items are changed")
                                {
                                    CHECK(c[0] == 1);
                                    CHECK(c[1] == -2);
                                    CHECK(c[2] == 3);
                                    CHECK(c[3] == -4);
                                    CHECK(c[4] == 5);
                                }
                            }
                        }
                    }
                    
            (4) 总结
                    a. 在编译引入的时候可以将
                            #define CATCH_CONFIG_MAIN
                            #include "catch.hpp" 
```

## 日志库
```shell
    1. Easylogging++ 日志库
            (0) 简单, 不需要编译生成动态链接库, 直接引用 .h 和 .cpp 就行
            (1) 可以使用配置文件来设置日志选项(包括打印日志文件名, 时间格式等)
                    #include "easylogging++.h"
                    INITIALIZE_EASYLOGGINGPP
                    
                    el::Configurations conf{"log.conf"};
                    el::Loggers::reconfigureAllLoggers(conf);
                    LOG(DEBUG) << "A debug message";
                    LOG(INFO) << "An info message";
                    
                    编译的时候加上 -DELPP_NO_DEFAULT_LOG_FILE
            (2) 性能跟踪功能, 可以粗略统计函数运行的时间
            (3) 记录程序崩溃日志, 通过定义 ELPP_FEATURE_CRASH_LOG  编译选项, 可以在日志中记录调用栈信息, 通过调用栈的信息, 
                再用 addr2line 工具分析那一行引发崩溃.
    2. spdlog 日志库
            (1) 日志记录满,日志文件切换很简单
                    #include "spdlog/sinks/rotating_file_sink.h"
                    
                    // 文件大小为 5MB，一共保留 3 个日志文件
                    auto file_sink = make_shared("test.log", 1048576 * 5, 3);  
                    
            (2) 适配用户定义的流输出
                    可以在用户自定义实现 "<<" 配合 spdlog 使用的 ostream 输出, 就可以打印容器的值了
                     
                     #include "output_container.h"
                     #include "spdlog/fmt/ostr.h"
                     
                     vector v;
                     // …
                     spdlog::info("Content of vector: {}", v);
```

## c++ rest sdk
```shell
    1. 
```

## STL 函数

```shell
    1. std::back_inserter() 函数,在尾部插入元素.
       
            template
              back_insert_iterator back_inserter (Container& x);
              
            功能: 构造一个迭代器在容器末尾添加元素
            返回值: 返回的是新增的迭代器
            注意: 使用 back_inserter 的容器是有 push_back 成员函数的容器, 比如 vector, deque list
            
    2. 全局 std::begin() , std::end()
            功能: 为容器和数组提供同一的迭代器, 
                    如果 std::begin() 传入的参数是容器,则调用的是 contain.begin(). 传入的参数是数组, 则返回一个指向该数组首个元素的指针 
                    如果 std::begin() 传入的参数是容器,则调用的是 contain.end(). 传入的参数是数组, 则返回一个指向该数组最后一个元素下一个地址的指针 
```

## Unicode 编码
```shell
    1. Unicode 的编码点从 0x0 到 0x10FFFF, 一个 Unicode 字符通过 U+6C49(汉) 这种形式表示
    2. Unicode 字符常见编码方式:
            (1) UTF-32 : 32 bit, 直接映射
            (2) UTF-16: 16 bit, 可以用 16 bit 表示的(U+0000 到 U+FFFF) 直接映射, 之外有专门的算法
            (3) UTF-8 : 1 到 4 字节的变长编码
    3. Unicode 文本文件通常有一个使用 BOM(byte order mark)字符的约定，即字符 U+FEFF, 通过这个 FEFF 的存储方式, 可以确定编码方式
    　　例如 文件开头是 0x00 00 FE FF，那这是大头在前的 UTF-32 编码,
           如果是 0xEF BB BF，那这是 UTF-8 编码
```

## 模板
```shell
    1. 在整个的编译过程中，可能产生多个这样的（相同）实例，但最后链接时，会只剩下一个实例。这也是为什么 C++ 会有一个单一定义的规则：
       如果不同的编译单元看到不同的定义的话，那链接时使用哪个定义是不确定的
    2. 如果在定义一个模板时(不管是函数模板还是类模板), 如果使用某个具体类(或则数据类型)进行实例化时, 该模板中一些函数功能, 
       这个具体类(或则数据类型)不能满足.则可以通过一下三种方式进行:
            (1) 在具体类中, 直接增加代码来支持函数功能
            (2) 对于函数模板, 直接针对这个类进行重载
                    cln::cl_I　my_mod(const cln::cl_I& lhs,　const cln::cl_I& rhs)
                    {
                        return mod(lhs, rhs);
                    }
            (3) 对于类模板, 可以针对这个类进行特化
                    template <>
                    cln::cl_I my_mod(const cln::cl_I& lhs,　const cln::cl_I& rhs)
                    {
                        return mod(lhs, rhs);
                    }
    3. 运行时的多态(通过虚函数的方式), 静态的多态则是泛型编程的模板实例化
       
```

## 工具
```shell
    1. 代码格式化工具 clang-format
    2. 代码检测工具
        (1) Clang-Tidy
                clang-tidy checks=clang-analyzer-*,modernize-*,readability-* test.cpp
        (2) cppcheck
    3. 排查工具
        (1) valgrind
    4. 高级语言和汇编的映射(Compiler Explorer)
            https://godbolt.org/z/zPNEJ4
            
       模板展开网站 https://cppinsights.io/
                
```

## 归纳
```shell
    1. 对 auto 自动类型推导使用
            #define TYPE_DISPLAY(var) \
                static type_displayer type_display_test
                
            template // declaration only for type_displayer;
            class type_displayer;
            
            用的时候，就写 TYPE_DISPLAY(变量名字)
```