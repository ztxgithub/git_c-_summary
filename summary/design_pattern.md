# 设计模式

## 单例模式

```shell
    1.单例模式也称为单件模式,单子模式,其意图是保证一个类仅有一个实例,并提供一个访问它的全局访问点,该实例被所有程序模块共享
    2.较简单的应用
        v1.0.0
      1  class Singleton
      2  {
      3  private:
      4      static Singleton* instance; 
      5      Singleton(){}  //类的构造函数定义为私有函数
      6  public:
      7      static Singleton* GetInstance()
      8      {
      9          if (NULL == instance) 
      10          {
      11             instance = new Singleton();
      12          }
      13          return instance;
      14    }
      15  };
        Singleton* Singleton::instance = NULL //静态数据成员是静态存储的，必须对他进行初始化
        
        类的 构造函数 设置为private,这就意味着Client是不能通过调用类的构造函数来生成一个实例,这是为了防止　Client多次
        调用构造函数生成多个实例出来. 如果要访问Singleton,需要通过GetInstance方法,这是一个静态的方法,
        主要负责创建自己的实例，返回实例的指针给Client
        
        注意:
            v1.0.0 代码有一个问题是无法保证多线程安全, 如果线程A 刚好运行到第9行代码,刚好判断完instance为　null 后,
            线程A就被挂起,运行线程B,而线程B 也进行GetInstance(),这个时候创建了一个实例,此时又切换到线程A,线程A继续下面的操作
            又创建了一个实例,这时就两个Singleton对象
            
         解决方法:
            1.较为简单的方法, 只需要在第9行　if (NULL == instance) 前加锁(Lock)进行临界资源保护, 缺点是　
            　每次访问该函数都需要进行一次加锁操作,影响程序的性能和执行效率.
        
    3.Singleton与全局变量的区别
        (1) 全局变量的缺点
                A.变量名冲突:必须小心维护变量名,每遇到一个全局变量,都必须仔细分辨该变量属于哪个模块、哪个程序
                B.耦合度难题：使用全局变量增加了函数与模块间的耦合程度，不易维护.
                C.单个实体问题：全局变量不能防止一个类生成多个实例出来.
                D.多线程访问：并发访问时需要使用同步机制,小心保护全局变量,避免冲突.
                
    4.Singleton的懒汉模式与饿汉模式
        (1) 懒汉模式(Lazy Singleton)
                使用懒汉模式时,Singleton在程序第一次调用的时候才会初始化自己(调用自身构造函数),这叫做延迟初始化,
                使用该模式时,由于if语句的存在(在GetInstance()中有if语句),会影响调用的效率
                在多线程环境下使用时,为了保证只能初始化一个实例,需要用锁来保证线程安全性,防止同时多个线程进入if语句中.
                如果遇到处理大量数据时,锁会成为整个性能的瓶颈,代码如上(较简单的应用)
                v1.0.0 每一个不同的　单例模式,都会有内存被占用,即使该单例模式以及出了作用域.
                
                解决的方法：使用函数内的 local static 对象。当第一次访问getInstance()方法时才创建实例
                    // version 1.2.0, 最好采用这种模式
                    class Singleton
                    {
                        private:
                            Singleton() { };
                            ~Singleton() { };
                            Singleton(const Singleton&);
                            Singleton& operator=(const Singleton&);
                        public:
                            static Singleton& getInstance() {
                                static Singleton instance;
                                return instance;
                            }
                    }; 
                    
                注意:
                   1.如果单例模式类ASingleton 中调用　单例模式类BSingleton 中的方法, 则必须考虑到　单例模式类BSingleton 
                   　 先进行初始化(C++中类静态成员变量的初始化顺序是不确定的)
                   2.C++0X以后,要求编译器保证内部静态变量的线程安全性,故C++0x之后该实现是线程安全的,C++0x之前仍需加锁.
                   3.为了保证只能初始化一个实例,需要用锁来保证线程安全性
                   
                适用场景:
                    一般懒汉模式适用于程序一部分中需要使用Singleton, 且在实例化后没有大量频繁访问或线程访问的情况. 
                   
        (2) 饿汉模式(Eager Singleton)
                使用饿汉模式时,Singleton在程序一开始就将自己实例化,之后的GetInstance方法仅返回实例的指针即可,
                这样就解决了上述提到的if语句影响效率的问题.如果Singleton的成员比较庞大,复杂,实例化Singleton会花一些时间,
                且这个实例一直占用着大量内存,在使用时要注意这部分的开销.使用饿汉模式用于多线程编程的话,
                由于线程访问之前,实例已存在,就不需要像懒汉模式中加入锁,因此饿汉模式保证了多线程安全。
                饿汉模式比较适用于程序整个运行过程中都需要访问、会被频繁访问或者需要被多线程访问的情况。代码如下：
                    class Singleton
                    {
                        private:
                            Singleton* instance;
                            Singleton(){};
                        public:
                            static Singleton* GetInstance()
                            {
                                return instance;
                            }
                    };
                    
                    Singleton* Singleton::instance = new Singleton();//在此直接实例化
                    
                    
                适用场景:
                    饿汉模式适用于Singleton在程序运行过程中一直被频繁调用,这样由于预先加载了实例,
                    访问实例时没有if语句,效率更高.
                
                
        (3) 可以继承的Singleton
                Singleton简单易用,但考虑到代码复用的话,Singleton不能够被继承,如果每次都要重新写一个Singleton类是很麻烦的.
                如果需要一个可以被继承的Singleton父类,需要用到C++中的模板来实现.
                模板是C++支持参数化多态的工具,使用模板可以使 用户为 类或者函数声明一种一般模式,
                使得类中的某些数据成员或者成员函数的参数、返回值取得任意类型
                
                template<class S>
                class Singleton
                {
                    public:
                        static S* GetInstance()
                        {
                            if (instance == null)
                            {
                                instance = new S();
                            }
                            return instance;
                        };
                    protected:
                        Singleton(){};
                    private:
                        static S* instance;
                        Singleton(const Singleton & ){};
                        Singleton & operator = ( const Singleton & rhs ){};
                };
                
                
    5.线程安全
        在使用单例模式时要考虑到　线程安全的问题,双重检查锁定模式(DCLP),设计DCLP的目的在于在共享资源(如单例)初始化时
        添加有效的线程安全检查功能.但DCLP也存在一个问题：它是不可靠的。此外,
        在本质上不改变传统设计模式实现的基础上,几乎找不到一种简便方法能够使DCLP在C/C++程序中变得可靠.
        DCLP无论在单处理器还是多处理器架构中,都可能由不同的原因导致失效。
                
                
                
                
                

           
```
