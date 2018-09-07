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
      或则 CountDownLatch(倒时器)
      
    2. CountDownLatch 是一个同步方式，主要用途:
            (1) 主线程等待多个子线程完成初始化后才能继续执行
            (2) 多个子线程等待主线程完成一定操作后才开始执行
            
            class CountDownLatch : boost::noncopyable
            {
             public:
            
              explicit CountDownLatch(int count);
            
              void wait();
            
              void countDown();
            
              int getCount() const;
            
             private:
              mutable MutexLock mutex_;
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


