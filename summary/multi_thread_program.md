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


