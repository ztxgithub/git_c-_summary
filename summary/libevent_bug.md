# libevent 2.0 版本
```shell
    1. event-config.h 文件定义了一些宏定义 例如 #define EVENT__SIZEOF_LONG_LONG 8, libevent 源码中有些代码需要这些宏定义,
       event-config.h 文件是通过　autoconf 检查所在系统的情况, 编译器版本特性, 以及 ./configure 参数, 都会将内容体现在 
       event-config.h 中
       
    2. libevent 错误日志
            用户调用 event_set_log_callback() 进行用户日志回调函数的注册, 这个用户日志回调函数里面把信息打印到文件中.
                typedef void (*event_log_cb)(int severity, const *msg);
                void event_set_log_callback(event_log_cb cb);
                
            注意用户日志回调函数中不能调用任何 libevent 提供的 api 函数
    3. 内存分配
            (1) 用户可以通过  event_set_mem_functions(void *(*malloc_fn)(size_t sz),
                                                    void *(*realloc_fn)(void *ptr, size_t sz),
                                                    void (*free_fn)(void *ptr))
                来注册用户自定义的内存分配函数, 虽然这个函数不做任何的检查, 但还是有一点要注意。这个三个指针, 
                要么全设为 NULL (恢复默认状态), 要么全部都非 NULL
    4. libevent 多线程
            (1) libevent 调用 evthread_use_pthreads(), libevent 里面的函数就会变成线程安全, 这个时候主线程使用 
                event_base_dispatch, 别的线程是可以安全得调用 event_add() 函数将 event 事件加入到 event_base 中
            (2) 注意: evthread_use_pthreads() 这个函数必须在 event_base_new() 函数之前调用
            (3) Libevent 提供的 pthreads 版本锁只支持递归锁和普通非递归锁，并不支持读写锁
            (4) evthread_use_pthreads() 函数内部进行 evthread_set_lock_callbacks() 和 evthread_set_condition_callbacks()
                线程锁的设置以及条件变量的设置.
            (5) 如果要进行内存分配, 日志记录, 线程锁的自定义回调函数设置, 则设置的顺序为 内存分配 -> 日志记录 -> 线程锁
            (6) 多线程锁的内部实现
                        int　event_add(struct event *ev, const struct timeval *tv)
                        {
                            //加锁
                            EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);
                             
                            res = event_add_internal(ev, tv, 0);
                            
                            //解锁
                            EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

                        }
                        
    5. Libevent 都是使用条件编译+宏定义的方式, 这样用相同的宏定义达到兼容不同系统.
            (1) 有符号类型 size_t
            (2) 指针类型 对应的 intptr_t
                        
```

## TAILQ_QUEUE
```shell
    1. 
        
        #define TAILQ_HEAD(name, type)                        \
        struct name {                                \
            struct type *tqh_first;                \
            struct type **tqh_last;            \
            
        }
        
        
        #define TAILQ_ENTRY(type)                        \
        struct {                                \
            struct type *tqe_next;                \
            struct type **tqe_prev;        \
        }

```

## 哈希结构体
```shell
    1. 
        struct event_list
        {
            struct event *tqh_first;
            struct event **tqh_last;
        };
        
        struct evmap_io {
            //TAILQ_HEAD (event_list, event);
            struct event_list events;
            ev_uint16_t nread;
            ev_uint16_t nwrite;
        };
         
         // hash 表中的元素, 包含着关键的 key -> fd
        struct event_map_entry {
            HT_ENTRY(event_map_entry) map_node; //next指针
            evutil_socket_t fd;
            union {
                struct evmap_io evmap_io;
            } ent;
        };
         
        struct event_io_map
        {
            // 哈希表, 是一个数组, 保存了  struct event_map_entry * 元素的数组(个数是 event_io_map.hth_table_length), 
            // 如果有冲突, 则  struct event_map_entry *中 event_map_entry.map_node 有值
            struct event_map_entry **hth_table;
            
            //哈希表的长度, 代表有 hash 值后的数量, 当然是越大越好, 代表 hash 值越均衡, 冲突越小, hth_table_length 越接近 hth_n_entries 越好.
            unsigned hth_table_length;
            
            //哈希的元素个数
            unsigned hth_n_entries;
            
            //resize 之前可以存多少个元素
            //在event_io_map_HT_GROW函数中可以看到其值为hth_table_length的
            //一半。但 hth_n_entries > =hth_load_limit时，就会发生增长哈希表的长度
            unsigned hth_load_limit;
            
            //后面素数表中的下标值。主要是指明用到了哪个素数,　在 event_io_map_HT_GROW 扩容有用.
            int hth_prime_idx;
        }
        
        该哈希表是使用链地址法解决冲突问题的, 至于 hash 函数采用的是取 % 方法, 
        使用的是 hashsocket(event_map_entry.fd) % event_io_map.hth_table_length. 在 event_map_entry 中同一个 fd 可以对应多个 event,
        因为 libevent 允许对同一个 fd 多次调用 event_new, event_add, 那么哈希表元素中就会有 TAILQ_QUEUE 队列.
        
    2. 在 libevent 中只有 windows 系统才使用 event_io_map 哈希表, 因为在 windows 中 fd 文件描述符是一个很大的整数值(有点像指针的值), 
       不适合放到 event_signal_map 结构中, 需要通过哈希函数转化为较小的值，存放到哈希表中. 而 linux 系统下, fd(文件描述符) 是
       从 0 开始的, 不会太大, 适合放到 event_signal_map 中.
       
    3. 哈希函数, 在进行 % event_io_map.hth_table_length 之前, 先对 fd(文件描述符)进行处理, 
      通过旋转加法(h += (h >> 2) | (h << 30)) 先拿到冲突比较小的 hash
      
    4. HT_ENTRY 宏
            
        //ht-internal.h文件
        #ifdef HT_CACHE_HASH_VALUES
        #define HT_ENTRY(type)                          \
          struct {                                      \
            struct type *hte_next;                      \
            unsigned hte_hash;                          \
          }
        #else
        #define HT_ENTRY(type)                          \
          struct {                                      \
            struct type *hte_next;                      \
          }
        #endif    
        
        其中如果定义了 HT_CACHE_HASH_VALUES 则　hte_hash　用来保存 hashsocket 的返回值, 只需要第一次计算, 后续 fd 都不会改变, 
        那么　hte_hash　也是不变的.
        
    5. 哈希表操作函数
            (1) event_io_map_HT_INSERT(struct event_io_map *head,
                       struct event_map_entry *elm)
                将 elm 查到对应链表的头部.
            (2) static inline void
                event_io_map_HT_FOREACH_FN(struct event_io_map *head,
                                           int (*fn)(struct event_map_entry *, void *),
                                           void *data);
                                           
                这里面遍历所有的哈希元素, 并在调用回调函数, 如果回调函数失败, 则将这个哈希元素从哈希表中删除, 但是这个函数没有对
                元素进行删除, 因为底层逻辑是用二维数据, 如果是未冲突的元素不存在内存泄露, 以后进行哈希扩展时, 会释放掉. 如果是已
                冲突的元素, 则可能会存在内存泄露.
                
            (3) 最佳的状态是所有的元素都不冲突, 
                定义了一个素数表, 用于扩展元素为 struct event_map_entry* 的数组  event_io_map.hth_table, 扩展的个数为素数表
                元素, 每个元素差不多差 2 倍递增. 扩展的条件是
            　　 event_io_map.hth_n_entries >= event_io_map.hth_load_limit, 而 event_io_map.hth_load_limit 值为 是
            　　新扩展数(event_io_map.hth_table_length)的一半. 在进行重新了哈希扩容后, 需要对原来的元素进行哈希化.
        
```

## event_signal_map
```shell
    1. 
        struct event_list
        {
            struct event *tqh_first;
            struct event **tqh_last;
        };
         
        struct evmap_signal {
            struct event_list events;
        };
         
        struct event_signal_map {
           
            void **entries; //二级指针，evmap_signal*数组
            int nentries; //元素个数
        };
        
        进行 event_signal_map 重新分配
        static int  evmap_make_space(struct event_signal_map *map, int slot, int msize)
        其中 slot 既代表信号值 sig(最多 32 中), 也可以代表文件描述符 fd,但是这 2 中不会混杂在一起,
        因为是 linux 环境下, fd 从 0 开始, 应该会很小.
        event_signal_map.entries 代表 evmap_signal*数组, 一个 fd 对应一个 evmap_signal*
        
        注意在 linux 环境下 event_io_map 等价与 event_signal_map
        
```

## event_base
```shell
    1. 在 libevent 中 backend 代表多路 IO 复用函数, 如 , poll, epoll 等函数.
    2. struct event_base 的创建, 可以使用默认的配置参数　struct event_config
            struct event_base *　event_base_new(void)
    3. 
        struct event_config_entry {
            TAILQ_ENTRY(event_config_entry) next;
              const *avoid_method;  // 禁用某种多路 IO 复用函数, 对应与 const struct eventop 的 eventop.name
        };
        
        enum event_method_feature {
            //支持边沿触发
            EV_FEATURE_ET = 0x01,
            //添加、删除、或者确定哪个事件激活这些动作的时间复杂度都为O(1)
            //、poll不满足这个特征. epoll则满足
            EV_FEATURE_O1 = 0x02,
            //支持任意的文件描述符，而不能仅仅支持套接字
            EV_FEATURE_FDS = 0x04
        };
        
        struct event_config {
            TAILQ_HEAD(event_configq, event_config_entry) entries;  // 禁用哪些 IO 多路复用函数
         
            int n_cpus_hint;  // 指明 CPU 的数量
            enum event_method_feature require_features; // 规定 IO 多路复用的函数特征
            enum event_base_config_flag flags;
        };
        
        (1) 禁用某个 IO 多路复用函数, 通过调用 int event_config_avoid_method(struct event_config *cfg, const *method)
        (2) 设置　规定 IO 多路复用的函数特征(event_config.require_features)
                int　event_config_require_features(struct event_config *cfg,　int features), 如果要设置多个特征, 
                不能调用该函数多次，而应该使用位操作。比如: EV_FEATURE_O1 | EV_FEATURE_FDS 作为参数.
                
                注意, 如果设置了 IO 多路复用的函数特征, 进行 event_base_new_with_config() 创建可能返回的是 NULL, 因为系统
                无法创建出满足这个　IO 多路复用的函数特征的 event_base
                
                            event_config *cfg = event_config_new();
                            event_config_require_features(cfg,  EV_FEATURE_O1 | EV_FEATURE_FDS);
                         
                            event_base *base = event_base_new_with_config(cfg);
                            if( base == NULL )
                            {
                                 base = event_base_new(); //使用默认的。
                            }
                            
        (3) enum event_base_config_flag
                a. EVENT_BASE_FLAG_NOLOCK：不要为 event_base 分配锁。设置这个选项可以为 event_base 节省一点加锁和解锁的时间,
                                           但是当多个线程访问 event_base 会变得不安全
                b. EVENT_BASE_FLAG_IGNORE_ENV：选择多路 IO 复用函数时，不检测 EVENT_* 环境变量. 使用这个标志要考虑清楚：
                                               因为这会使得用户更难调试程序与 Libevent 之间的交互
                c. EVENT_BASE_FLAG_NO_CACHE_TIME：在执行 event_base_loop 的时候没有缓存(cache) loop 时间.该函数的 while 循环会经常取
                                                 系统时间，如果缓存(cache)时间，那么就取缓存的时间的。否则只能通过系统提供的函数来
                                                 获取系统时间很耗时        
                d. EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST：告知 Libevent, 在使用 epoll 多路 IO 复用函数基础上, 可以安全地使用
                                                        更快的基于 changelist 的多路 IO 复用函数：epoll-changelist 多路 IO 
                                                        复用可以在多路 IO 复用函数调用之间，同样的 fd 多次修改其状态的情况下，
                                                        避免不必要的系统调用。但是如果传递任何使用 dup() 或者其变体克隆的 fd 给
                                                        Libevent，epoll-changelist多路IO复用函数会触发一个内核 bug，
                                                        导致不正确的结果。在不使用 epoll 这个多路 IO 复用函数的情况下，
                                                        这个标志是没有效果的。也可以通过设置 EVENT_EPOLL_USE_CHANGELIST 环境
                                                        变量来打开 epoll-changelist 选项
                e. EVENT_BASE_FLAG_STARTUP_IOCP：仅用于 Windows。这使得 Libevent 在启动时就启用任何必需的 IOCP 分发逻辑,
                                                 而不是按需启用。如果设置了这个宏，那么 evconn_listener_new 和 
                                                 bufferevent_socket_new 函数的内部将使用 IOCP
                                                 
        (4) 特征设置 event_config_require_features() 和 CPU 数目设置 event_config_set_num_cpus_hint ()函数调用会覆盖之前的设置
            如果要同时设置多个，那么需要在参数中使用位运算中的 | 。而另外两个变量的设置可以通过多次调用函数的方式同时设置多个值
            
    4. 获取当前的配置 event_base_config
            (1) const **event_get_supported_methods(void); 获取当前系统所支持的多路IO复用函数有哪些(字符串数组)
            (2) const *event_base_get_method(const struct event_base *); 
                    对于 event_base 这个使用的多路 IO 复用函数是哪个.
            (3) int event_base_get_features(const struct event_base *base); 获取参数 event_base 当前所采用的特征是什么
            (4) 
                static int  event_config_is_avoided_method(const struct event_config *cfg, const *method)
                传入参数 cfg 和 method, 返回值为非 0 代表这个 method 多路复用函数被禁用了, 0: 启用
```

## 跨平台的 Reactor 接口的实现
```shell
    1. 
        struct eventop {
        
            const *name; //多路IO复用函数的名字
         
            void *(*init)(struct event_base *);
         
            int (*add)(struct event_base *, evutil_socket_t fd, short old, short events, void *fdinfo);
            int (*del)(struct event_base *, evutil_socket_t fd, short old, short events, void *fdinfo);
            int (*dispatch)(struct event_base *, struct timeval *);
            void (*dealloc)(struct event_base *);
         
            int need_reinit; //是否要重新初始化
            //多路IO复用的特征
            enum event_method_feature features;
            size_t fdinfo_len; //额外信息的长度。有些多路IO复用函数需要额外的信息
        }
        
        
        struct event_base {
        
            const struct eventop *evsel;
            void *evbase;   // 这个结构体主要个用来保存 IO 多路复用函数需要的参数, 例如 监听的文件描述符fd，监听的事件等等
         
            …
        };
        
    2. libevent 如何选择 IO 多路复用函数
            通过文件中宏定义
            #ifdef _EVENT_HAVE_SELECT
            extern const struct eventop selectops;
            #endif
            #ifdef _EVENT_HAVE_POLL
            extern const struct eventop pollops;
            #endif
            #ifdef _EVENT_HAVE_EPOLL
            extern const struct eventop epollops;
            #endif
            #endif
             
           
            static const struct eventop *eventops[] = {
            #ifdef _EVENT_HAVE_WORKING_KQUEUE
                &kqops,
            #endif
            #ifdef _EVENT_HAVE_EPOLL
                &epollops,
            #endif
            #ifdef _EVENT_HAVE_POLL
                &pollops,
            #endif
            #ifdef _EVENT_HAVE_SELECT
                &selectops,
            #endif
                NULL
            };
            
            根据宏定义判断当前的操作系统环境是否支持多路 IO 复用函数,同时根据宏定义将对应的指针放到全局数组中, 根据这个数组的第一个
            满足的元素赋值(如果设置了 event_config, 需要根据 event_config 是否有指定的禁用的 method, 和　
            是否满足 event_config所描述的特征)给 event_base.evsel. 因为是条件宏, 所以在编译器编译代码之前就完成宏的替换
            
    3. event_base.evbase 存放 IO 复用需要的结构体
            例如:
            
                struct epollop {
                    struct epoll_event *events;
                    int nevents;
                    int epfd;
                };
                
                //.c文件
                struct selectop {
                    int event_fds;        
                    int event_fdsz;
                    int resize_out_sets;
                    fd_set *event_readset_in;
                    fd_set *event_writeset_in;
                    fd_set *event_readset_out;
                    fd_set *event_writeset_out;
                };
                 
                //poll.c文件
                struct pollop {
                    int event_count;        
                    int nfds;            
                    int realloc_copy;        
                    struct pollfd *event_set;
                    struct pollfd *event_set_copy;
                };
                
    4.  由于有 evsel 和 evbase 这个两个指针变量，当初始化完成之后，再也不用担心具体使用的多路 IO 复用函数是哪个. evsel 结构体的
        函数指针提供了统一的接口, 上层的代码要使用到多路 IO 复用函数的一些操作函数时，直接调用 evsel 结构体提供的函数指针即可。
        Libevent 实现了统一的跨平台 Reactor 接口。
```

## event 操作
```shell
    1. event 的状态
            a. 用 event_new 创建的 even 都是处于已初始化状态(ev->ev_flags: EVLIST_INIT)
            b. 调用 event_add(struct event *ev, const struct timeval *timeout) 后 event.ev_flags | EVLIST_INSERTED(已注册),
               如果第二个参数有值, 则 event.ev_flags | EVLIST_TIMEOUT
            c. 在进行 event_base_dispatch() 过程中, 进行 evsel->dispatch(base, tv_p) 函数等待事件触发(不管是 IO 事件还是超时事件)
             　事件触发后(epoll_wait() 返回成功), 再调用 event_active_nolock(), 将该 event 加入到激活队列中, 
              event.ev_flags | EVLIST_ACTIVE
              
    2.  event_pending(const struct event *ev, short event, struct timeval *tv)
            可以直到 event 是否已经被注册到 event_base 中, 如果返回 0 则代表该 event 已经从 event_base 中删除.
            
    3. 手动激活 event(void event_active(struct event *ev, int res, short ncalls))
            通过调用 event_active(struct event *ev, int res, short ncalls), 手动将这个 event 激活加到激活队列中, 而不管
            这个 event 有没有被触发.如果执行激活动作的线程不是主线程，那么还要唤醒主线程，让主线程及时处理激活 event,
            不再睡眠在多路 IO 复用函数中. 由于手动激活一个 event 是直接把这个 event 插入到激活队列的，所以 event 的被激活原因
            (event.ev_res)可以不是该 event 监听的事件(是实际主动调用的 event_active() 函数传过来的事件类型参数), 比如说该 event
            只监听 EV_READ 事件, 那么可以调用 event_active(ev,EV_SIGNAL, 1); 用信号事件激活该 event, 那么 event.res 是 EV_SIGNAL
            
    5. 删除 event(void event_free(struct event *ev))
           
            
            
```

## Libevent 工作流程
```shell
    1. 
        struct event {
        
            // 激活队列
            TAILQ_ENTRY(event) ev_active_next; 
            
            // 注册事件队列
            TAILQ_ENTRY(event) ev_next;      
            
            // for managing timeouts 
            // 仅用于定时事件处理器 (event).EV_TIMEOUT 类型
            union {
            
                TAILQ_ENTRY(event) ev_next_with_common_timeout;
                
                // 指明该 event 结构体在堆的位置
                int min_heap_idx; 
            } ev_timeout_pos;    
            
            // 对于 I/O 事件，是文件描述符；对于 signal 事件，是信号值
            evutil_socket_t ev_fd;
         
            //所属的 event_base
            struct event_base *ev_base; 
         
            // 因为信号和 I/O 是不能同时设置的。所以可以使用共用体以省内存在低版本的 Libevent，两者是分开的，不在共用体内
            // 无论是信号还是 IO，都有一个 TAILQ_ENTRY 的队列。它用于这样的情景: 用户对同一个 fd 调用 event_new 多次，
            // 并且都使用了不同的回调函数。 每次调用 event_new 都会产生一个 event*。这个 xxx_next 成员就是把这些 event 连接起来
            union {
                
                //用于IO事件
                struct {
                    TAILQ_ENTRY(event) ev_io_next;
                    
                    //指定事件的超时值(不是绝对时间)
                    struct timeval ev_timeout; 
                } ev_io;
         
                
                //用于信号事件
                struct {
                    TAILQ_ENTRY(event) ev_signal_next;
                    
                    //事件就绪执行时，调用ev_callback的次数
                    short ev_ncalls; 
                    
                    //指针，指向次数
                    short *ev_pncalls; 
                } ev_signal;
            } _ev;
            
         
            //记录监听的事件类型 EV_READ EVTIMEOUT之类
            short ev_events;
            
            // 记录了被激活的原因, 例如 EV_READ EV_TIMEOUT
            short ev_res;        
            
            //libevent 用于标记 event 信息的字段，表明其当前的状态.
            //可能值为前面的 EVLIST_XXX, 也可能是 event_config.flags 的值
            
           // #define EVLIST_TIMEOUT    0x01    // event 从属于定时器队列或者时间堆
           // #define EVLIST_INSERTED    0x02    // event 从属于注册队列
           // #define EVLIST_SIGNAL    0x04        // 没有使用
           // #define EVLIST_ACTIVE    0x08        // event 从属于活动队列
           // #define EVLIST_INTERNAL    0x10    // 该 event 是内部使用的。信号处理时有用到
           // #define EVLIST_INIT    0x80        // event 已经被初始化了
            
            short ev_flags; 
         
            //本 event 的优先级。调用 event_priority_set 设置
            ev_uint8_t ev_pri;
            
            // 这个值在 event_assign() 函数被赋值, 
            // 如果 event.events 包含 EV_SIGNAL, 则 event.ev_closure 被赋值为 EV_CLOSURE_SIGNAL
            // 如果 event.events 包含 EV_PERSIST, 则 event.ev_closure 被赋值为 EV_CLOSURE_PERSIST, 其他的情况都为 EV_CLOSURE_NONE
            // 这个值实际运用是在　event_process_active_single_queue() 函数中
            ev_uint8_t ev_closure;
            struct timeval ev_timeout;  // 记录从 1970 到现在的时间
         
            // 回调函数
            void (*ev_callback)(evutil_socket_t, short, void *arg); 
            // 回调函数的参数
            void *ev_arg; 
        };
        
        
        struct event_base {
        
            // Function pointers other data to describe this event_bases backend.
            const struct eventop *evsel;
            
            // Pointer to backend-specific data.
            void *evbase;
        
            // List of changes to tell backend about at next dispatch.  Only used by the O(1) backends.
            struct event_changelist changelist;
        
            // Function pointers used to describe the backend that this event_base uses for signals 
            const struct eventop *evsigsel;
            
            // Data to implement the common signal handelr code.
            struct evsig_info sig;
        
            // Number of virtual events 
            int virtual_event_count;
            
            // Number of total events added to this event_base 
            int event_count;
            
            // Number of total events active in this event_base 
            int event_count_active;
        
            // Set if we should terminate the loop once were done processing events.
            int event_gotterm;
            
            // Set if we should terminate the loop immediately 
            int event_break;
            
            // Set if we should start a new instance of the loop immediately. 
            int event_continue;
        
            // The currently running priority of events 
            int event_running_priority;
        
            // Set if were running the event_base_loop function, to prevent reentrant invocation.
            int running_loop;
        
            // Active event management.
            // An array of nactivequeues queues for active events (ones that have triggered, whose callbacks
            // need to be called).  Low priority numbers are more important, stall higher ones.
            struct event_list *activequeues;
            
            // The length of the activequeues array 
            int nactivequeues;
        
            
        
            // An array of common_timeout_list* for all of the common timeout values we know. 
            struct common_timeout_list **common_timeout_queues;
        
            // The number of entries used in common_timeout_queues 
            int n_common_timeouts;
            
            // The total size of common_timeout_queues.
            int n_common_timeouts_allocated;
        
            // List of defered_cb that are active.  We run these after the active events. 
            struct deferred_cb_queue defer_queue;
        
            // Mapping from file descriptors to enabled (added) events 
            struct event_io_map io;
        
            // Mapping from signal numbers to enabled (added) events.
            struct event_signal_map sigmap;
        
            // All events that have been enabled (added) in this event_base 
            struct event_list eventqueue;
        
            // Stored timeval; used to detect when time is running backwards.
            struct timeval event_tv;
        
            // Priority queue of events with timeouts. 
            struct min_heap timeheap;
        
            // Stored timeval: used to avoid calling gettimeofday/clock_gettime too often. 
            struct timeval tv_cache;
        
        #if defined(_EVENT_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
            // Difference between internal time (maybe from clock_gettime) gettimeofday.
            struct timeval tv_clock_diff;
            
            // Second in which we last updated tv_clock_diff, in monotonic time. 
            time_t last_updated_clock_diff;
        #endif
        
        #ifndef _EVENT_DISABLE_THREAD_SUPPORT
            
            // The thread currently running the event_loop for this base 
            unsigned long th_owner_id;
            
            // A lock to prevent conflicting accesses to this event_base
            void *th_base_lock;
            
            // The event whose callback is executing right now 
            struct event *current_event;
            
            // A condition that gets signalled when were done processing an event with waiters on it. 
            void *current_event_cond;
            
            // Number of threads blocking on current_event_cond. 
            int current_event_waiters;
        #endif
        
            // Flags that this base was configured with 
            enum event_base_config_flag flags;
        
            // Notify main thread to wake up break, etc.  True if the base already has a pending notify, we dont need
            // to add any more.
            int is_notify_pending;
            
            // A socketpair used by some th_notify functions to wake up the main thread. 
            evutil_socket_t th_notify_fd[2];
            
            // An event used by some th_notify functions to wake up the main thread. 
            struct event th_notify;
            
            // A function used to wake up the main thread from another thread. 
            int (*th_notify_fn)(struct event_base *base);
        };
        
    2. 经典流程
            (1) 创建 event_base
                    可以调用 event_base_new() -> event_base_new_with_config(NULL), 这里面创建一个 
                struct event_base, 并对 event_base 进行初始化设置, 其中包括 IO 多路复用函数的选择.如果 libevent 使能
                多线程方式, 则初始化对应的锁和条件变量.
                        struct event_base *base = event_base_new();
                
            (2) 创建 event
                    调用　event_new(event_base, fd, events, cb, arg) ->  event_assign(event, event_base, fd, events, cb, arg)
            
                    struct event *cmd_ev = event_new(base, fd, EV_READ | EV_PERSIST, cmd_cb, NULL);
                注意: 如果这个 event 是用来监听一个信号的，那么就不能让这个 event 监听读或者写事件。原因是其与信号 event 的实现方法
                      相抵触
                      
            (3) 将 event 加入到 event_base 中
                    调用 event_add(struct event *ev, const struct timeval *tv), 主要是将对应的
                 event->fd 加入到 IO 多路复用中, 并且将 event 加入到 event_base.eventqueue 中, 将 event.ev_flags 置为
                 EVLIST_INIT | EVLIST_INSERTED
                 
            (4) 进入主循环, 监听 event, 并调用 event 对应的回调函数
                    event_base_dispatch() -> event_base_loop(event_base, 0) -> event_base_loop(struct event_base *base, int flags)
                    
                调用 evsel->dispatch() 多路 IO 复用函数进行 event 的监听, 把满足条件的 event 放到 event_base 的激活队列中, 
                此时 event.ev_flags 值被置为 EVLIST_INIT | EVLIST_INSERTED | EVLIST_ACTIVE
                再调用 event_process_active() 遍历这个激活队列所有的活跃的 event, 并逐个调用对应的回调函数, 调用的顺序是根据
                优先级从高到低优先级遍历激活 event 队列(event->ev_pri 为 0 是最高的), 在调用回调函数之前需要先将 event 从激活队列
                删除, 如果ev->ev_events 包含 EV_PERSIST(持久化), 不需要再从注册队列中删除, 如果ev->ev_events 不包含
                EV_PERSIST(持久化), 则需要从注册队列中删除, 并且调用多路 IO 复用函数将 fd 删除.
                    
```

## event 优先级
```shell
    1. 设置 event_base 优先级队列个数
        int event_base_priority_init(struct event_base *base, int npriorities)
            设置优先级队列, 其中 npriorities 代表优先级队列的个数, 从 0 开始, 要在　event_base_dispatch() 前 
            调用　event_base_priority_init() 
            
    2. 设置 event 的优先级
            int　event_priority_set(struct event *ev, int pri)
            主要注意的是传入参数 pri 不能 >= ev->ev_base->nactivequeues, 同时设置的该 event 不能处于激活(EVLIST_ACTIVE)的状态 
            
    3. 在 event_assign 函数中, 有对 event 进行默认的优先级设置 ev->ev_pri = base->nactivequeues / 2; 为优先级数组长度的一半，
    
```

## 信号 event 处理
```shell
    1. 
        //evsignal-internal.h文件
        struct evsig_info {
            //用于监听 socketpair 读端的 event. ev_signal_pair[1]为读端
            struct event ev_signal;
            
            //socketpair
            evutil_socket_t ev_signal_pair[2];
            
            //用来标志是否已经将 ev_signal 这个 event 加入到 event_base 中了
            int ev_signal_added;
            
            //用户一共要监听多少个信号
            int ev_n_signals_added;
         
            //数组。用户可能已经设置过某个信号的信号捕抓函数。但
            //Libevent还是要为这个信号设置另外一个信号捕抓函数，
            //此时，就要保存用户之前设置的信号捕抓函数。当用户不要
            //监听这个信号时，就能够恢复用户之前的捕抓函数。
            //因为是有多个信号，所以得用一个数组保存。
        #ifdef _EVENT_HAVE_SIGACTION
            struct sigaction **sh_old; 
        #else//保存的是捕抓函数的函数指针，又因为是数组。所以是二级指针
            ev_sighandler_t **sh_old; 
        #endif
            
            int sh_old_max; //数组的长度
        };
        
        struct event_base {
         
            const struct eventop *evsigsel;
            struct evsig_info sig;
         
            ...
            struct event_signal_map sigmap;
            ...
        };
        
    2. 信号 event 的初始化
        在 event_base 初始化时 -> base->evbase = base->evsel->init(base) -> evsig_init(base) 这里面创建 socketpair
        并将 socketpair 的一个读端与 base->sig.ev_signal 相关联
        设置　base->sig.ev_signal_pair[1] 读事件的回调函数
        event_assign(&base->sig.ev_signal, base, base->sig.ev_signal_pair[1],
                EV_READ | EV_PERSIST, evsig_cb, base);
        
        将　
            static const struct eventop evsigops = {
                "signal",
                NULL,
                evsig_add,
                evsig_del,
                NULL,
                NULL,
                0, 0, 0
            };
            
            赋值给 event_base.evsigsel 
    3. 将信号 event 加入到 event_base 中 evsig_add() 函数
            当　sig->ev_signal_added(struct evsig_info *sig) 变量为 0 时(即用户第一次监听一个信号),就会将 sig->ev_signal 
            这个 event 加入到 event_base 中.只需要一个 event 就可以监听不同的信号, 因为在信号处理函数中往 socketpair 写的 socket
            发送 sig 值, 信号触发函数中就可以通过信号值来判断说明信号类型, 所以 Libevent 并不会为每一个信号监听创建一个 event,
            它只会创建一个全局的专门用于监听信号的 event
            
    4. 设置信号触发回调函数
            在 evsig_add() 函数中调用 _evsig_set_handler 函数完成设置 Libevent 内部的信号捕抓函数
        其中都对指定的 signal 设置对应的回调函数　evsig_handler, 这个回调函数  evsig_handler 做的事情就是向 socketpair[0] 写
        对应的 signal 值　同时用 *sig->sh_old[evsignal] 保存原来的信号对应的回调函数
        
        注意: 当我们对某个信号进行 event_new 和 event_add 后, 就不应该再次设置该信号的信号捕抓函数。否则 event_base 
             将无法监听到信号的发生, 而是被最新的回调函数所替代.
             
    5. 激活信号 event
       当对应的信号触发后, 信号的回调函数被触发, 往 socketpair[0] 写对应的信号值的数据, 信号 event 从  socketpair[1] 检测到
     　读事件, 调用 evsig_cb 回调函数, 这个回调函数主要将指定的 event_signal_map.entries[sig] 对应的 
       evmap_signal.events(struct event_list) 进行遍历, 其同一个 sig 信号可能对应的多个 events , 加入到 active 队列中
       TAILQ_INSERT_TAIL(&base->activequeues[ev->ev_pri], ev,ev_active_next);
       
    6. 执行以激活的信号 event
            event_base_dispatch() -> event_base_loop(event_base, 0) -> event_base_loop(struct event_base *base, int flags)
       跟 io 事件的回调函数触发一致, 都是先从最高优先级激活队列中进行遍历
```

## evthread_notify_base 通知主线程
```shell
    1. 
        (1) 背景
                在主线程中调用 event_base_dispatch() 函数, 该函数又调用了多路 IO 复用函数, 例如 epoll_wait(), 进行超时等待, 如果另外一个线程
    　　      向 event_base 添加了一个 event(不管是 IO 还是 超时), 都要及时告知 epoll_wait().
        (2) 原理
                跟信号 event 类似, 提供一个内部的 IO  event, 专门用于唤醒主线程。当其他线程有 event 要 add 进来时,就往这个内部的
            IO event 写入一个字节。此时，主线程在 dispatch 时，就能检测到可读，也就醒来了,这就完成了通知
            
        (3) 
            struct event_base {
                …
                //event_base是否处于通知的未决状态。即次线程已经通知了，但主线程还没处理这个通知
                int is_notify_pending;
                
                evutil_socket_t th_notify_fd[2]; //通信管道
                struct event th_notify;//用于监听 th_notify_fd 的读端
                
                //有两个可供选择的通知函数，指向其中一个通知函数, 
                // 主要是向写端发送一个字节的数据, 在 event_base 中的　event_base_dispatch() 事件中检测到事件
                int (*th_notify_fn)(struct event_base *base);
            };
            
    2. 创建通知 event 并将之加入到 event_base
            创建通知 event , 并加入 event_base 的时机是 event_base_new() -> event_base_new_with_config() -> 
            evthread_make_base_notifiable(), 只有在已经支持多线程的情况下才会调用 evthread_make_base_notifiable 函数的,
            event_base.th_notify_fn() 通知 event_base 检测到事件, 也是在　evthread_make_base_notifiable() 中进行设置, 
            同时将 base->th_notify(通知事件) 优先级设置为最高
            
    3. 当其他线程 add 新的 event 到 event_base 后调用 evthread_notify_base() 进行通知
            int evthread_notify_base(struct event_base *base)
            {
                ......
                //通知处于未决状态，当event_base醒过来就变成已决的了。
                base->is_notify_pending = 1;
                
               
                return base->th_notify_fn(base);
            }
            
    4. event_base 中 event_base.th_notify_fd[0] 通知读事件回调函数, 做的事情是将 event_base.th_notify_fd[0] 所有的数据都读完,
       event_base.is_notify_pending 置为 0,　结束,  event_base_dispatch()重新开始, 这个时候就可以将新的 event 进行监听了
       
    4. 要让 libevent 支持通知机制, 必须让 libevent 使用多线程, 在代码一开始调用　evthread_use_pthreads()
```

## 超时 event 处理
```shell
    1. 超时 event 处理原理, libevent 同时进行多个超时 event 进行监听, 采用小根堆的形式拿到最小值, 再将最小值赋值给多路 IO 复用函数中
       超时时间.
    2. 设置超时值
            int　event_add(struct event *ev, const struct timeval *tv)　->  
            int event_add_internal(struct event *ev, const struct timeval *tv, int tv_is_absolute)
            其中 tv_is_absolute 设置为 0, 代表使用相对时间, 例如隔 internal 时间, 如果 设置为 1, 则使用绝对时间, 不能连续连续触发
            超时事件.
            
            对于同一个 event, 如果是 IO event 或者信号 event，那么将无法多次添加. 但如果是一个超时 event, 那么是可以多次添加的.
            并且对应超时值会使用最后添加时指明的那个，之前的统统不要，即替换掉之前的超时值
            
            如果调用 event_add_internal() 中 tv 不为空, 则 event.ev_flags 增加了 EVLIST_TIMEOUT
            
            event_add_internal() 这个函数同时还要考虑到是其他的线程进行调用, 那么如果超时时间改变比原来更小, 则需要调用
            evthread_notify_base() 通知 event_base_dispatch() 超时时间改变了
    3. 调用多路 IO 复用函数
            event_base_loop() 对于超时事件处理, 首先从超时事件堆中取出最小的时间 event.ev_timeout 已经是绝对时间了(time_t 时间戳),
       如果绝对时间已经小于 now, 则多路 IO 复用函数超时时间设定为 0, 立马返回, 否则设定为时间最短的超时时间. 接着调用 timeout_process(base)
       处理超时事件, 将满足的所有超时事件插入到激活链表中. 需要遍历小根堆的元素, 而不是只取出堆顶,因为当主线程调用多路 IO 复用函数
       进入等待时，其他线程可能添加了多个超时值更小的 event 之后按优先级处理激活队列
            
    4. 判断该 event 是不是　EV_PERSIST, 如果是在 event_base_loop()-> event_process_active(base) ->     
       event_process_active_single_queue(base, activeq) -> event_persist_closure() 中重新加到多路 IO 复用函数中
       如果用户指定 EV_PERSIST，那么在 event_assign 中就记录下来, 在 event_process_active_single_queue 函数中会针对永久 event 
       进行调用 event_persist_closure 函数对之进行处理。在 event_persist_closure 函数中，如果是一般的永久 event
       (比如读写事件不设置超时时间, 只 EV_PERSIST, 读写触发完后不重新加入多路 IO 复用, 因为就没删除过)，那么就直接调用该 event 
       的回调函数。如果是超时永久 event，那么就需要再次计算新的超时时间，并将这个 event 再次插入到 event_base 中(timeout_process 函数删除过)
       
       如果一个 event(有设置超时时间) 因可读而被激活，那么其超时时间就要重新计算。而不是之前的那个了。也就是说，如果一个 event 设置了
       3秒的超时，但 1 秒后就可读了，那么下一个超时值，就要重新计算设置，而不是 2 秒后
       
  
```

## 时间管理
```shell
     1. Libevent 的时间一般是用在超时 event 的。对于超时 event，用户只需给出一个超时时间，比如多少秒，而不是一个绝对时间。
        但在 Libevent 内部，要将这个时间转换成绝对时间。所以在 Libevent 内部会经常获取系统时间(绝对时间)，然后进行一些处理，
        比如，转换、比较
     2. cache 时间, 每次 event_base_loop 循环, 都会更新 cahce(绝对时间) , gettime(struct event_base *base, struct timeval *tp)
     　　函数是如果 cache 有记录,优先使用 cache, 没有则通过调用 gettimeofday() 函数获取系统时间.
     3. 解决用户手动修改系统时间, 因为 libevent 内部是使用绝对时间, 而对外提供的是时间长度, 如果当前 9 点, 用户设置 1 分钟, 如果这时
     　　用户设置系统时间为 7 点, 则应该是 7 点 1 分触发事件.
            (1) 解决方案一：
                    使用 monotonic 时间, 因为单调时间是从 boot 启动到现在的时间, 用户是没有办法修改这个绝对时间. 即如果
                clock_gettime(CLOCK_MONOTONIC, &ts) 返回值为 0, 则代表系统支持 monotonic 时间 
                
            (2) 如果系统不是单调递增时间, 则只能使用获取用户能修改的时间, 需要尽可能精确记录时间差.尽可能精确地计算用户往回调了多长时间,
                将小根堆中的全部 event 都减去相同的时间差就可以了. 
                libevent 用 event_base.event_tv 实时保存用户系统时间(可能会发生时间前置的情况), libevent 采用了多采点的方式, 
                event_base_loop 函数中的 while 循环体里面每次循环 gettime(base, &base->event_tv), 都会将用户系统最新的时间
                更新到 event_base.event_tv(绝对时间)
                
     4. libevent 出现问题的主要原因是 event_base.event_tv(绝对时间), 事件的 event.ev_timeout(绝对时间)不同步, 判断是否需哟啊调整时间
     　　是通过 event_base.event_tv 与 now 相比, 再将 event.ev_timeout 进行调整, 如果 event_base.event_tv 有问题, 但是 
         event.ev_timeout 已经是正确的, 还进行错误调整, 就会出现问题.
         例如:
            在 event_base_new_with_config 函数中有 gettime(base,&base->event_tv)，所以 event_base.event_tv 记录了修改前的时间
         而 event_add 是在修改系统时间后才调用的。所以 event 结构体的 event.ev_timeout 变量使用的是修改系统时间后的超时时间，
         这是正确的时间。在执行 timeout_correct 函数时，Libevent 发现用户修改了系统时间(根据 event_base.event_tv 来判断)，
         所以就将本来正确的 event.ev_timeout 减去了 off。所以 event.ev_timeout 就变得比较修改后的系统时间小了。
         在后面检查超时时，就会发现该 event 已经超时了(实际是没有超时)，就把它触发。
                
```

## common-timeout(公共超时管理)
```shell
    1. libevent 2.0 使用了 common-timeout 和小根堆配合使用. 小根堆是适用于多个超时 event 的超时时长是随机的(但是会按照超时时间点来排序)
       多路 IO 复用函数超时时间则需要从小根堆中取出最小超时时间点 - now.
       而 common-timeout 则适用与大量的超时 event 具有相同的超时时长
       
    2. 
       struct event_base {
            //因为可以有多个不同时长的超时 event 组。故得是数组
            //因为数组元素是 common_timeout_list 指针，所以得是二级指针
            struct common_timeout_list **common_timeout_queues;
            
            //数组元素个数
            int n_common_timeouts;
            
            // 已分配的数组元素个数,包含部分还没有使用的
            // 在　event_base_init_common_timeout() 函数中先提前申请 n_common_timeouts_allocated 个　struct common_timeout_list *
            // 以后就先从这里面找
            int n_common_timeouts_allocated;
       };
       
       struct common_timeout_list {
            //超时 event 队列, 将所有具有相同超时时长的超时 event 放到一个队列里面
            struct event_list events;
            
            //超时时长
            struct timeval duration;
            
            //具有相同超时时长的超时 event 代表
            // 并不是从多个具有相同超时时长的超时 event 中选择一个作为代表，而是在内部有一个event
            struct event timeout_event;
            
            struct event_base *base;
       };
       
       common_timeout_list 结构体里面有一个 event 结构体成员, 并不是从多个具有相同超时时长的超时 event 中选择一个作为代表,
       而是在内部有一个 event
       
    3. 区分使用 common-timeout 还是小顶堆
            (1) 在 event_add() 中需要判断是使用 common-timeout 还是小顶堆, 通过 tv.tv_usec 的内容进行判断(即 common-timeout标志)
            (2) tv.tv_usec 代表微秒, 最大也就是 999999, 只用低 20 比特位就可以了, 所以前 12 bit 用来标识 common-timeout,
                最高的 4 比特位用来标志是不是 common-timeout, (tv->tv_usec & COMMON_TIMEOUT_MASK) == COMMON_TIMEOUT_MAGIC
                取最高的前 4 bit 进行判断. 次 8 比特位用来记录该超时时长在 event_base.common_timeout_queues数组中的位置，即下标值,
                因为只有 8 bit , 所以最多是 256 个 struct common_timeout_list *.
                根据这些可以决定插入小根堆还是　common-timeout
                
    4. 申请特定时长的 common-timeout
            const struct timeval* event_base_init_common_timeout(struct event_base *base, const struct timeval *duration)
            这个主要是如果 event_base.common_timeout_queues 中没有 duration 的时长, 就申请新的 struct common_timeout_list* 
            加入到 event_base.common_timeout_queues 中. 同时返回一个 struct timeval 结构体指针变量(此时已经有 common-timeout)
            的标志了. 这时可以作为参数传到 event_add() 的第二个函数中.
            在 event_base_init_common_timeout() 函数中给 common_timeout_list.timeout_event 设置了回调函数和回调参数, 回调函数
            的参数是这个 common_timeout_list 结构体变量指针, 在回调函数中, 有了这个指针, 就可以访问 events 变量，
            即访问到该结构体上的所有超时 event,这样就可以手动激活这些超时 event
            
    5. 在 event_add() 中当超时 event 被加入 common-timeout 时并不会设置 notify 变量的，即不需要通知主线程. 而如果超时 event 
       加入到小顶堆中则会设置 notify,通知主线程. 同时将这个 common-timeout 标志的 event 插入到对应的 event_base.common_timeout_queues
       的　common_timeout_list.events 中, 虽然有相同超时时长，但超时时间(超时时刻)却是 超时时长 + 调用event_add的时间。
       所以是在不同的时间触发超时的。它们根据绝对超时时间，升序排在队列中。 一般来说，直接插入队尾即可。因为后插入的，绝对超时时间肯定大。
       但由于线程抢占的原因，可能一个线程在 evutil_timeradd(&now, &tmp, &ev->ev_timeout); 执行完，还没来得及插入，
       就被另外一个线程抢占了。而这个线程也是要插入一个 common-timeout的超时event。这样就会发生：超时时间小的反而后插入。
        所以要从后面开始遍历队列，寻找一个合适的地方.
        
    6. common-timeout 和小根堆配合使用
            common-timeout 是采用一个 event 代表的方式进行工作的, 肯定要有一个代表被插入小根堆中, 首先先拿出这个 common_timeout_list
       中超时时刻最短的作为加入[小根堆]超时时间, 但是 event 则是用 common_timeout_list.timeout_event(内部的代表)
       
    7. common-timeout event 激活
           common-timeout 的回调函数主要进行处理　common_timeout_list.events 中满足超时的回调函数, 在回调函数中,
           会手动把用户的超时 event 激活,这样用户的超时 event 就能被处理了
                
```

## evbuffer 与网络 IO
```shell
    1. 在读取 socket 数据之前, Libevent 会调用 ioctl 函数来获取这个 socket 的读缓冲区中有多少字节, 进而确定本次要读多少字节到 
       evbuffer 中. Libevent 会根据要读取的字节数, 在真正 read 之前会先把 evbuffer 扩容, 免得在 read 的时候缓冲区不够.
       在 linux 环境下, Libevent 就会选择在 n 个 evbuffer_chain 中找到足够的空闲空间(往往通过申请堆空间), 因为这样可以使用类似
       Linux 的 iovec 结构体. 把链表的各个 evbuffer_chain 的空闲空间的地址赋值给 iovec 数组, 然后调用 readv 函数直接读取,
       readv 会把数据读取到相应的 chain 中
       
    2. 写数据, evbuffer 是用链表的形式存放数据, 要把这些链表上的数据写入 socket, 可以使用 writev() 函数, 如果不支持 writev() 函数,
       只能使用一般的 write 函数, 但这个函数要求数据放在一个连续的空间, 所以通过 evbuffer_pullup() 将链表中各个 chain 的数据 copy 
       到一个连续的内存空间. 也可以通过 sendfile() 函数进行发送

```

        

## bufferevent 工作流程
```shell
    1. 
        struct bufferevent {
            
            struct event_base *ev_base;
            
            // 操作结构体，成员有一些函数指针
            const struct bufferevent_ops *be_ops;
        
            // 读事件event
            struct event ev_read;
            
            // 写事件event
            struct event ev_write;
        
            // 读缓冲区
            struct evbuffer *input;
        
            // 写缓冲区
            struct evbuffer *output;
        
            // 读水位
            struct event_watermark wm_read;
            // 写水位
            struct event_watermark wm_write;
            
            // 可读时的回调函数指针
            bufferevent_data_cb readcb;
            
            // 可写时的回调函数指针
            bufferevent_data_cb writecb;
            
            // 错误发生时的回调函数指针
            bufferevent_event_cb errorcb;
            
            // 回调函数的参数
            void *cbarg;
        
            // 读事件 event 的超时值
            struct timeval timeout_read;
            // 写事件 event 的超时值
            struct timeval timeout_write;
        
            
            short enabled;
        };
        
        
        struct bufferevent_private {
     
            struct bufferevent bev;
        
            // 设置 input evbuffer 的高水位时，需要一个 evbuffer 回调函数配合工作
            struct evbuffer_cb_entry *read_watermarks_cb;
        
            // If set, we should free the lock when we free the bufferevent. 
            unsigned own_lock : 1;
        
            // Flag: set if we have deferred callbacks a read callback is pending.
            unsigned readcb_pending : 1;
            
            // Flag: set if we have deferred callbacks a write callback is pending.
            unsigned writecb_pending : 1;
            
            // Flag: set if we are currently busy connecting.
            unsigned connecting : 1;
            
            // 标志连接被拒绝
            unsigned connection_refused : 1;
            
            // Set to the events pending if we have deferred callbacks an events callback is pending.
            short eventcb_pending;
        
            // 标志是什么原因把 读 挂起来(这个标志设置后是的读事件无法被监听)
            bufferevent_suspend_flags read_suspended;
        
            // 标志是什么原因把 写 挂起来(这个标志设置后是的写事件无法被监听)
            bufferevent_suspend_flags write_suspended;
        
            // Set to the current socket errno if we have deferred callbacks an events callback is pending. 
            int errno_pending;
        
            // The DNS error code for bufferevent_socket_connect_hostname 
            int dns_error;
        
            // Used to implement deferred callbacks 
            struct deferred_cb deferred;
        
            
            enum bufferevent_options options;
        
            // bufferevent的引用计数
            int refcnt;
        
            // Lock for this bufferevent.  Shared by the inbuf the outbuf. If NULL, locking is disabled.
            void *lock;
        
            // Rate-limiting information for this bufferevent 
            struct bufferevent_rate_limit *rate_limiting;
        };
        
        struct bufferevent_ops {
            // 类型名称
            const *type;
            
            
            // 成员 bev 的偏移量
            off_t mem_offset;
        
            // 启动.将 event 加入到 event_base 中
            int (*enable)(struct bufferevent *, short);
        
            //  Disables one more of EV_READ|EV_WRITE on a bufferevent. 
            int (*disable)(struct bufferevent *, short);
        
            // Free any storage deallocate any extra data structures used in this implementation.
            void (*destruct)(struct bufferevent *);
        
            // Called when the timeouts on the bufferevent have changed.
            int (*adj_timeouts)(struct bufferevent *);
        
            // Called to flush data. 
            int (*flush)(struct bufferevent *, short, enum bufferevent_flush_mode);
        
            // 获取成员的值
            int (*ctrl)(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);
        
        };
        
    2. bufferevent 接口
            (1) struct bufferevent *bufferevent_socket_new(struct event_base *base, evutil_socket_t fd, int options)
                    描述:
                         分配一个 bufferevent 对象并初始化, 该函数内部依次调用了 bufferevent_init_common_, event_assign(evread),
                         event_assign(evwrite), 在该函数中只是初始化了读写事件, 并没有调用 event_add, 两个事件的回调函数分别是定义在
                         bufferevent_socket.c 中的 bufferevent_readcb, bufferevent_writecb, 传递的参数是 bufferevent 指针
                         
                    参数:
                        options: 
                                BEV_OPT_CLOSE_ON_FREE ：　当释放这个事件的时候, 会自动关闭文件描述符等数据
                                
            ()  struct bufferevent *bufferevent_new(evutil_socket_t fd, evbuffercb readcb, evbuffercb writecb, 
                                                    everrorcb errorcb, void *cbarg)
                    描述:
                        分配一个 bufferevent 对象并设置回调, 该函数的实现其实是依次调用了 bufferevent_socket_new 和 
                        bufferevent_setcb
                        
                    注意：
                        不推荐使用, 改用 bufferevent_socket_new 和  bufferevent_setcb 配合使用
                        
            ()  int bufferevent_socket_connect(struct bufferevent *, const struct sockaddr *, int socklen);
                    描述:
                        用于客户端的连接, 函数内调用了 connect 系统回调函数, 再将创建的 socket 赋值给 struct bufferevent,
                        简化的调用流程
                        
            () struct evconnlistener *evconnlistener_new_bind(struct event_base *base, evconnlistener_cb cb, void *ptr, 
                                                              unsigned flags, int backlog, const struct sockaddr *sa, 
                                                              int socklen)
                    描述:
                        函数内部已经调用的 accept, cb 函数回调函数被调用, 说明有一个客户端连接到这个服务器, 
                         
            () void bufferevent_setcb(struct bufferevent *bufev, bufferevent_data_cb readcb, bufferevent_data_cb writecb,
                                      bufferevent_event_cb eventcb, void *cbarg)
                    描述: 
                        为 bufferevent 结构设置 readcb、writecb、eventcb(error 的错误回调函数)
                        
            () int bufferevent_enable(struct bufferevent *bufev, short event)
                    描述:
                        使 bufferevent 生效(初始化的时候并未调用 event_add), 该函数调用 bufferevent 的成员 be_ops.enable, 
                    根据上面 bufferevent_init_common_函数可知, bufferevent_enable 实际调用了 
                    bufferevent_ops_socket.be_socket_enable, bufferevent_ops_socket.be_socket_enable 调用了 
                    bufferevent_add_event_函数, bufferevent_add_event_ 函数调用了 event_add。使得 evread、evwrite 事件有效的
                    函数是 bufferevent_enable
                    
            () int bufferevent_disable(struct bufferevent *bufev, short event)
                    描述:
                        使 bufferevent 失效, bufferevent_disable  -> 
                        bufferevent_ops_socket.be_socket_disable -> event_del
                                 
            () int bufferevent_write(struct bufferevent *bufev, const void *data, size_t size)
                    描述:
                        往该 struct bufferevent 结构体中写入数据, 内部则自动将数据通过绑定的 socket 发送出去.
                        
            () size_t bufferevent_read(struct bufferevent *bufev, void *data, size_t size)
                    描述:
                        从 struct bufferevent 中取出 socket 接受的数据, 赋值到 data 缓冲区中.
                        
                        
        
    3. bufferevent 使用
            (1) 先调用 bufferevent_socket_new(), bufferevent_setcb(), 
                再调用 bufferevent_enable(struct bufferevent *bufev, short event) 实际上进行 event 加入到 event_base 中.
                再 event_base_dispatch
                
            (2) 读事件的水位:
                    a. 低水位, 是当可读的数据量到达这个低水位后，才会调用用户设置的回调函数, 比如用户想每次读取 100 字节,
                       那么就可以把低水位设置为 100. 当可读数据的字节数小于 100 时, 即使有数据都不会打扰用户(即不会调用用户设置
                       的回调函数). 可读数据大于等于 100 字节后, 才会调用用户的回调函数
                    b. 高水位, 把读事件的 evbuffer 的数据量限制在高水位之下. 比如, 用户认为读缓冲区不能太大(太大的话,链表会很长).
                       那么用户就会设置读事件的高水位. 当读缓冲区的数据量达到这个高水位后, 即使 socket fd 还有数据没有读,
                       也不会读进这个读缓冲区里面. 就是控制 evbuffer 的大小. Libevent 发现 evbuffer 的数据量到达高水位后,
                       就会把可读事件给挂起来,让它不能再触发了(这样就防止不断的死循环)
                       
            (3) 从 socket 中读取数据: 如果一个 socket 可读, 那么监听可读事件的 event 的回调函数就会被调用. 这个回调函数是在
                bufferevent_socket_new 函数中被 Libevent 内部设置的, 设置为 bufferevent_readcb 函数, 用户并不知情.
                当 socket 有数据可读时, Libevent 就会监听到, 然后调用 bufferevent_readcb 函数处理. 该函数会调用 evbuffer_read
                函数, 把数据从 socket fd 中读取到 evbuffer 中. 然后再调用用户在 bufferevent_setcb 函数中设置的读事件回调函数.
               当用户的读事件回调函数被调用时, 数据已经在 evbuffer 中了, 用户拿来就用, 无需调用 read 这类会阻塞的函数
               
               对用户的读事件回调函数的触发是边缘触发的.在回调函数中, 用户应该尽可能地把 evbuffer 的所有数据都读出来.
               如果想等到下一次回调时再读, 那么需要等到下一次 socketfd 接收到数据才会触发用户的回调函数. 如果之后 socket fd 
               一直收不到任何数据, 那么即使 evbuffer 还有数据，用户的回调函数也不会被调用了
               
            (3) 处理写事件, 因为写缓冲区没有写满, 监听写事件一直被触发, 做法是当我们确实要写入数据时, 才监听可写事件. 我们调用
                bufferevent_write 写入数据时, Libevent 才会把监听可写事件的那个 event 注册到 event_base 中. 当 Libevent 
                把数据都写入到 fd 的缓冲区后, Libevent 又会把这个 event 从 event_base 中删除
                
    4. 使用流程
            (1) 
                客户端:
                 struct event_base* base = event_base_new();
                 // sockfd 已经成功连接
                 struct bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
                 bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void*)ev_cmd);
                 bufferevent_enable(bev, EV_READ | EV_PERSIST);
                 event_base_dispatch(base);                                
                    
```

## libevent 细节解析
```shell
    1. 一个超时 event 可以多次调用 event_add 函数
            所有的 event 都可以多次调用 event_add 函数. 不过只有超时 event 多次调用才有实质的意义, 其他 event 多次调用会被发现.
            然后直接 return.如果每次调用 event_add 时, 超时值不同的话，那么以最后一次调用的为准. 如果想取消超时, 让这个 event 
            变成普通的 event, 直接把 event_add 的第二个参数设为 NULL 即可
            
    2. 将一个超时 event 设置成永久触发(EV_PERSIST)
            struct event *ev = event_new(base, -1, EV_PERSIST, cb, arg);
            struct timeval timout = {2, 0}; //两秒的超时
            event_add(ev, &timeout);
            
    3. 在 Linux 中, Libevent 默认使用 epoll
    
    4. 如果要使用 libevent 下的多线程, 需要在 event_base_new() 函数之前调用 evthread_use_pthreads() , 以确保 event_base 
       的线程安全
       
    5. 如果需要定制内存分配、日志、线程锁, 则需要考虑到顺序, 内存分配、日志记录、线程锁
    
    6. bufferevent 线程安全
            调用 bufferevent_socket_new 的时候加入了 BEV_OPT_THREADSAFE 选项, bufferevent 就线程安全
            
    7. bufferevent_write 是是非阻塞的, 同时调用该函数后, 会将用户数据进行缓存, 自行进行 socket 发送.
    
    8. bufferevent 的可读事件是边沿触发, 如果客户端往服务器发了 100 字节的数据, 而且客户端仅仅发送一次数据. 那么服务器触发可读事件后,
       就应该把这 100 字节都读出来, 而不是只读 4 个字节, 因为回调函数不会再触发, 直到客户端再发数据给服务端.
       
    9. 使用 bufferevent ,每次最多只能读取 4096 字节(代码写死)
    
    10. 
        event_core.a： 包含 Libevent 的核心内容. 比如 event, buffer, bufferevent, log, epoll, evthread
        event_extra.a： 包含 Libevent 额外提供的四大功能，为：event_tagging、http、dns、rpc
        event_pthreads.a： 包含了pthreads线程的具体实现
        event.a： event.a = event_core + event_extra
```

## 参考资料
```shell
    1. https://blog.csdn.net/luotuo44/article/list/1
```

## evbuffer
```shell
    1. 
        struct evbuffer_chain {
             // 下一个数据节点
            struct evbuffer_chain *next; 
            
            // buffer的大小
            size_t buffer_len;
         
            // 错开不使用的空间。该成员的值一般等于 0
            ev_off_t misalign;
         
            //evbuffer_chain 已存数据的字节数, 所以要从 buffer + misalign + off 的位置开始写入数据
            size_t off;            
            unsigned *buffer;
        };
        
        struct evbuffer {
        
            // 头数据节点
            struct evbuffer_chain *first;
            // 尾数据节点
            struct evbuffer_chain *last;
            
            //这是一个二级指针。使用*last_with_datap时，指向的是链表中最后一个有数据的evbuffer_chain。
            //所以last_with_datap存储的是倒数第二个evbuffer_chain的next成员地址。
            //一开始buffer->last_with_datap = &buffer->first;此时first为NULL。所以当链表没有节点时
            //*last_with_datap为NULL。当只有一个节点时*last_with_datap就是first。    
            struct evbuffer_chain **last_with_datap;
         
           // 链表中所有chain的总字节数
            size_t total_len;
         
        };

2. evbuffer_chain 中 evbuffer_chain.buffer 的内存申请不是单独申请的, 而是连同 evbuffer_chain 结构体一起申请, 
       evbuffer_chain.buffer 就紧跟在 evbuffer_chain 之后, 所以释放内存的时候就不需要分别释放怎么麻烦, 直接释放申请的指针地址,
       就能把 evbuffer_chain 和 evbuffer_chain.buffer 一起释放
       
    3. 在链表尾添加数据(evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen))
            (1) 该链表为空, 即这是第一次插入数据. 直接把新建的 evbuffer_chain 插入到链表中, 通过调用 evbuffer_chain_insert
            (2) 链表的最后一个节点(即evbuffer_chain)还有一些空余的空间, 放得下本次要插入的数据, 此时直接把数据追加到最后一个节点即可
            (3) 链表的最后一个节点并不能放得下本次要插入的数据，那么就需要把本次要插入的数据分开由两个 evbuffer_chain 存放
            
    4. 提前预留 buffer 空间大小(int evbuffer_expand(struct evbuffer *buf, size_t datlen))    
            预留空间的作用是当下次添加数据时, 无需额外申请空间就能保存数据
            1. 假如这个链表本身(evbuffer)还有大于 datlen 的空闲空间，那么这个 evbuffer_expand 函数将不做任何操作
            2. 如果这个链表的所有 buffer 空间都被用完了, 那么解决需要创建一个长度为 datlen 的 evbuffer_chain, 然后把这个
               evbuffer_chain 插入到链表最后面即可. 此时这个 evbuffer_chain 的 off 就等于 0
            3. 如果链表的最后一个有数据 chain 还有一些空闲空间，但小于 datlen, evbuffer_expand 是调用 
               evbuffer_expand_singlechain 实现扩大空间的. 而 evbuffer_expand_singlechain 函数有一个特点, 预留空间 datlen 
               必须是在一个 evbuffer_chain 中，不能跨 chain. 解决方案是新建一个比 datlen 更大的 chain, 把最后一个 chain 上的数据
               迁移到这个新建的 chain 上. 这样就既能保证该 chain 节点也能填满, 也保证了预留空间 datlen 必须在是一个 chain 的。
               如果最后一个 chain 的数据比较多, Libevent 就认为迁移不划算, 那么 Libevent 就让这个 chain 最后留有一些空间不使用
               
    5. 在链表头添加数据
            evbuffer_chain.misalign 代表空闲空间, 要在链表头插入数据, new 一个新的 evbuffer_chain, 然后把要插入的数据放到这个
       新建个的 evbuffer_chain 中. 但 evbuffer_chain_new 申请到的 buffer 空间可能会大于要插入的数据长度. 插入数据后, buffer 就
       必然会剩下一些空闲空间. 那么这个空闲空间放在 buffer 的前面好还是后面好呢？ Libevent 认为放在前面会好些, 此时 misalign 就有用
       它表示错开不用的空间, 也就是空闲空间. 如果再次在链表头插入数据, 就可以使用到这些空闲空间了. misalign 也可以认为是空闲空间,
       可以随时使用

    6. 字符串操作
             evbuffer_ptr_memcmp() 字符串的比较函数, 比较某一个字符串和从 evbuffer 中某个位置开始的字符是否相等. 比较的时候需要考虑到跨 evbuffer_chain 的问题.