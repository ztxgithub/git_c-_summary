# C++ boost库

## 基础知识

```shell

```

## boost 简介

```c++
    1.boost是一个准标准库,相当于STL的延续和扩充,它的设计理念和STL比较接近,都是利用泛型让复用达到最大化.
      不过对比STL,boost更加实用.STL集中在算法部分,而boost包含了不少工具类,可以完成比较具体的工作
      
    2.分类
        (1) format库: 
                    提供了对流的“printf-like”功能,printf里使用%d、%s等等的参数做替换的方法在很多情况下还是非常方便的,
                    STL的iostream则缺乏这样的功能,format为stream增加了这个功能,并且功能比原始的printf更强.
                    
        (2) regex:
                    正则表达式库,如果需要做字符串分析的人就会理解正则表达式有多么有用了
                    
        (3) tokenizer库
                    支持字符串分割
                    
        (4) array：
                    提供了常量大小的数组的一个包装，喜欢用数组但是苦恼数组定位、确定数组大小等功能的人这下开心了
                    
        (5) dynamic_bitset:
                    动态分配大小的bitset
                    
        (6) pool:
                    内存池,不用害怕频繁分配释放内存导致内存碎片,也不用自己辛辛苦苦自己实现了
                    
        (7) smart_ptr:
                    智能指针
                    
        (8) date_time:
                    这个是平台、类库无关的实现，如果程序需要跨平台，可以考虑用这个
                    
        (9) uitlity里提供了一个noncopyable类
                可以实现“无法复制”的类。很多情况下，我们需要避免一个类被复制，比如代表文件句柄的类，
                文件句柄如果被两个实例共享,操作上会有很多问题,而且语义上也说不过去。
                一般的避免实例复制的方法是把拷贝构造和operator=私有化,
                现在只要继承一下这个类就可以了，清晰了很多。
                    
```

## boost 安装

```shell
    1.到　https://www.boost.org/　进行下载安装包(boost_1_67_0.tar.gz) 
    2. > tar -xvf boost_1_67_0.tar.gz 
    3. 运行bootstrap.sh脚本并设置相关参数
            > sudo ./bootstrap.sh --with-libraries=all --with-toolset=gcc
            
            其中　--with-libraries: 代表要编译那些库,all的话就是全部编译,只想编译部分库的话就把库的名称写上,
                                    之间用 , 号分隔即可
                                    atomic	
                                    chrono	
                                    context	
                                    coroutine	
                                    date_time	
                                    exception	
                                    filesystem	
                                    graph	图组件和算法
                                    graph_parallel	
                                    iostreams	
                                    locale	
                                    log	
                                    math	
                                    mpi	用模板实现的元编程框架
                                    program_options	
                                    python	把C++类和函数映射到Python之中
                                    random	
                                    regex	正则表达式库
                                    serialization	
                                    signals	
                                    system	
                                    test	
                                    thread	可移植的C++多线程库
                                    timer	
                                    wave
                                    
                --with-toolset指定编译时使用哪种编译器，Linux下使用gcc即可
                
    4.  > sudo ./b2 toolset=gcc
    5. 
        > sudo ./b2 install --prefix=/usr
        
        指定boost的安装目录,不加此参数的话默认的头文件在/usr/local/include/boost目录下,
        库文件在/usr/local/lib/目录下.这里把安装目录指定为--prefix=/usr则boost会直接安装到系统头文件目录和库文件目录下,
        可以省略配置环境变量
        
    6. > ldconfig
                                    
```

##  bind

```shell

    1.概念
        (1) bind就是函数适配器
                适配器是一种机制,把已有的东西改吧改吧、限制限制,从而让它适应新的逻辑.
                需要指出, 容器、迭代器和函数都有适配器。
                bind就是一个函数适配器,它接受一个可调用对象,生成一个新的可调用对象来适应原对象的参数列表
                
    2.简单应用
    
        #include <iostream>  
        #include <functional>  
        using namespace std;  
        using namespace std::placeholders;  // 提供占位符
          
        int main()  
        {  
            auto fun = [](int *array, int n, int num){  
                for (int i = 0; i < n; i++)  
                {  
                    if (array[i] > num)  
                        cout << array[i] << ends;  
                }  
                cout << endl;  
            };  
            
            int array[] = { 1, 3, 5, 7, 9 };  
            //_1，_2 是占位符 ,对于新的函数的参数序号
            //通过bind函数将原有的函数,提供默认形参,以形成新的函数
            auto fun1 = bind(fun, _1, _2, 5); 
             
            //等价于调用fun(array, sizeof(array) / sizeof(*array), 5);  
            fun1(array, sizeof(array) / sizeof(*array));  
            cin.get();  
            return 0;  
        }  
        
        (1) 常见用法一
                fun()的调用需要传递三个参数,而用bind()进行绑定后只需两个参数了,
                因为第三个参数在绑定时被固定了下来,减少函数参数的调用
                
                _1,_2是占位符,定义于命名空间placeholders中._1是newfun的第一个参数,_2是newfun的第二个参数
                
        (2) 常见用法二
                更改参数的调用顺序
                例如:
                    int fun(int a, int b);
                    
                    auto newfun = bind(fun, _2, _1);
                    
                    调用newfun(x, y); 相当于调用fun(y, x);
                    
    3.绑定类成员函数
        (1) 对类成员函数进行绑定与对普通函数进行绑定,稍有不同：对类成员函数绑定需要用到类对象或类指针
            这个很好理解：类成员函数被封装在类中,故不可随便访问，需借助类对象或类指针。
            
                    #include <iostream>  
                    #include <functional>  
                    using namespace std;  
                    using namespace std::placeholders;  
                    class MyClass  
                    {  
                    public:  
                        void fun1(void)  
                        {  
                            cout << "void fun1(void)" << endl;  
                        }  
                        int fun2(int i)  
                        {  
                            cout << "int fun2(int i)" << " i = " << i << endl;  
                            return i;  
                        }  
                    };  
                    
                  int main()
                  {
                      MyClass my;
                      //使用类对象绑定
                      auto fun1 = bind(&MyClass::fun1, my);
                      fun1();
                  
                      MyClass *p;
                      //使用类指针绑定
                      auto fun2 = bind(&MyClass::fun2, p, _1);
                      fun2(1);
                      
                      return 0;
                  }
                  
    4.其他注意事项
        (1) std::ref()表示传引用, std::cref()表示传不变引用, 
            std::ref()和std::cref()是针对函数式编程(如std::bind)
        
                #include <functional>
                #include <iostream>
                 
                void f(int& n1, int& n2, const int& n3)
                {
                    std::cout << "In function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
                    ++n1; // 如果是调用bind() 进行函数绑定, 则不能用传统意义上的引用
                    ++n2; // increments the main()'s n2
                    // ++n3; // compile error
                }
                 
                int main()
                {
                    int n1 = 1, n2 = 2, n3 = 3;
                    std::function<void()> bound_f = std::bind(f, n1, std::ref(n2), std::cref(n3));
                    n1 = 10;
                    n2 = 11;
                    n3 = 12;
                    std::cout << "Before function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
                    bound_f();
                    std::cout << "After function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
                }
                
                结果:
                    Before function: 10 11 12
                    In function: 1 11 12
                    After function: 10 12 12
                    
                在执行std::bind后,在函数f()中n1的值仍然是1, n2和n3改成了修改的值.
                说明std::bind使用的是参数的拷贝而不是引用。
                具体为什么std::bind不使用引用，可能确实有一些需求，使得C++11的设计者认为默认应该采用拷贝,
                如果使用者有需求，加上std::ref即可。
                
        (2) 
                void fun_2(int &a,int &b)  
                {  
                    a++;  
                    b++;  
                    cout<<a<<"  "<<b<<endl;  
                } 
                
                int n = 2;  
                int m = 3;  
                   
                auto f4 = bind(fun_2, n,placeholders::_1);  
                f4(m); //print:3  4  
               
                cout<<m<<endl;//print:4  说明：bind对于不事先绑定的参数，通过std::placeholders传递的参数是通过引用传递的  
                cout<<n<<endl;//print:2  说明：bind对于预先绑定的函数参数是通过值传递的 

```

## functioin

```shell
    1.它是函数、函数对象、函数指针、和成员函数的包装器,可以容纳任何类型的函数对象,函数指针,引用函数,成员函数的指针.
      以统一的方式处理函数,函数对象,函数指针和成员函数。
      允许保存和延迟执行函数.
      
    2.简单应用
    
        #include <functional>  
        #include <iostream>  
           
        struct Foo {  
            Foo(int num) : num_(num) {}  
            void print_add(int i) const { std::cout << num_+i << '\n'; }  
            int num_;  
        };  
           
        void print_num(int i)  
        {  
            std::cout << i << '\n';  
        }  
           
        int main()  
        {  
            // 保存自由函数  
            std::function<void(int)> f_display = print_num;  
            f_display(-9);  
           
            // 保存 lambda 表达式  
            std::function<void()> f_display_42 = []() { print_num(42); };  
            f_display_42();  
           
            // 保存 std::bind 的结果  
            std::function<void()> f_display_31337 = std::bind(print_num, 31337);  
            f_display_31337();  
           
            // 保存成员函数  
            std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;  
            Foo foo(314159);  
            f_add_display(foo, 1);  
        }  

```

## std::set

```shell
    1. std::set 内部保存是使用红黑树实现， set::insert 之后会将数据进行排序保存
    2. STL 的 map、multimap、set、multiset 都有三个比较特殊的函数，lower_bound、upper_bound、equal_range
        原型:
            iterator lower_bound (const value_type& val) const;
                描述:
                    返回指向容器中第一个值等于给定搜索值或在给定搜索值之后的元素的迭代器，
                    即第一个等于或大于给定搜索值元素的迭代器。
                    
                参数:
                     value_type 是存储在当前容器中的元素的类型
                     
                返回值:
                    指向容器中第一个等于给定搜索值或在给定搜索值之后的元素的迭代器
            
            iterator upper_bound (const value_type& val) const;
                描述:
                    查找的键值为key，则C.upper_bound(key)会查找第一个比key大的元素的迭代器
            
            pair<iterator,iterator> equal_range (const value_type& val) const;
            equal_range 返回两个迭代器，第一个迭代器是 lower_bound 的返回值，第二个迭代器是 upper_bound 的返回值
            
        (A) 实例代码
                    typedef std::pair<int, Timer*> Entry;
                    set<Entry> foo1;
                    Timer* timer1 = new Timer(1);
                    foo1.insert(make_pair(1, timer1));
                    Timer* timer2 = new Timer(1);
                    foo1.insert(make_pair(2, timer2));
                    Timer* timer6 = new Timer(1);
                    foo1.insert(make_pair(6, timer6));
                    
                    int now = 2;
                    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
                    set<Entry>::iterator it = foo1.lower_bound(sentry);
                    printf("it->first:%d\n", it->first);　　//输出　it->first:2

```

## std::copy

```shell
    1. 如果要把一个序列（sequence）拷贝到一个容器（container）中去
            std::copy(start, end, std::back_inserter(container));
            
            start 和 end 是输入序列（假设有N各元素）的迭代器（iterator），container是一个容器，该容器的接口包含函数push_back。
            假设 container 开始是空的，那么 copy 完毕后它就包含N个元素，并且顺序与原来队列中的元素顺序一样。
            标准库提供的 back_inserter 模板函数很方便，因为它为 container 返回一个 back_insert_iterator 迭代器，
            这样，复制的元素都被追加到container的末尾了。 
            现在假设 container 开始非空（例如：container 必须在循环中反复被使用好几次）。那么，要达到原来的目标，
            必须先调用 clear 函数然后才能插入新序列。这会导致旧的元素对象被析构，新添加进来的被构造。
            不仅如此，container 自身使用的动态内存也会被释放然后又创建，就像list，map，set的节点。
            某些vector的实现在调用 clear 的时候甚至会释放所有内存。
            
    2. 考虑到在一个已有的元素上直接 copy 覆盖更高效, copy只负责复制，不负责申请空间，所以复制前必须有足够的空间
        如果 container 的大小小于输入序列的长度 N 的话，程序会导致崩溃（crash）
            std::copy(start, end, container.begin()); 
```

## std::fill

```shell
    1. 将容器内某个范围值都填充为 value
            std::fill(message1.begin(), message1.end(), value);

```

## string

```shell
    1. 可以提前申请好内存大小
            string.resize(len)

```
