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


