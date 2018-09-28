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

### 利用 share_ptr 实现 copy-on-wirte (读写锁的效果)

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
                            printf("copy the whole list\n");　　　// 不受影响,因为新的 g_foos 的引用为 1, 而老的 g_foos
                          }                                      // 引用大于 1,　有其他 reader 在引用，当 reader 引用完后，
                          assert(g_foos.unique());　　　　　　　　 // 会自动释放老的 g_foos(之所以怎么做 reader 读没有修改过的数据)
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

### 单线程服务器的常用编程模型

```shell
    1.Reactor 模型
        (1) 简介
                Reactor 模式是处理并发 I/O 比较常见的一种模式，用于同步I/O，中心思想是将所有要处理的 I/O 事件注册到一个
                中心 I/O 多路复用器上，同时主线程/进程阻塞在多路复用器上；一旦有I/O事件到来或是准备就绪(文件描述符或socket可读、写)，
                多路复用器返回并将事先注册的相应 I/O 事件分发到对应的处理器中。
                
        　　    Reactor 是一种事件驱动机制，和普通函数调用的不同之处在于： Reactor 模式中应用程序不是主动的调用
        　　　　某个API完成处理，而是恰恰相反，Reactor逆置了事件处理流程，应用程序需要提供相应的接口并注册到Reactor上，
               如果相应的事件发生，Reactor将主动调用应用程序注册的接口，这些接口又称为“回调函数”
               
        (2) 关键组件
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
                        
                D. 具体的事件处理器（concrete event handler）
                        是事件处理器接口的实现。它实现了应用程序提供的某个服务。每个具体的事件处理器总和一个描述符相关。
                        它使用描述符来识别事件、识别应用程序提供的服务。
                        
                E. Reactor 管理器（reactor）
                        定义了一些接口，用于应用程序控制事件调度，以及应用程序注册、删除事件处理器和相关的描述符。
                        它是事件处理器的调度核心。 Reactor管理器使用同步事件分离器来等待事件的发生。
                        一旦事件发生，Reactor管理器先是分离每个事件，然后调度事件处理器，
                        最后调用相关的模板函数来处理这个事件。
                        
        (3) 反应堆
                背景：
                    基于事件驱动的IO复用编程在高并发，高需求(业务量大)情况下有压力，服务器共有10万个并发连接，此时，
                    一次IO复用接口的调用返回了100个活跃的连接等待处理。先根据这100个连接找出其对应的对象，
                    这并不难，epoll的返回连接数据结构里就有这样的指针可以用。接着，循环的处理每一个连接，
                    找出这个对象此刻的上下文状态，再使用read、write这样的网络IO获取此次的操作内容，
                    结合上下文状态查询此时应当选择哪个业务方法处理，调用相应方法完成操作后，若请求结束，
                    则删除对象及其上下文， 这样，我们就陷入了面向过程编程方法之中了，我们的主程序需要关注各种不同类型的请求，
                    在不同状态下，对于不同的请求命令选择不同的业务处理方法。这会导致随着请求类型的增加，
                    请求状态的增加，请求命令的增加，主程序复杂度快速膨胀，导致维护越来越困难。
                    
                解决方法:
                    反应堆模式可以在软件工程层面，将事件驱动框架分离出具体业务，将不同类型请求之间用OO的思想分离。
                    通常，反应堆不仅使用IO复用处理网络事件驱动，还会实现定时器来处理时间事件的驱动
                    （请求的超时处理或者定时任务的处理）
                    
                内容:
                    (1) 处理应用时基于OO思想，不同的类型的请求处理间是分离的。例如，A类型请求是用户注册请求，
                        B类型请求是查询用户头像，那么当我们把用户头像新增多种分辨率图片时，更改B类型请求的代码处理逻辑时，
                        完全不涉及A类型请求代码的修改。
                        
                    (2) 应用处理请求的逻辑，与事件分发框架完全分离。即写应用处理时，不用去管何时调用IO复用，
                        不用去管什么调用epoll_wait，去处理它返回的多个socket连接。应用代码中，只关心如何读取、发送socket上的数据
                        如何处理业务逻辑。事件分发框架有一个抽象的事件接口，所有的应用必须实现抽象的事件接口，
                        通过这种抽象才把应用与框架进行分离。
                        
                    (3) 反应堆上提供注册、移除事件方法，供应用代码使用，而分发事件方法，通常是循环的调用而已，
                        是否提供给应用代码调用，还是由框架简单粗暴的直接循环使用，这是框架的自由。
                        
                    (4) IO多路复用也是一个抽象，它可以是具体的select，也可以是epoll，
                        它们只必须提供采集到某一瞬间所有待监控连接中活跃的连接
                        
                    (5) 定时器也是由反应堆对象使用，它必须至少提供4个方法，包括添加、删除定时器事件，这该由应用代码调用。
                        最近超时时间是需要的，这会被反应堆对象使用，用于确认select或者epoll_wait执行时的阻塞超时时间，
                        防止IO的等待影响了定时事件的处理。遍历也是由反应堆框架使用，用于处理定时事件。

     
    2.Proacotr模型
         在Proactor模式中，事件处理者(或者代由事件分离者发起)直接发起一个异步读写操作(相当于请求)，
         而实际的工作是由操作系统来完成的。发起时，需要提供的参数包括用于存放读到数据的缓存区，读的数据大小，
         或者用于存放外发数据的缓存区，以及这个请求完后的回调函数等信息。
         事件分离者得知了这个请求，它默默等待这个请求的完成，然后转发完成事件给相应的事件处理者或者回调。
         
    3.Reactor 模型 和　Proacotr模型区别
        Reactor是在事件发生时就通知事先注册的事件（读写由处理函数完成）；
        Proactor是在事件发生时进行异步I/O（读写由OS完成），待IO完成事件分离器才调度处理器来处理。
        
        在Reactor（同步）中实现读：
         - 注册读就绪事件和相应的事件处理器
         - 事件分离器等待事件
         - 事件到来，激活分离器，分离器调用事件对应的处理器。
         - 事件处理器完成实际的读操作，处理读到的数据，注册新的事件，然后返还控制权。
         
        Proactor（异步）中的读：
         - 处理器发起异步读操作（注意：操作系统必须支持异步IO）。在这种情况下，处理器无视IO就绪事件，它关注的是完成事件。
         - 事件分离器等待操作完成事件
         - 在分离器等待过程中，操作系统利用并行的内核线程执行实际的读操作，并将结果数据存入用户自定义缓冲区，
         　最后通知事件分离器读操作完成。
         - 事件分离器呼唤处理器。
         - 事件处理器处理用户自定义缓冲区中的数据，然后启动一个新的异步操作，并将控制权返回事件分离器。
       
```

### 多线程服务器的常用编程模型

```shell
    1.编程模型
        (1) 使用线程池，线程内使用 I/O 阻塞操作
        (2) non-blocking IO + one loop per thread
                程序里每个 IO 线程都有 event loop(也称为 Reactor),event loop 是个循环用于处理读写事件和定时事件
                一个程序可以有多个 event loop, 每个 IO 线程对应一个 event loop,对于实时性有要求的 connection
                可以独占一个线程，数据量大的 connection 独占一个线程，并把数据处理任务分摊给几个计算线程(线程池)
                其他次要的 connection 共享一个线程
                
    2. 推荐模式(event loop + thread pool)
            (1) event loop : IO 多路复用以及非阻塞操作
            (2) thread pool 用来做计算，例如任务队列 blockingqueue
            (3) 以这种方式写服务器程序，需要一个基于 Reactor 模式的网络库来支撑(muduo　网络库)
            
```

### 进程间通信方式(首选 TCP)

```shell
    1.进程间通信使用 TCP 的理由
        (1) 可以跨主机，具有伸缩性
                一台机器处理能力不够，可以在多态进行部署，只要将 ip:port 进行配置即可
                
        (2) 如果其中一个进程崩溃，另外的进程会立马感知
        (3) 可记录，可重现，可以使用 tcpdump 或则　Wireshark 工具进行记录，　通过 tcpcopy 进行压力测试
        (4) 跨语言通信
                
    2.　进行间通信 TCP 与 pipe 区别
            相同点: 
                  (1) 都是操作文件描述符用来收发字节流
                  (2) 都可以使用 read/write/fcntl/select/epoll 等系统调用
                  
            不同点:
                  (1) TCP 传输数据是双向的， pipe 传输数据是单向的
                  (2) pipe 只能在父子进行间通信
                    
```

### 服务器提供服务 model

```shell
    1.运行一个单线程的进程
        (1) 必须使用单线程的场景
                A. 程序可能会使用 fork()系统调用，即启动看门狗进程(守护进程)
                B. 需要限制程序的 CPU 占用率．　因为是单线程程序，所以最多只占用一个核，例如对于辅助性的程序(日志的压缩备份)
                　　该程序比较适合单线程(gzip),　它对系统的性能影响小，不会给正常的主业务进程造成影响(业务处理变慢)
                
        (2) 缺点
                单线程程序采用 event loop 模式，它是非抢占式，不考虑优先级只考虑谁来的早
                
    2.运行多线程的进程 和　运行多个单线程的进程的选择
        取决于服务程序响应一次请求所访问的内存大小，如果程序需要一个较大的本地 Cache 用来缓存基础数据，而每次请求
        则需要访问该内存，则使用多线程的进程比较合适．
        
```

## C++ 多线程系统编程

### C++ 系统库的线程安全性

```shell
    1. 一个共享对象(例如 Vector)被多个线程使用，其中某线程有 write 操作，另外线程即使是 read-only(Vector.size()) 也
     　要加锁
     
    2. c++ 的 iostream 不是线程安全的，　std:cout << "Now is" << time(NULL), 这个在多线程情况下可能存在
    　　内容输出的偏差，可能会有其他线程的打印信息．可以使用 printf() 来解决
    
    3. exit() 除了终止进程外，还会析构全局对象，这可能造成死锁
            例子:
                void fun_exit()
                {
                	exit(1);
                }
                
                class GlobalObject
                {
                	public:
                		void doit()
                		{
                			MutexLockGuard lock(mutex_);
                			fun_exit();
                		}
                		
                		~GlobalObject()
                		{
                			MutexLockGuard lock(mutex_); // 此次可能发生死锁
                		}
                		
                	private:
                		MutexLock mutex_;
                };
                
                GlobalObject g_obj;
                
                int main()
                {
                	g_obj.doit();
                }
                
            程序调用　GlobalObject::doit() 成员函数中辗转调用　exit()　函数，导致析构全局对象，调用
            ~GlobalObject()(这里又申请加锁，之前　doit() 函数已经加锁了，导致死锁)
            
    4. fork() 一般不能在多线程程序中调用，因为 fork() 后子进程就只有一个线程，其他线程都消失
    5. printf() 函数是线程安全的，它会持有 stdout/stderr 的锁，使得多个线程打印消息不会串．
    6. 通过 O_NONBLOCK 和　FD_CLOEXEC 对于 syscall(例如 accept4(), pipe2)　等支持，说明
       Linux 服务器开发的主流模型从 fork() + work processes 模型　到　多线程模型的转变．
       fork() 函数将来只专门负责启动别的进程的"看门狗程序"
       
    7. 线程是宝贵，一个程序的线程数最好与 CPU 核数保存一致，一台机器不应该运行几百个用户线程，这会大大增加
    　　内核调度的负担，减低性能
    8. 线程的创建和销毁是有代价的，一个程序最好能在开始创建所有所需的线程，并且一直使用，避免在运行过程中不断的
    　　创建和销毁线程，如果必须要这么做，频率可以减低到 1 分钟 1 次
    9. 每个线程应该有明确的职责，例如 IO 线程(EventLoop::loop()， 处理 IO 事件），计算线程(ThreadPool)
```

## 多线程日志

```shell
    1.日志通常用于故障诊断和追踪，也可用于性能分析．
    2. 对于关键进程，日记通常要记录
            (1) 收到的每条内部消息的 id (包括关键字段，长度，hash)
            (2) 收到的每条外部消息全文(全文内容较少)
            (3) 发出的每条消息的全文(全文内容较少)
            (4) 关键内部状态的变更
            
    3.功能需求
        (1) 日志的输出级别在运行中可调，同一个可执行文件可以在 QA 测试环境中输出 DEBUG 级别的日志，在生产环境
        　　　输出　INFO 级别的日志(通过环境变量设置)
        (2) 对于分布式系统的服务进程，日志的目的地只能写在本地磁盘，不能往网络系统(NFS)写日志，原因如下
                A. 日志的功能之一就是诊断网络故障(连接断开，网络不同，网络拥塞)，那日志还通过网络进行传输没用
                B. 如果接受网络日志的服务器发送故障，会导致发送日志的服务阻塞
                C. 增加网络带宽消耗
                
        (3) 日志文件的滚动(rolling), 滚动的条件有 2 个
                A. 文件大小(每写满 1GB 就更新到下一个文件)
                B. 时间(每天零点就新建一个日志文件，无论前一个文件有没有写满)
                
                
        (4) 程序崩溃问题
                程序崩溃，最后几条日志往往丢失(日志库不能每条消息都 flush 到磁盘)，解决方法如下:
                    A. 定期(默认 3 秒)将缓冲区的日志消息 flush 到磁盘
                    
        (5) 日志内容格式
                日期     时间  微妙　        线程　  日志级别　正文(消息内容)  源文件名:行号
              20180914  17:17:25.125770Z   23261   INFO     ......         src:num
              
                A. 避免在日志内容中出现正则表达式元字符(例如 '[' 和 ']'), 方便使用 less 命令进行查找 
                
    4.多线程异步日志
        一个日志库分为前端和后端，前端供应用程序调用 API,只管日志的打印，不管存储．后端则负责把日志信息存储到本地．可以
        有多个前端和一个后端．
        
        具体实现:
            (1) 采用类似的双缓冲机制，前端定义多个缓冲区保存日志信息，而后端也有多个缓冲区与前端的缓冲区进行交换．
                    第一种情况:
                        当前端一个缓冲区满了，则将日志消息保存到下一个缓冲区，将条件变量发送给后端，后端收到消息后
                        将后端空闲的缓冲区与前端包含所有内容的缓冲区进行交换(这个过程是加锁的状态下),之后后端
                        将日志消息保存到本地磁盘
                        
                    第二种情况:
                        当前端的日志打印比较慢且少，后端  cond_.waitForSeconds(flushInterval_) 等待一段时间
                        超时，也将会与前端缓冲区进行交换，并将日志消息保存到本地
                        
                    第三中情况:(一般不太可能)
                        如果前端日志打印太过频繁，而使得后端来不及更新到磁盘，采取的方法当后端缓冲区大小超过一定数量时
                        丢弃

```

## 并发网络服务程序设计

```shell
    1. 正确得使用 non-blocking IO 需要考虑的问题很多，不适合直接调用 Socket API , 需要一个功能完善的网络库支撑
    2. 分类
            (1) 方案0 : accept + read/write (先阻塞等待连接，再接受和发送数据，最后断开连接)
                内容: 该方案是阻塞型 IO , 一次服务一个客户(同一时间)，这个方案不适合长连接，
                      比较适合多个请求直接没有业务相关的短连接
                 
            (2) 方案1 : accept + fork (先阻塞等待连接,然后对每一个客户端创建一个子进程)
                内容: 称之为 child-per-client 或则 fork()-per-client,俗称 process-per-connection,
                      这种情况适合并发连接数不大的，计算响应的工作量大于 fork() 的开销，比如数据库服务器(PostgreSQL),
                      适合长连接
                      
            (3) 方案2 : accept + thread (先阻塞等待连接,然后对每一个客户端创建一个线程)
            　　　内容: 这是传统的 java 网络编程方案 thread-per-connection , 在 java1.4 引入 NIO 之前常用的方案
            　　　　　　 适合长连接，不适合短链接，并且伸缩性受线程数的影响
            
            (4) 方案5 : poll(Reactor) 
                内容: 单线程 IO 多路复用，单线程Reactor 模式其优点是网络库赋值数据的收发，而用户只需要关心业务逻辑
                　　　确定是事件的优先级得不到保证，因为 poll 返回后进行具体业务不会被其他紧急事件所打断，而且当
                　　　某个事件发生时，其需要延迟几秒再处理，这时不能用 sleep() 进行阻塞等待，这样其他事件也要被
                　　　阻塞了，解决方法是注册一个超时回调函数
                
            (5) 方案8 : Reactor + thread poll
                内容: 全部的网络 IO 工作都在一个 Reactor 线程完成，计算的任务交个 thread pool (线程池)，
                　　　比较适合计算任务相互独立，IO 压力不大(NIO 主要是网络数据的读写)
                
                     线程池另外一个作用是可以执行阻塞操作，在计算的线程池中进行操作，这样避免阻塞住 Reactor IO 线程
                     
            (6) 方案9 : Reactors in threads (one loop per thread)　
                内容: 这是 muduo 内置的多线程方案，也是 Netty 内置的多线程方案，有一个 main Reactor 负责
                　　　accept() 连接，把某个连接挂载 Reactor pools(可以采用 round-robin), 其中每一个
                　　　Reactor　代表一个线程(先 epoll , 再对事件进行处理), 以后该连接的每一事件都是通过
                　　　这个　Reactor　线程进程处理，　由于一个连接完全有一个线程管理，那么请求的顺序性
                　　　有保证，因为是采用　Reactor pools　的机制，可以防止一个 Reactor 线程处理能力饱和．
                
            (7) 方案10: Reactors in processes (每一个进程一个 Reactor )
                内容: Ngnix 的内置方案
                
                      
    3. 总结
            (1) 当一个线程阻塞等待 read() 上，如果又想要在该 socket 上发送数据(TCP 是全双工模式)
                    第一种方案: 用 2 个线程，一个读，一个写
                    第二种方案: IO 多路复用， 用一个线程就能处理多个连接，其复用的不是 IO 连接，而是复用线程，
                    　　　　　　那么 select / epoll 一定是配合 non-blocking IO,而使用 non-blocking IO
                              肯定要使用应用层的 buff, 其中初版的事件驱动模式是收到事件后判断该事件是 accept
                              还是读事件，然后进行相应的业务处理，这种做法其实不利于将来的功能扩展．
                              这时就引出 Reactor 模式使 event-driver(事件驱动)的网络编程有章可循，它的中心思想
                               是网络编程有很多事务性的工作，可以提取为公共的框架，而用户只需要将业务代码写到
                               回调函数中，将该回调函数注册到框架中，让事件产生时，框架自动调用用户的回调函数．
                               
                               
            (2) Reactor 事件循环所在的线程叫 IO 线程
            (3) 使用一个 event loop 还是多个 events loops
                    按照每千兆比特每秒的吞吐量配一个 event loop,　所以编写运行在千兆以太网的网络程序上，
                    用一个 event loop 就可以了．如果要实现不同事务优先级，则可以将一个 event loop 对应与
                    一个事务
                    
            (4) 推荐使用的网络编程模式 方案9 和 方案8 结合　one loop per thread　+ thread pool
            (5) 使用 non-blocking IO 肯定要使用应用层的 buff(即网络库必须要有 buff)
                    A. TcpConnection 必须要有 output buffer
                            由于操作系统发送缓冲区的限制，所以如果应用层要发送大数据的话， TCP底层协议的要发送
                            多次，但应用层程序不关心，只管生成数据，只要调用 TcpConnection::Send()［不阻塞］，其
                            网络库(TcpConnection::Send() 的实现) 负责具体的发送，所以应用层调用
                            TcpConnection::Send()完继续进行 event loop. 具体实现的流程是用户层将发送的数据 append 
                            到　对应 TcpConnection output buffer，　然后注册 POLLOUT 事件，一旦 socket 变得可写
                            就立刻发送剩下的数据．　如果　output buffer 里还有待发送的数据，而程序又想关闭连接，
                            那么这个时候网络库不能立马关闭连接，而是等数据发送完毕
                            
                
```

