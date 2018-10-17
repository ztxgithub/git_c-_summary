# muduo 网络库

## 简要

```shell
       1. 编写网络库的目的
              主要 C++/java 等高级语义的 socket api 没有提供更高级的封装，编写网络程序很容易出现问题，为了
              提高开发的效率以及开发的难度，以及更加方便处理并发连接．
              
       2. 线程模型
            采用 one loop per thread + thread pool 模型，每个线程最多有一个 EventLoop, 每个
            TcpConnection 必须由某个 EventLoop 管理，每个文件描述符只能由一个线程进行读写
            
       3. 基于事件的非阻塞网络编程
                由原来的 "主动调用 recv() 接受数据，　主动调用 accept() 接受新的连接，　主动调用 send()
                发送数据"　改变为　"注册一个收数据的回调，网络库收到数据就会调用我，直接把数据提供给我，
                供我消费．　注册一个接受连接的回调，网络库接受到新的连接会回调我，直接把新的连接对象传给
                我，供我使用．　需要发送数据时，只管往连接中写数据，网络库负责无阻塞得发送"
                
       4. TCP 网络编程相关的问题：　Linux 多线程服务端编程(P137)
       5.TCP 网络编程(三个半事件)
            1. 　连接的建立．　包括服务端的接受(accept) 新连接和客户端发起连接(connect)
            2. 　连接的断开．　包括主动连接(close,shutdown)和被动连接(read的字节数为 0 )
            3. 　消息的接受．　接受的处理(阻塞还是非阻塞，如何处理分包，应用层的缓冲设计)
            3.5  消息的发送．　对于低流量的服务，可以不必关心这个事件，这里发送完毕只是将数据从
            　　　　　　　　　　用户拷贝到系统的发送缓冲区，由 TCP 协议 自主得将数据发送出去，
            　　　　　　　　　　发送完毕不代表对端接受成功
            
       6. TCP half-close
            通过shutdownWrite()先关闭 "写"　方向的连接, 等对方关闭之后，在关闭本地的 "读" 方向的连接.
            而不能直接使用 close() 关闭读写连接
            
       7. TCP 分包
            在发送一个消息或一帧数据时，通过一定的处理，让接收方能从字节流中识别并截取出一个个消息．
            (1) 对于短连接(通常就发一条消息)，发送方发送完一条消息后主动关闭连接，则接收方 read() 
            　　直到为 0, 说明消息接受完毕
            (2) 长连接的 TCP 分包方法:
                    A. 消息长度固定
                    B. 使用特殊的字符或则字符串作为消息的边界
                    C. 每一条消息的头部加一个长度字段
                    D. 利用消息本身的格式来分包，例如 XML 格式的消息中 <root>  </root>　的配对，以及 JSON 格式中
                    　　{...} 的配对，解析这种消息格式通常会用到状态机
                    
       8. 对于网络库中，可以用 codec 来分离 "消息接受" 和 "消息处理"，如果网络库只提供相当于 char buf[1024], 或则
       　　不提供消息缓冲区，仅仅通知程序 "socket 可读/可写", 要程序自己管理 IO buf, 这样网络库用起来就很不方便
            

```

## 客户端工具

```shell
    1.netcat
        A. 实现网络客户端打印服务器响应的内容
            > nc ip port
```

## 性能测试

```shell
    1.吞吐量测试
        可以根据 ping pong 协议来测试 muduo(网络库) 在单机上的吞吐量.
        ping pong 协议是当客户端与服务端建立连接时，客户端向服务端发送一些数据，服务端则 echo 这些数据(服务端不进行
        数据的解析，只是远远本本得返回给客户端)，客户端再 echo 给服务器，这些数据像乒乓球一样来回在客户端和服务端
        传输，直到连接中断

```

## 值得一读

```shell
    1. example/asio/chat/server_thread.cc
       example/asio/chat/server_thread_efficient.cc 借 shared_ptr 实现 copy-on-write 的手法减低锁竞争
       example/asio/chat/server_thread_highperformance.cc 采用 thread local　变量，实现多线程高效转发．

```

## 网络库的 IO Buffer

```shell
    1.Buffer 功能需求
        (1) 对外表现为一块连续的内存(char *p, int len),以方便客户编写
        (2) 其 size() 可以自动增长，适应不同大小的消息，而不是 char buf[1024]
        (3) 内部以 std::vector<char> 来保存数据，并提供相应的
        
    2. 如何设计每个连接的接受缓冲区
            (1) 一方面要考虑到减少系统调用(read()),一次从 TCP socket 读的数据越多越好(这意味着连接的接受缓冲区越大越好)
            　　另一方面连接的接受缓冲区不能太大，这样如果服务端的连接数大(例如10000),则消耗内存过多
            
            (2) 解决方案(readv() 结合栈上空间)
                    在栈上准备 10000 byte 的 extrabuf, 然后利用 readv() 读取数据， iovec 有两块，第一块指向了
                    muduo buf 的 writable(针对 muduo buf 而言) 字节，第二块则指向 extrabuf,如果读入的数据不多，
                    则全部读到 muduo buf 中；如果读入数据太大，则先存满 muduo buf,再接着存 extrabuf, 然后程序再把
                    extrabuf 里的数据 append 到 muduo buf
                    
```

## codec (编解码器)

```shell
    1. 编解码器(codec)是 encoder 和　decoder 的缩写，它是一层中间件，位于 TcpConnection 和 ChatServer 之间，
       它将识别完整的数据消息，将 muduo:net:Buffer 转化为 string, 再将 string 给 ChatServer 处理函数进行处理．
       发送过程也是一样的， ChatServer 通过 LengthHeaderCodec::send()来发送 string, LengthHeaderCodec 将它
       编码为 Buffer
       
    2. ProtobufCodec 和 ProtobufDispatcher 主要用于 Protobuf Message 的消息的编解码, 先将消息进行解码,再根据
       对应的消息类型进行消息的分发处理. 新的 Protobuf Message 类型只需要实现对应的业务逻辑就行
```

## 限制服务器的最大并发连接数

```shell
    1.限制服务器的最大并发连接数的原因
        (1) 防止服务器超载
        (2) 防止 file descriptor 用完
                file descriptor 用完可能导致 CPU 使用率为 100%, 当使用 Reactor 模式进行非阻塞的 accept 连接时,
                当 file descriptor 用完时, accept() 返回 EMFILE, 本进程的文件描述符已经达到上限, 无法为新连接
                创建 socket 文件描述符, 这个时候继续调用 epoll_wait(), 会立马返回,因为新的连接等待可读, listen socket
                还是可读,这时就会陷入 busy loop 
                
            解决方案:
                第一种方案:
                    占用一个空闲的文件描述符,当遇到 file descriptor 用完的情况,先关闭该空闲的文件描述符,提供一个空闲的
                    名额,再 accept() 后拿到这个 socket 文件描述符, 随后立刻 close() 掉, 这样就优雅的断开了客户端的连接
                    最后重新打开一个空闲文件,把"坑"占用,以备再次出现这种情况
                    
                第二种方案:
                    将 soft limit 与 hard limit 相比设置低一点,这样如果超过 soft limit 就主动关闭连接,
                    避免出现 file descriptor 耗尽(使用超过 hard limit)
                    
                第三种方案:
                    可以在程序中定义一个保存当前活动连接数的变量,当它超过设置的最大连接数时,在 onConnect 时就立刻 close 掉
```

## 定时器

```shell
    1.计时(获取当前时间)
        只使用 gettimeofday() 来获取当前时间,原因如下:
            A. 在 x86-64 平台上, gettimeofday() 不是系统调用,是在用户太实现的,减少了上下文切换和内核开销
            
    2.定时
        只使用 timerfd_* 系列函数(timerfd_create, timerfd_gettime, timerfd_settime), 原因如下:
            A. timerfd_create() 把时间变成一个文件描述符,这样很方便融入到 epoll/select 框架中,
               用同一的方式处理 IO 事件和超时事件
```

## 用 timing wheel 踢掉空闲连接

```shell
    1.采用 share_ptr 和 weak_ptr 方式进行踢掉空闲连接, 首先当连接建立时将该连接作为强引用加入到循环队列尾部的 bucket,
      同时将(WeakEntryPtr weakEntry(entry); conn->setContext(weakEntry)) 以便消息到达时使用
      当该连接收到消息时, 取出对应的值转为强引用,在加入到循环队列尾部的 bucket.
      当超时事件产生时, 向循环队列尾部的插入空的 bucket, 这样循环队列头就会有 bucket 弹出,这样就会进行析构函数,
      如果强引用计数为 0, 则调用该析构函数
      
      muduo/examples/idleconnection/echo.cc

```

# muduo 网络库设计及实现

## EventLoop

```shell
    1. one loop per thread 代表处理网络 IO 线程中只能有一个 EventLoop 对象,因此在 EventLoop 构造函数时
       会检查当前线程是否已经创建了其他的 EventLoop 对象.EventLoop 的构造函数会记住本对象所属的线程(threadId_)
       创建 EventLoop 对象的线程是 IO 线程,主要用于运行事件循环 EventLoop::loop(). 同时 EventLoop 对象可能会在
       其他线程中使用(不一定是网络 IO 线程),EventLoop 对象中的有些成员函数是线程安全的(可以跨线程调用), 但是有些成员函数
       (例如 EventLoop::loop()) 只能在网络 IO 线程中调用,同时 EventLoop 对象的创建也只能在网络 IO 线程创建,
       所以 EventLoop 提供了 isInLoopThread() 的成员函数来判断能不能在该线程中调用 EventLoop::loop().
       
       EventLoop 包含　Poll class，　它调用 Poller::loop() 获取当前活动的 channel 列表，　然后依次调用
       活动的 channel 列表中的 Channel::handleEvent() 函数
       
```

## Reactor 关键结构

```shell
    1. Reactor 最核心是事件分发机制,即将 IO multiplexing 拿到的 IO 事件分发给各自文件描述符的事件处理函数
    2. channel class
            每一个 channel 对象只属于一个 EventLoop 对象(即只属于某一个 IO 线程), 每一个 channel 对象只负责
            一个文件描述符(一个 socket 连接或则 timerfd )的 IO 事件分发,但它并不拥有这个 fd,  channel 对象析构的时候也不会
            close(fd), channel 对象会把这个 fd 发生的不同的 IO 事件分发到不同的回调函数,例如 ReadCallback,
            WriteCallback 等. muduo 用于一般不直接使用 channel 对象,而会使用更上层的封装,例如 TcpConnection,
            该 TcpConnection 对象有 channel 的成员变量, 它由 EventLoop:loop() 调用,
            它的功能是根据 revents_(事件发生类型) 调用不同的回调函数.
            
    3. Poll class 
        Poll class 是 IO multiplexing 的封装, Poll class 中 包含要关心的事件集合
        vector<struct pollfd> pollfds_ , Poller::poll()函数的发生事件 revents 保存到对应的 channel class 中,
        供 channel::handleEvent() 使用,但 Poller 并不拥有 Channel , Channel 在析构之前自己 
        unregister (EventLoop::removeChannel()), 避免空悬指针, Poll 值负责 IO multiplexing ,不负责事件的分发.
        Poller::updateChannel()的功能是维护和更新 pollfds_.
        
    3. TimeQueue class
        实现定时器功能，
```
