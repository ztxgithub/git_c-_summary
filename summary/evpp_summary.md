# evpp 概要

## 基础知识
```shell
    1. std::enable_shared_from_this 的用法
            作用: 在一个对象内部构造该对象自身(this)的 shared_ptr 时, 即使该对象已经被 shared_ptr 管理着, 也不会造成对象被两个独立的
                 智能指针管理, 避免在析构时导致对象被重复释放, 这要求我们在对象内构造对象的智能指针时, 
                 必须能识别有对象是否已经由其他智能指针管理, 智能指针的数量, 并且我们创建智能指针后也能让之前的智能指针感知到.
                 
            使用: 继承 enable_shared_from_this 类, 调用 shared_from_this() 函数生成 shared_ptr
            
           struct Good : public std::enable_shared_from_this<Good>
           {
               void fun()
               {
                   shared_ptr<Good> sp{shared_from_this()};
                   
                   shared_ptr<Good> sp{this};  // 错误, 会导致资源重复释放
                    
                   cout<< sp.use_count() <<endl;
               }
           };
           
           shared_ptr<Good> sp{make_shared<Good>()};
           sp->fun(); //输出为2
```

## 类

## Any

```shell
    1. 保存任何类型的数据
    2. 使用
            Usage 1 :
           
               Buffer* buf(new Buffer());
               Any any(buf);
               Buffer* b = any_cast<Buffer*>(any);
               assert(buf == b);
               delete buf;
           
           
            Usage 2 :
           
               std::shared_ptr<Buffer> buf(new Buffer());
               Any any(buf);
               std::shared_ptr<Buffer> b = any_cast<std::shared_ptr<Buffer>>(any);
               assert(buf.get() == b.get());
           
           
            Usage 3 :
           
               std::shared_ptr<Buffer> buf(new Buffer());
               Any any(buf);
               std::shared_ptr<Buffer> b = any.Get<std::shared_ptr<Buffer>>();
               assert(buf.get() == b.get());
```

## Duration
```shell
    1. 这个 Duration 代表同一的时间计数, 以最小单位 nanoseconds(纳秒) 保存, 可以转为 秒, 毫秒, 微妙(Microseconds), 分钟, 小时,
       也可以将其转化为系统的 struct timeval
```

## EventLoop

```shell
    1. 包含有成员变量 evbase_(struct event_base*), 如果是调用无参的 EventLoop() 则内部自己创建 ,
       否则 EventLoop(struct event_base* base) 由外部传入
       
    2. EventLoop 使用流程
        (1) 第一: 先 EventLoop() 的初始化, 可以使用无参数的初始化 EventLoop eventLoop; 也可以采用有参数的初始化
                  explicit EventLoop(struct event_base* base), 
                        初始化 EventLoop(由外部传入 struct event_base)
                            1. 获取调用这个对象的线程 tid
                            2. 设置双端套接字
                                 A. 设置 双端套接字 的 pipe[1] 被通知时回调函数(EventLoop::DoPendingFunctors), 主要是
                                    为了处理在 EventLoop::vector<Functor> 的函数
                                 B. (1). 设置 event_ 事件对象属性, 与 pipe[1] 绑定, 监听可读
                                    (2). 设置 event_ 事件归属于 evbase_
                            3. 调用 event_add, 使 watcher_.event_(pipe[1) 加入事件队列中
                            4. 此时 EventLoop.status_ 为 kRunning
                            
        (2) 第二: 进行 EventLoop::Run() 监听事件, 通过 event_base_dispatch(evbase_), 
                 重新赋值 tid_ = std::this_thread::get_id();
                 
    3. EventLoop 提供的功能
        (1) EventLoop::Run() 调用 event_add, 使 watcher_.event_(pipe[1) 加入事件队列中, 调用 event_base_dispatch 不断进行循环调度事件
        (2) EventLoop::RunInLoop(const Functor& functor) 
               1. 如果 EventLoop 所在的线程是正在运行的, 并且外部调用 EventLoop::RunInLoop() 这个函数所在的线程与启动 EventLoop 的
                  线程是一样的, 则直接调用传入的函数
               2. 如果外部调用 EventLoop::RunInLoop() 这个函数所在的线程与启动 EventLoop 的线程是不一样的, 则先将这个函数保存到函数队列中,
                  再通过事件的形式给 PipeEventWatcher.pipe[0] 发消息, 触发双套接字的 pipe[1] 检测到事件, 调用注册的回调函数
                  (DoPendingFunctors, 处理待处理的函数), 这样就能保证所有调用 EventLoop::RunInLoop(f), 所处理的函数都在 EventLoop 所在
                  的线程中处理了
        (3) EventLoop::RunAfter(Duration delay, const Functor& f) 支持延迟一段时间进行调用自定义的函数(一次性), 
            EventLoop::RunEvery(Duration delay, const Functor& f) 支持周期性间隔时间进行调用自定义的函数 
            主要是通过向 InvokeTimer::Create(...), 再调用 InvokeTimer::Start()
            
    4. 实例代码
    
            void Print(evpp::EventLoop* loop, int* count) {
                if (*count < 3) {
                    std::cout << "Hello, world! count=" << *count << std::endl;
                    ++(*count);
                    loop->RunAfter(evpp::Duration(2.0), std::bind(&Print, loop, count));
                } else {
                    loop->Stop();
                }
            }
            
            evpp::EventLoop loop;
            int count = 0;
            loop.RunAfter(evpp::Duration(2.0), std::bind(&Print, &loop, &count));
            loop.Run();

```

## EventLoopThread
```shell
    1. EventLoopThread 类包含有 std::shared_ptr<EventLoop> event_loop_ 和 std::shared_ptr<std::thread> thread_
    2. EventLoopThread() 构造函数 new 出来 EventLoop
    3. EventLoopThread::Start(bool wait_thread_started, Functor pre, Functor post) 则负责创建线程, 并且在线程中
       执行 EventLoop.Run(), 其中 wait_thread_started 代表调用 EventLoopThread::Start 是否阻塞等待运行成功.
       Functor pre 则是在真正执行监听事件前的函数,不过也是通过 event_loop_->QueueInLoop(std::move(fn)) 执行
       
    使用:
         std::share_ptr<evpp::EventLoopThread> t(new evpp::EventLoopThread);
         t->Start(true);
         usleep(1000);
         t->loop()->RunAfter(delay, &OnTimeout);
         t->loop()->RunInLoop(&OnCount);
         
         // uinit
         t->Stop(true);
         t.reset();
```

## EventLoopThreadPool
```shell
    1. EventLoopThreadPool 主要是包含多个 EventLoopThread 类, 为了提高 IO 网络事件的处理效率, 使用多个 EventLoopThread 类
    2. EventLoopThreadPool(EventLoop* base_loop, uint32_t thread_number), 其中 base_loop 是由外部创建传入, 需要在外部进行
       base_loop.Run,  thread_number 则是代表需要 new 出多少个 EventLoopThread 类
    3. EventLoopThreadPool::Start(bool wait_thread_started) 这个函数主要做的事情是创建 EventLoopThreadPool.thread_num_ 
       个 EventLoopThread 类(EventLoopThread::Start()), 并保存到　EventLoopThreadPool.threads_ 中
    4. EventLoop* EventLoopThreadPool::GetNextLoop() 通过顺序获取 index 拿到对应的 EventLoop*(next = next % threads_.size())
       这边永远不会拿到外部传入的 base_loop
       
    使用:
            std::unique_ptr<evpp::EventLoopThread> loop(new evpp::EventLoopThread);
            loop->Start(true);
        
            int thread_num = 24;
            std::unique_ptr<evpp::EventLoopThreadPool> pool(new evpp::EventLoopThreadPool(loop->loop(), thread_num));
            pool->Start(true);
        
            for (int i = 0; i < thread_num; i++) {
                pool->GetNextLoop()->RunInLoop(&OnCount);
            }
        
            // uinit
            pool->Stop(true);
            loop->Stop(true);
            pool.reset();
            loop.reset();
```

## InvokeTimer
```shell
    1. InvokeTimer 类对外提供了 超时时间触发的函数的功能, 还有定期触发函数的功能.
    2. InvokeTimer::Create(EventLoop* evloop, Duration timeout, const Functor& f, bool periodic) 
            主要传入 Event_loop_, timeout(超时时间,多久后执行函数), 回调函数, 以及是否周期性执行
    3. InvokeTimer::Start() 则是 定义了 InvokeTimer 初始化函数(注册超时后触发回调函数, 并将这个事件加入到监听队列中), 
       将这个初始化函数通过 loop_->RunInLoop 去执行.这个主要是确保 InvokeTimer 初始化函数是在 loop_ 运行的线程中
```

## EventWatcher(事件监听的基类)
```shell
    1. EventWatcher 这个类的构造函数 EventWatcher::EventWatcher(struct event_base* evbase, const Handler& handler) 不对外调用, 
       是被继承类(PipeEventWatcher, TimerEventWatcher)的构造函数调用, 传入的 evbase(struct event_base*),
       通常是封装过的 EventLoop 类, handler 则是用户自定义的回调函数,处于最下面调用的, 
       PipeEventWatcher 和 TimerEventWatcher 中的回调函数会包含 EventWatcher 中的 handler_. 同时内部的
       bool EventWatcher::Watch(Duration timeout) 也是不对外开发的, 由被继承类(PipeEventWatcher, TimerEventWatcher)内部的
       AsyncWait() 调用, bool EventWatcher::Watch(Duration timeout) 就是调用 event_add() 将 event_ 加入到监听事件中.
    
    对外
    2. bool EventWatcher::Init() : 供外部调用, 里面县调用了继承类(PipeEventWatcher, TimerEventWatcher) 覆写的虚函数 DoInit()
       这个 DoInit() 做的事情是将调用 event_set() 将 event_ 与 fd_, 是否可读,是否可写以及触发回调函数进行绑定
       之后再调用 event_base_set() 设置该 event_ 归属于哪个 loop
```

## PipeEventWatcher

```shell
    1. PipeEventWatcher 这个类继承 EventWatcher(事件监听的基类), 通过 socketpair() 来创建相互连接的套接字
       pipe_[2]; // Write to pipe_[0] , Read from pipe_[1], pipe_[0]写的数据,在 pipe_[1] 原封不动读出来
       
       // 定义这个主要是适配 event_set 的回调函数
       static void HandlerFn(evpp_socket_t fd, short which, void* v);
       
    2. 因为 pipe_[1] 与 event_ 进行关联, 所以通过向 pipe_[0] 写数据, 则 pipe_[1] 对应的 event_事件被检测到, 再调用 
       HandlerFn 函数, HandlerFn 函数 再调用真正的用户自定义的函数.
       
    对外   
    3. PipeEventWatcher::PipeEventWatcher(EventLoop* loop, const Handler& handler) 主要是调用基类  
        EventWatcher::EventWatcher(struct event_base* evbase, const Handler& handler)
    4. bool PipeEventWatcher::AsyncWait() 主要是调用 基类的EventWatcher::Watch() 进行 event_add() 将事件加入到监听队列中．
    5. void PipeEventWatcher::Notify() 负责触发事件
```

## TimerEventWatcher
```shell
    1. TimerEventWatcher 这个类继承 EventWatcher(事件监听的基类), 只是简单的用做超时事件触发, 这个类本身没有业务功能,
       例如是否定时的触发,由传过来的回调函数决定的.
    2. TimerEventWatcher.DoInit() 的作用就是 event_set() 基类的 EventWatcher.event_ 与
       wrap 回调函数(TimerEventWatcher::HandlerFn), 自身 this 参数绑定, TimerEventWatcher::HandlerFn 里面右
       调用了用户自定义的回调函数(存放在基类中 EventWatcher.handle_)
      
    对外   
    3. TimerEventWatcher.TimerEventWatcher(....) 的构造函数 主要是传递给 基类 EventWatcher 的 event_base 和 用户自定义的
           回调函数 handle_, 还有就是超时时间 timeout_
    4. TimerEventWatcher::AsyncWait() 则是将已经设置号的 ev 事件通过 event_add() 加入到事件监听队列中, 同时在 event_add()
       函数中可以设置超时时间
       
    使用:
         std::share_ptr<TimerEventWatcher> timer_;
         
         timer_.reset(new TimerEventWatcher(loop_, std::bind(&Connector::OnConnectTimeout, shared_from_this()), timeout_));
         timer_->Init();  // 基类 EventWatcher 的方法
         timer_->AsyncWait();
         
         // 取消定时器
         timer_->Cancel();
         timer_.reset();
```

## FdChannel
```shell
    1. FdChannel 主要是包含有传入的 Eventloop_, 已连接的 fd_(由外部导入, FdChannel 类不负责释放),
       自己 new 出来的 event_(struct event*), 该fd_读事件触发的回调函数(read_fn_)由外部传入, 
       该fd_写事件触发的回调函数(write_fn_)由外部传入,  FdChannel 自身是不允许拷贝构造函数的. 同时负责设置 event_ 和 fd_
       绑定并注册事件触发时的回调函数(这个回调函数会调用外部传入的读事件回调函数和写事件回调函数), 以及 event_ 和 Eventloop_ 的从属关系, 
       并将该 event_ 加入监听队列中.
       
    2. FdChannel(EventLoop* loop, evpp_socket_t fd, bool watch_read_event, bool watch_write_event) 
     　　　　　loop 和 fd都是外面已经创建好传入的, watch_read_event  表示 fd 是否监控可读事件, watch_write_event  表示
      　fd 是否监控可写事件
       
    3. FdChannel::AttachToLoop() 
            首先调用 AttachToLoop() 这个函数的线程与 传入的 loop 进行 loop.Run 的线程要一样, 如果已经被绑定
      　attached_ 标志位为 true)了, 则先将 event_ 进行 event_del() 删除，通过 event_set() 进行 event_(struct event*)
       和 fd_ 的绑定, 同时设置该 event_ 的监听属性(是否可读事件,是否可写事件)以及该 event_ 是否持久化(EV_PERSIST, 
       当事件触发完后不需要再 EventAdd), 注册事件触发的回调函数, 设置该 event_ 归属于哪个 loop, 
       event_base_set(loop_->event_base(), event_), 调用 EventAdd() 将该 event_ 加入到事件监听队列中,
       一直等待事件触发,没有超时时间.
       
    4. FdChannel::EnableReadEvent()  
            这个函数的线程与 传入的 loop 进行 loop.Run 的线程要一样,使能可读事件,　如果设置后的事件与原先有变化
       (events_ |= kReadable), 则 更新事件操作, 采用的逻辑是 如果是没有要监听的事件类型了,则调用DetachFromLoop 
       (直接将已经绑定到 EventLoop 的 event_ 事件进行删除, 将 attached 置为 false), 否则, 调用 AttachToLoop()
       类似的操作有 EnableWriteEvent(), DisableReadEvent(), DisableWriteEvent(), DisableAllEvent()
       
    5. SetReadCallback(const ReadEventCallback& cb), SetWriteCallback(const EventCallback& cb) 
    　　　　　　设置用户定义的读,写事件的回调函数.
    
    使用:
        std::shared_ptr<FdChannel> chan_;
        chan_.reset(new FdChannel(loop_, fd_, false, true));
        chan_->SetWriteCallback(std::bind(&Connector::HandleWrite, shared_from_this()));  // 注册可写事件回调函数
        chan_->AttachToLoop();
        
        // 结束操作
        chan_->DisableAllEvent();
        chan_->Close();
```

## Listener
```shell
    1.  Listener 类 主要是接受外部传入的 loop_(EventLoop *), 以及监听的 "ip:port", 再根据这些信息创建一个 listen_fd, 
        对 listen_fd 进行网络设置, bind, listen, 之后结合 loop 进行事件监听, 创建出 listen FdChannel 成员变量, 再进行
        libevent 相关的操作, event_ 与 fd_ 的绑定关系,读事件的设置,读回调函数的注册, 以及 event_ 与 传入 loop 的从属关系,
        和 event_add 将这个 event_ 加入到监听事件
    
    2. void Listener::HandleAccept()
            当 listen_fd 检测到可读事件, 　再进行 accept() 得到已连接的 socket , 再进行 NewConnectionCallback 调用  
            new_conn_fn_(nfd, string_raddr, sock::sockaddr_in_cast(&ss));
    
    对外:    
    2. Listener::Listener(EventLoop* l, const std::string& addr) 
            传入外部的 EventLoop 和 要监听的 "ip:port"
            
    3. Listener::Listen(int backlog)
            创建一个非阻塞的 socket(设置 keepalive, reuseAddr, reusePort), 　将 socket 与 监听的 ip 和 port 进行 bind(), 
       再进行 listen() 监听
       
    4. Listener::Accept()
            构造 liseten_fd 的 FdChannel, 设置 listen_fd 读事件的回调函数 Listener::HandleAccept,将 FdChannel::AttachToLoop()
       函数 传到所属的 loop_ 的线程中运行. 
       
    5. Listener::SetNewConnectionCallback(NewConnectionCallback cb)
            设置已连接 socket 用户自定义回调函数, 这个主要是通过 Listener::HandleAccept() 进行调用
       
```

## TCPConn
```shell
    1. TCPConn 主要是供 TCPServer 和 TCPClient 类内部使用, TCPConn 保存有外部传入的 loop_(EventLoop*), 以及已连接的 fd_, 
       和还有 socket 连接时/断开连接的 conn_fn_ 回调函数. msg_fn_ 数据收到的回调函数, 这些都是从外部传进来的.
       内部创建的 chan_ (FdChannel), input_buffer_(从 socket 收到的接受缓冲区), output_buffer_(发送缓冲区), 
    2. TCPConn::HandleClose() :  调用 HandleClose() 前, TCPConn.status_ 状态应该为 kDisconnecting, 取消　TCPConn.FdChannel 
       所有类型事件监听, 将 TCPConn.FdChannel 所属的 event_ 从 loop 中删除, 如果有延迟 close 的定时器任务, 
       则进行　delay_close_timer_->Cancel(). 连接失败时, 调用 conn_fn_ 回调函数, 将 TCPConn.status_ 置为 kDisconnected
    3. TCPConn::HandleRead() : 当监听到可读事件时触发的回调函数, 先从 socket 中将数据读到 input_buffer_中, 
       其中 input_buffer_中 可能会动态增长(核心是调用 readv), 如果读取数据收到的字节数大于 0 时, 调用消息收到的回调函数 msg_fn_.
       如果收到的字节数等于 0 , 则代表连接断开.如果是作为客户端的连接断开, 则直接调用 TCPConn::HandleClose(). 
       如果是服务端检测到连接断开, 先进行 TCPConn.FdChannel::DisableReadEvent(), 禁止可读事件的监听.如果服务端设置了延迟关闭,
       则调用 loop_->RunAfter(close_delay_, std::bind(&TCPConn::DelayClose, shared_from_this())) 进行延迟调用
       DelayClose() 函数, 延迟关闭的作用是能够给服务器响应最后一个正常数据响应
       
    4. TCPConn::HandleWrite() : 可写事件的回调函数, 通过 send() 发送 output_buffer_ , 如果可以一次性发送, 
        则 chan_->DisableWriteEvent()(这样就不用可写事件的回调函数不断被触发), 并调用 write_complete_fn_ 函数. 如果通过 send()
        函数无法一次性将 output_buffer_ 发送完成, 则只需要偏移到具体的字节数
    5. TCPConn::OnAttachedToLoop() 这个函数主要由 TCPServer 和 TCPClient 类进行调用, 负责使能读事件 FdChannel.EnableReadEvent(),
       并且调用连接时的回调函数 conn_fn_
    6. TCPConn::SendInLoop(const void* data, size_t len): 初始化时　TCPConn 中 output queue 为空, 并且对应的内部的 FdChannel
       未设置为可写监听事件(代表目前没有正在进行 socket 发送), 则直接调用系统函数 send() 发送. 如果数据能一次性发送给对端, 
       则调用发送完成回调函数(write_complete_fn_). 如果在调用 SendInLoop 函数的当前已经有写事件(目前有数据在发送), 
       则将数据追加到 output_buffer_ 中, 等待写监听事件触发进行 send 系统函数调用.
       
    外部:   
    1. TCPConn::TCPConn(EventLoop* loop, const std::string& name, evpp_socket_t sockfd, const std::string& laddr,
                        const std::string& raddr, uint64_t conn_id)
        这个都是外部传入参数, 其中 name 为 TCPConn 的名字, sockfd 为连接的套接字. laddr 为 TCPServer 本地地址("ip:port"),
         raddr 为已连接客户端的 "ip:port", 同时创建了一个属于这个已连接的 FdChannel 类, 并对这个 FdChannel 设置可读事件的回调
         函数 TCPConn::HandleRead(), 可写事件的回调函数TCPConn::HandleWrite().
    2. TCPConn::Send(const void* data, size_t len), 供外部使用, 如果这个函数调用的线程跟 loop_.Run 的线程一致, 则直接调用 
        TCPConn::SendInLoop(const void* data, size_t len), 如果不再同一个线程中则调用  
        loop_->RunInLoop(std::bind(&TCPConn::SendStringInLoop, shared_from_this(), buf->NextAllString()));
        
    使用:
            typedef std::shared_ptr<TCPConn> TCPConnPtr;
            TCPConnPtr c = TCPConnPtr(new TCPConn(loop_, name_, sockfd, laddr, remote_addr_, id++));
            c->set_type(TCPConn::kOutgoing);
            c->SetMessageCallback(msg_fn_);
            c->SetConnectionCallback(conn_fn_);
            c->SetCloseCallback(std::bind(&TCPClient::OnRemoveConnection, this, std::placeholders::_1));
            c->OnAttachedToLoop();  // 只有在 TCPServer 和 TCPClient 类才能被调用
```

## TCPServer
```shell
    1. TCPServer 里面包含 Listener 类成员, 用于监听客户端的连接, TCPServer.connections_ 用于已连接 socket 的保存.
    2. TCPServer::HandleNewConn(evpp_socket_t sockfd,
                                const std::string& remote_addr/*ip:port*/,
                                const struct sockaddr_in* raddr)
           调用这个函数所处的线程 与 传入的 loop 进行 loop.Run 的线程要一样, 从 EventLoopThreadPool 事件处理线程池中选出一个
           io_loop(EventLoop*), 在创建 conn (TCPConnPtr), 对 conn 设置消息收到的回调函数 msg_fn_, 设置消息连接回调函数　
           conn_fn_(用于连接时, 断开时), 设置 conn 被断开后回调函数 TCPServer::RemoveConnection
           conn->SetCloseCallback(std::bind(&TCPServer::RemoveConnection, this, std::placeholders::_1)), 
           在 io_loop 中运行 TCPConn::OnAttachedToLoop(),  io_loop->RunInLoop(std::bind(&TCPConn::OnAttachedToLoop, conn));
    对外：
    1. TCPServer::TCPServer(EventLoop* loop, const std::string& laddr, const std::string& name, uint32_t thread_num)
            其中传入参数 loop 这个主要用于 listen event 的监听事件, laddr("ip:port") 监听的地址, name 这个是 TCPServer 的名字,
       thread_num 这个主要用于创建多个 EventLoopThread 用于处理已连接 conn 的事件(可读, 可写)的监听.
       
    2. bool TCPServer::Init()
            主要是创建 Listener 类指针, 并进行 Listener::Listen()
    3. void TCPServer::SetConnectionCallback(const ConnectionCallback& cb) 
            设置用户自定义连接时/断开时的回调函数
    4. void TCPServer::SetMessageCallback(MessageCallback cb)
            
    5. bool TCPServer::Start()
            对 tpool_(std::shared_ptr<EventLoopThreadPool>) 中的每一个 EventLoopThread 进行启动(
       EventLoopThread::Start(bool wait_thread_started, Functor pre, Functor post)), 并阻塞等待所有线程都运行起来
       这个  EventLoopThread::Start() 才返回, 设置 Listener 的有连接 socket 的回调函数(TCPServer::HandleNewConn)
       进行 Listener.Accept(), 即构造 liseten_fd 的 FdChannel, 设置 listen_fd 读事件的回调函数 Listener::HandleAccept(),
       将 FdChannel::AttachToLoop()函数 传到所属的 loop_ 的线程中运行
       
```

## Connector
```shell
    1. Connector 主要是用于连接 tcp_server 的操作, 本身包含自己创建的　chan_(FdChannel), 用于进行连接时的操作, 不用与连接后 socket
        数据的交互, 数据的交互是 TCPClient 中的 TCPClient.conn_(TCPConnPtr) 干的事情, 
        而且还包含连接时失败的定时器(TimerEventWatcher), 以及域名解析服务, 在连接之前进行域名转化为 ip, 成功连接后再调用传进来的
        用户自定义的回调函数．
      
    2. Connector::HandleError() : 
            这个只处理连接时的发送的错误,不处理已经连接后断开的异常.
        将 Connector::status_ 状态设置为 kDisconnected, 结束 连接时的 chan_(FdChannel), 结束 dns_resolver_(DNSResolver)的
        服务处理,  结束 timer_(TimerEventWatcher) 连接超时的任务, 调用用户自定义的断开连接的回调函数, 
        如果 TCPClient.auto_reconnect_ 设置了自动重连, 则再开始连接失败时,也进行重连
        
    3. Connector::Connect()
            创建一个非阻塞的 sock, 如果客户端需要有 bind 本地某个端口的需求, 则进行固定端口的绑定, 再进行 connect 系统函数的调用
       connect 成功返回后, 创建 chan_(FdChannel), 并使能可写事件, 并设置了可写事件回调函数 Connector::HandleWrite 再进行　
       event_ 事件的与 fd_, 绑定, event_base_set(), event_add() 加入到监听事件队列中
       
    4. Connector::HandleWrite()
            通过 getsockopt() 获取 已连接的 sock 是否有相关的错误信息, 如果有相关的错误信息,则调用　Connector::HandleError()
       通过 getsockname() 来获取已连接的 sock 对应本地的 localAddr, 将这个 fd_, localAddr 参数传入到用户自定义的回调函数中 conn_fn_
       取消连接超时定时器 (TimerEventWatcher.Cancel()), 结束 连接时的FdChannel
    
    对外:
    2. Connector::Connector(EventLoop* l, TCPClient* client)
            由 TCPClient 类进行调用, 其中 client 传入的是 TCPClient 自身 this, Connector 中 remote_addr_,
       timeout_(连接时的超时时间), raddr_　的内容也是由 client 中的数据提供.
       
    3. Connector::Start()
                创建一个连接超时的定时器, 当成功连上服务器, 则会将这个定时器取消掉, 否则会被触发, 如果纯 ip, 
       则进行 Connector::Connect() 操作.如果是 hostname 形式, 定义 DNS resovle 完回调函数 Connector::OnDNSResolved, 
       并注册到 DNSResolver, 进行 DNSResolver.Start() 内部当　DNSResolver　解析成功时, 会调用 Connector::OnDNSResolved,
        而 Connector::OnDNSResolved 则又会调用 Connector::Connect()
        
    使用
           std::shared_ptr<Connector> connector_;
           connector_.reset(new Connector(loop_, this));
           connector_->SetNewConnectionCallback(std::bind(&TCPClient::OnConnection, this, std::placeholders::_1, std::placeholders::_2));
           connector_->Start(); 
```

## TCPClient
```shell
    1.  TCPClient 包含 TCPClient.connector_(Connector) 只用与刚开始连接时的事件操作, 不参与连接断开处理,同时处理连接时错误的处理
        TCPClient.conn_(TCPConnPtr) 则保存已连接的 socket 负责后续的 socket 数据交互, 以及连接断开的操作处理.
    
    2. TCPClient::OnConnection(evpp_socket_t sockfd, const std::string& laddr)
            如果连接 server 失败(sockfd < 0), 则调用用户传入的连接时的回调函数回传给用户, 如果成功连接到 server, 则在已连接 sockfd 
       上创建 TCPConn, 设置 TCPConn.type_ 为 TCPConn::kOutgoing,设置消息收到回调函数 msg_fn_, 设置已连接的回调函数 conn_fn_, 
       设置连接断开关闭成功回调函数 TCPClient::OnRemoveConnection, 调用 TCPConn.OnAttachedToLoop(),  
       负责使能读事件 FdChannel.EnableReadEvent(), 并且调用连接时的回调函数 conn_fn_
        
    3. TCPClient::OnRemoveConnection(const TCPConnPtr& c)
            将 TCPClient.conn_(TCPConnPtr) 重置为 NULL, 如果支持自动重连,则进行 TCPClient::Reconnect()-> TCPClient::Connect()
        
    对外：
    2. TCPClient::TCPClient(EventLoop* loop, const std::string& raddr, const std::string& name) 
            传入外部的 loop, raddr(tcp server 地址 "ip:port")
            
    3. TCPClient::Bind(const std::string& addr)
            传入参数为 addr("ip:port"), 自身绑定固定的端口号, 到 TCPClient.local_addr_. 这个作用是在 TCPClient::Connect() ->
       Connector::Connect()中有 bind() 的业务    
     
    4. TCPClient::SetConnectionCallback(const ConnectionCallback& cb)   
       TCPClient::SetMessageCallback(const MessageCallback& cb) 
          
    5. TCPClient::Connect()
            创建 conn 连接时对象(Connector), 设置 conn 连接成功到服务器的回调函数 TCPClient::OnConnection, 
       开始 Connector::Start()
       
            
    
            
```

# benchmark

## throughput(吞吐量)
```shell
    1. 用 ping pong 协议来测试吞吐量, ping pong 协议是客户端和服务器都实现 echo 协议. 当 TCP 连接建立时, 客户端向服务器发送一些数据,
       服务器会 echo 回这些数据，然后客户端再 echo 回服务器。这些数据就会像乒乓球一样在客户端和服务器之间来回传送，直到有一方断开连接为止
       这是用来测试吞吐量的常用办法
    2. tcp_server 支持多线程的 EventLoopThread 处理,进行 echo ping-pong 
    3. tcp client 通过写一个 Client 类, 里面 session 代表有多少个已连接 conn, 同时依附于多少个 EventLoopThread 线程, 这些线程
    　　用来处理 client 数据 echo ping-pong. 还有一个 loop_ 进行超时机制调用 Client 类中的所有管理 session 的连接的断开,
       并统计收到的总的字节数, 得出吞吐量 
       得出的结论是：
          A: 
            一个包是 1 KB(1024 字节), 一个处理线程 EventLoopThread, 跑了 100 秒
                (1) 1 个 conn 连接数, 则吞吐量为 64 MB/s   ///////////////////////////////////
                (2) 10 个 conn 连接数或则是 100 conn 连接数, 则吞吐量为 180 MB/s
                (3) 1000 个 conn 连接数, 则吞吐量为 160 MB/s
                (4) 10000 个 conn 连接数, 则吞吐量为 100 MB/s
                
            
            一个包是 2 KB(2048 字节), 一个处理线程 EventLoopThread, 跑了 100 秒
                (1) 1 个 conn 连接数, 则吞吐量为 110 MB/s
                (2) 10 个 conn 连接数或则是 100 conn 连接数, 则吞吐量为 340 MB/s
                (3) 1000 个 conn 连接数, 则吞吐量为 260 MB/s
                (4) 10000 个 conn 连接数, 则吞吐量为 190 MB/s
                
            一个包是 4 KB(4096 字节), 一个处理线程 EventLoopThread, 跑了 100 秒
                (1) 1 个 conn 连接数, 则吞吐量为 220 MB/s
                (2) 10 个 conn 连接数或则是 100 conn 连接数, 则吞吐量为 630 MB/s
                (3) 1000 个 conn 连接数, 则吞吐量为 490 MB/s
                (4) 10000 个 conn 连接数, 则吞吐量为 260 MB/s
                
            一个包是 8 KB(8192 字节), 一个处理线程 EventLoopThread, 跑了 100 秒
                (1) 1 个 conn 连接数, 则吞吐量为 390 MB/s
                (2) 10 个 conn 连接数或则是 100 conn 连接数, 则吞吐量为 1000 MB/s
                (3) 1000 个 conn 连接数, 则吞吐量为 780 MB/s
                (4) 10000 个 conn 连接数, 则吞吐量为 640 MB/s
                
            一个包是 16 KB(16384 字节), 一个处理线程 EventLoopThread, 跑了 100 秒
                (1) 1 个 conn 连接数, 则吞吐量为 630 MB/s
                (2) 10 个 conn 连接数或则是 100 conn 连接数, 则吞吐量为 1590 MB/s
                (3) 1000 个 conn 连接数, 则吞吐量为 1100 MB/s
                (4) 10000 个 conn 连接数, 则吞吐量为 940 MB/s
                
            一个包是 80 KB(81920 字节), 一个处理线程 EventLoopThread, 跑了 100 秒
                (1) 1 个 conn 连接数, 则吞吐量为 1500 MB/s
                (2) 10 个 conn 连接数, 则吞吐量为 2000 MB/s
                (3) 100 conn 连接数, 则吞吐量为 1400 MB/s
                (3) 1000 个 conn 连接数, 则吞吐量为 1300 MB/s
                (4) 10000 个 conn 连接数, 则吞吐量为 1200 MB/s
                
            如果是一个处理线程 EventLoopThread, 10 个客户端(连接数), 效果最佳
            
          B:
            一个包是 4 KB(4096 字节), 跑了 60 秒
                (1) 100 个 conn 连接数, 1 个处理线程 EventLoopThread, 则吞吐量为 520 MB/s
                (2) 100 个 conn 连接数, 2 个处理线程 EventLoopThread, 则吞吐量为 880 MB/s
                (3) 100 个 conn 连接数, 4 个处理线程 EventLoopThread, 则吞吐量为 1500 MB/s
                (4) 100 个 conn 连接数, 6 个处理线程 EventLoopThread, 则吞吐量为 2200 MB/s  ////////////////////////////////
                (5) 100 个 conn 连接数, 8 个处理线程 EventLoopThread, 则吞吐量为 2000 MB/s
                
                (6)  1000 个 conn 连接数, 1 个处理线程 EventLoopThread, 则吞吐量为 380 MB/s
                (7)  1000 个 conn 连接数, 2 个处理线程 EventLoopThread, 则吞吐量为 690 MB/s
                (8)  1000 个 conn 连接数, 4 个处理线程 EventLoopThread, 则吞吐量为 1200 MB/s
                (9)  1000 个 conn 连接数, 6 个处理线程 EventLoopThread, 则吞吐量为 1900 MB/s
                (10) 1000 个 conn 连接数, 8 个处理线程 EventLoopThread, 则吞吐量为 1900 MB/s
                
            
            一个包是 16 KB(16384 字节), 跑了 100 秒
                (1) 100 个 conn 连接数, 1 个处理线程 EventLoopThread, 则吞吐量为 2200 MB/s
                (2) 100 个 conn 连接数, 2 个处理线程 EventLoopThread, 则吞吐量为 2800 MB/s
                (3) 100 个 conn 连接数, 4 个处理线程 EventLoopThread, 则吞吐量为 3800 MB/s
                (4) 100 个 conn 连接数, 6 个处理线程 EventLoopThread, 则吞吐量为 4800 MB/s
                (5) 100 个 conn 连接数, 8 个处理线程 EventLoopThread, 则吞吐量为 5600 MB/s
                
                (6)  1000 个 conn 连接数, 1 个处理线程 EventLoopThread, 则吞吐量为 1400 MB/s
                (7)  1000 个 conn 连接数, 2 个处理线程 EventLoopThread, 则吞吐量为 2000 MB/s
                (8)  1000 个 conn 连接数, 4 个处理线程 EventLoopThread, 则吞吐量为 2400 MB/s
                (9)  1000 个 conn 连接数, 6 个处理线程 EventLoopThread, 则吞吐量为 2700 MB/s
                (10) 1000 个 conn 连接数, 8 个处理线程 EventLoopThread, 则吞吐量为 3000 MB/s
```
