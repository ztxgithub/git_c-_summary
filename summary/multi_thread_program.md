# C++  多线程系统编程

## 多线程下对象的生命期管理

### 对象的创建

```shell
    1.对象构造要达到多线程环境下的线程安全，只需要在构造函数中不泄露 this 指针，
      表现如下:
            A. 不要在构造函数中将　this 指针传给其他对象(因为这个时候 this 对象还没初始化完全，刚好其他对象访问
            　　这个半成品的对象会有问题)
            B. 在构造函数中即使是最后一行也不能泄露 this 指针(因为要考虑到该对象为基类，执行完基类的构造函数
            　　还会继续执行派生类的构造函数)
            
    2. 实例
            A. 错误用法
                class Foo: public Observer
                {
                    public:
                        Foo(Observer *s)　　//　不能在构造函数中将 this 指针泄露出去
                        {
                            s->register(this);  //错误，非线程安全
                        }
                }
                
            B. 正确用法
                class Foo: public Observer　
                {
                    public:
                        Foo()；
                        void observer_register(Observer *s)
                        {
                            s->register(this);  //正确，等构造函数执行完后才进行其他操作
                        }
                }
```

### 死锁情况

```shell
    (1) 
        void swap(Counter &a, Counter &b)
        {
            Lock(a.mutex);
            Lock(b.mutex);
            int value = a.value;
            a.value = b.value;
            b.value = value;
        }
        
        如果线程A执行 swap(a, b), 而线程B执行 swap(b, a) 就有可能产生死锁
        
        解决方案:
            可以比较 mutex对象的地址，始终加锁地址小的mutex对象
```

### 原始指针

```shell
    1.简介
        在多线程的情况下，指向对象的原始指针是很不安全的，很容易造成空悬指针
        
    2.空悬指针
            有指针 p1 和指针 p2 同时指向一个对象，如果线程 A 通过 p1 将该对象释放，则线程 B 中的 p2 就是空悬指针，p2 虽然有值
            但它指向的对象已经被释放掉(p2 却感知不到)
            
    3. 解决空悬方案
            A. 加入一层 proxy(代理)
                    将指针 p1 和 指针 p2 指向永久有效的对象 proxy, 而 proxy 对象指向 Object 原始指针
                    当销毁 Object 原始指针时，proxy == 0, 则 p2　就可以通过判断　proxy　的值看 Object
                    对象是否被释放
                    
                    不足:
                        p2判断 proxy 的值不为0时，正准备引用 Object　对象相关的数据时，期间处于另外一个线程的 p1 
                        销毁了 Object 对象
                        
            B. 使用 shared_ptr / weak_ptr (推荐使用)
```

## 线程同步

### mutex(互斥器)

```shell
    1.使用 RAII 机制封装 mutex 的创建，销毁，加锁，解锁，即保证锁的生效期间等于一个作用域，不会因异常而忘记解锁
    2.不手工调用 lock() 和　unlock() 函数，一切交给栈上的 Guard 对象的构造函数(加锁操作)和析构函数(解锁操作)，
    　这样保证同一个函数同一个 scope　内有加锁和解锁一对操作，避免在 foo() 函数内加锁，出现异常忘记解锁而导致死锁
    　现象，这种做法被称为 Scoped Locking
    
    3.如果一个函数既可能在已加锁的情况下调用，也可能在未加锁的情况下调用，那么就得拆分2个函数
        (1) 跟原来的函数名一致，新的函数内加锁，转而调用第二个函数
        (2) 给函数名加上 withlockhold ,不加锁，里面的具体的业务代码
        
        void post(const Foo &f)
        {
        	MutextLockGuard lock(mutex);  //进行加锁
        	postWithLockHold(f);
        }
        
        void postWithLockHold(const Foo &f)
        {
        	具体的业务代码
        }

```

### 条件变量

```shell
    1.条件变量是非常底层的同步原语，很少直接使用，一般用它来实现高层的同步措施，例如 BlockingQueue<T> 
      或则 CountDownLatch(倒时器)，如果多线程中大量使用 mutex 或则 Condition 底层同步原语，则程序显得复杂而
      庞大
      
    2. CountDownLatch 是一个同步方式，主要用途:
            (1) 主线程等待多个子线程完成初始化后才能继续执行
            (2) 多个子线程等待主线程完成一定操作后才开始执行
            
            class CountDownLatch : boost::noncopyable
            {
             public:
            
               explicit CountDownLatch(int count)
                  : mutex_(),                   // 初始化顺序要与成员申明保存一致
                    condition_(mutex_),
                    count_(count)
                {
                }
            
              void wait();
            
              void countDown();
            
              int getCount() const;
            
             private:
              mutable MutexLock mutex_;  //　顺序很重要，先 mutex 后 condition
              Condition condition_;
              int count_;
            };
            
            void CountDownLatch::wait()
            {
              MutexLockGuard lock(mutex_);
              while (count_ > 0)
              {
                condition_.wait();
              }
            }
            
            void CountDownLatch::countDown()
            {
              MutexLockGuard lock(mutex_);
              --count_;
              if (count_ == 0)
              {
                condition_.notifyAll();　// 其中这里为　Condition::notifyAll() 而不是　Condition::notify()　主要是考虑到
                                        //  向多个子线程发送消息，而 Condition::notify() 则是向单个线程发送消息
              }
            }
```

## 利用 share_ptr 实现 copy-on-wirte(读写锁的效果)

```shell
    1.用普通的 mutex 来替换读写锁，原理如下
            (1) shared_ptr<Foo> 是　Foo 对象的指针
            (2) 对于 reader 端，定义局部变量　shared_ptr<Foo> sp_read, 进行赋值则对应的引用计数加 1, 当出了区域
            　　 对应的引用计数减１
            (3) 对于 write 端，当发现对象的引用计数为 1 时，可以放心原地修改，如果不为1则需要通过 share_ptr.reset()
                函数，将数据拷贝出来进行修改
                
            具体代码:
            
                typedef std::vector<Foo> FooList;
                typedef boost::shared_ptr<FooList> FooListPtr;  // 定义了 FooList*, 通过智能指针进行管理
                FooListPtr g_foos;
                MutexLock mutex;
                
                writer 端
                    void post(const Foo& f)
                    {
                          printf("post\n");
                          MutexLockGuard lock(mutex);  // 对　g_foos 进行互斥
                          if (!g_foos.unique())
                          {
                            g_foos.reset(new FooList(*g_foos));  // 通过将原来老的数据拷贝到 g_foos, 使其他线程对应读数据
                            printf("copy the whole list\n");　　　// 不受影响
                          }
                          assert(g_foos.unique());
                          g_foos->push_back(f);
                    }
                    
                reader 端
                
                    void traverse()
                    {
                      FooListPtr foos;
                      {
                        MutexLockGuard lock(mutex);
                        foos = g_foos;
                        assert(!g_foos.unique());  // 这边　foos = g_foos 后, g_foos 的引用计数肯定要大于 2
                      }
                    
                      // assert(!foos.unique()); this may not hold
                    
                      for (std::vector<Foo>::const_iterator it = foos->begin();
                          it != foos->end(); ++it)
                      {
                        it->doit();
                      }
                    }
```

## 单线程服务器的常用编程模型

```shell
    1.Reactor 模型
        (1) 简介
                Reactor 模式是处理并发 I/O 比较常见的一种模式，用于同步I/O，中心思想是将所有要处理的 I/O 事件注册到一个
                中心 I/O 多路复用器上，同时主线程/进程阻塞在多路复用器上；一旦有I/O事件到来或是准备就绪(文件描述符或socket可读、写)，
                多路复用器返回并将事先注册的相应 I/O 事件分发到对应的处理器中。
                
        　　    Reactor 是一种事件驱动机制，和普通函数调用的不同之处在于： Reactor 模式中应用程序不是主动的调用
        　　　　某个API完成处理，而是恰恰相反，Reactor逆置了事件处理流程，应用程序需要提供相应的接口并注册到Reactor上，
               如果相应的事件发生，Reactor将主动调用应用程序注册的接口，这些接口又称为“回调函数”
               
        (5) 关键组件
                A. 描述符（handle）
                        由操作系统提供的资源，用于识别每一个事件，如Socket描述符、文件描述符、信号的值等。在Linux中，
                        它用一个整数来表示。事件可以来自外部，如来自客户端的连接请求、数据请求等。事件也可以来自内部，
                        如信号、定时器事件。
                        
                B. 同步事件多路分离器（event demultiplexer）
                        事件的到来是随机的、异步的，无法预知程序何时收到一个客户连接请求或收到一个信号。
                        所以程序要循环等待并处理事件，这就是事件循环。在事件循环中，等待事件一般使用I/O复用技术实现。
                        在linux系统上一般是 select、poll、epoll_wait 等系统调用，用来等待一个或多个事件的发生。
                        I/O 框架库一般将各种 I/O 复用系统调用封装成统一的接口，称为事件多路分离器。
                        调用者会被阻塞，直到分离器分离的描述符集上有事件发生。
                        
                C. 事件处理器（event handler）
                        I/O 框架库提供的事件处理器通常是由一个或多个模板函数组成的接口。这些模板函数描述了和
                        应用程序相关的对某个事件的操作，用户需要继承它来实现自己的事件处理器，即具体事件处理器。
                        因此，事件处理器中的回调函数一般声明为虚函数，以支持用户拓展。
    
```


