# C++ 编译链接模型

## C 语言的编译模型

```c++
    1. 编译模型: 预处理->编译(耗时最大)->汇编->链接
    2. c 语言采用了隐式函数声明,当一个函数未进行定义声明时,直接使用该函数,c 编译器会默认为该函返回 int, 
       接受任意个数 int 型参数.其编译不会出错,链接会出错
    3. 预处理的作用是减少各个源代码中重复的代码
    4. c 编译器提供单遍编译,只需要记住 struct 的成员和偏移,知道外部变量的类型,就可以一边解析源代码,一边生成目标代码
       这样 c 编译器只需要栈和数组处理表达式,不用动态申请内存,所以编译器的内存消耗是固定的,这使得函数定义不能嵌套函数定义.
                    
```

## c++ 语言的编译模型

```shell
    1. c++ 继承 c 的单遍编译,这将影响到 c++ 的名字查找和函数重载决议
    2. 函数的原型声明可以看作是对函数的前向声明
```

## c++ 链接

```shell
    1.重载函数中只有返回值类型不同不能作为重载函数的标准.
    2. 	
        printer.h
       	
       	class Printer
       	{
       		public:
       			Printer();
       			~Printer();
       			
       		private:
       			class Impl;  //前向声明
       			boost::scoped_ptr<Impl> impl_;
       	};
       	
       	printer.cc
       	
       	class Printer::Impl
       	{
       		// 类 Impl 的定义 
       	};
       	
       	Printer::Printer():impl_(new Impl)
       	{}
       	
       	Printer::~Printer()
       	{
       	  // 尽管析构函数是空，但必须放到这里定义。否则编译器在调用默认的 ~Printer() inline
       	  // 时，会默认调用 ~Impl(),这时候会报错
       	}
       	
    3. 一般在类的 public 中模板的定义要放在头文件中,否则会有链接错误
       而对于 private 成员函数模板,可以不用在头文件中定义,用户代码不能调用它,无法随意具体化,
       不会造成链接错误.
       
    4. 头文件的缺点
            (1) 传递性, 头文件可以包含其他的头文件,这样导致代码非常庞大,编译缓慢,而且一旦某个头文件改动了
                        与它相关的(包含它)的源文件都要一并重新编译
                        
            (2) 顺序性, 一个源文件可以包含多个头文件,同时跟头文件的顺序有关.通常做法将头文件分为几类
                       (c语言系统头文件, c++ 标准库头文件, c++ 第三方库头文件, 公司基础库头文件, 项目本身的头文件)
    5. 头文件的使用规则
            (1) 将文件间的编译依赖降至最小
            (2) 让 class 名字, 头文件名字, 源文件名字直接相关
            
    6. 库文件的组织原则
            (1) c++ 标准库的版本跟 c++ 编译器直接关联
            (2) c 标准库的版本跟 Linux 系统版本相关
```

## 二进制兼容性

```shell
    1. 动态链接库的 ABI(application binary interface )
    2. 二进制兼容是在升级(修复 bug)库文件,不必重新编译使用这个库的可执行文件,而且程序的功能不会被破坏.
    3. 如何看改动是不是二进制兼容
            如果改的只是动态链接库的内部实现,其头文件中的对外接口保存不变,则认为是二进制兼容
    4. 避免使用虚函数作为库的接口
            虚函数作为库的接口会给二进制兼容带来麻烦,
            (1) 虚函数作为接口的 2 大用法
                    I. 调用
                    II. 回调.事件的通知,客户端代码一般会继承 interface, 然后把自己定义的回调函数注册到
                        库中,等库回调自己,
                        
            (2) 虚函数作为接口的弊端
                    一旦发布不能修改,例如,
                    原先定义
                    class Graphics
                    {
                         virtual void drawline(int x0, int y0, int x1, int y1);
                         virtual void drawline(Point p0, Point p1);
                         ........
                    };
                    
                    现在想对 drawline 新增新的方法
                    错误方法一: 从中间穿插
                        class Graphics
                        {
                             virtual void drawline(int x0, int y0, int x1, int y1);
                             virtual void drawline(double x0, double y0, double x1, double y1); // + 错误
                             virtual void drawline(Point p0, Point p1);
                             ........
                        };
                        不能以这种方式进行新增,因为 c++ 以虚函数表偏移的形式进行调用虚函数(vtable[offset]), 如果
                        从中间穿插了 virtual void drawline(double x0, double y0, double x1, double y1);
                        则导致 vtable 的排列发送变化, 使得正在运行的可执行文件无法再用旧的 offset 调用函数.
                        
                    错误方法二: 把新的虚函数放到 class 的尾部
                        class Graphics
                        {
                             virtual void drawline(int x0, int y0, int x1, int y1);
                             virtual void drawline(Point p0, Point p1);
                             ........
                             
                             virtual void drawline(double x0, double y0, double x1, double y1); // + 错误 考虑到继承
                        };    
                        原因 1 : 没有和原来的 drawline() 函数放在一起,影响阅读维护
                        原因 2:  如果 Graphics 被继承,则其派生类的虚函数表偏移会改变
                        
    5. 动态链接库的推荐做法
            (1) 场景一: 如果动态链接库的使用范围窄,只是本团队两三个程序再用,升级新版本容易协调,两个大版本(新功能增加 v1.1.0 到 v1.2.0)
                       之间不必二进制兼容.可执行程序要用新功能,那么用户代码(可执行程序)需要重新编译.如果是原地打补丁(动态链接库 fix, 
                       小版本更新 v1.1.1 到 v1.1.2),则需要进行二进制兼容(可执行程序不需要重新编译)
                       
            (2) 场景二: 库的使用范围广,用的厂家多,每个厂家的 release cycle 都不相同, 则使用 pimpl 方法(不使用virtual 函数,
                       而是使用 non-virtual 函数)
                       原来的代码:
                       
                       Graphics.h
                        class Graphics
                        {
                           public:
                               void drawline(int x0, int y0, int x1, int y1);
                               void drawline(Point p0, Point p1);
                              
                           private:
                                class Impl;  // 前向声明
                                boost:scoped_ptr<Impl> impl;
                           
                        };       
                        
                        Graphics.cpp
                         class Graphics::Impl
                            {
                                public:
                                  void drawline(int x0, int y0, int x1, int y1);
                                  void drawline(Point p0, Point p1);
                            };    
                              
                              
                         如果要加入新功能,不必通过继承来扩展,可以直接在头文件中修改(因为是 non-virtual 函数是按照名字链接的)
                         
                         Graphics.h
                         class Graphics
                         {
                            public:
                                void drawline(int x0, int y0, int x1, int y1);
                                void drawline(double x0, double y0, double x1, double y1); 
                                void drawline(Point p0, Point p1);
                               
                            private:
                                 class Impl;  // 前向声明
                                 boost:scoped_ptr<Impl> impl;
                            
                         };  
                         
                         Graphics.cpp
                              class Graphics::Impl
                                 {
                                     public:
                                       void drawline(int x0, int y0, int x1, int y1);
                                       void drawline(double x0, double y0, double x1, double y1); 
                                       void drawline(Point p0, Point p1);
                                 };  
                                 
                         这种升级方式不必重新编译可执行程序,老的可执行程序也能用(因为是调用 non-virtual 函数来调用), 所以 
                         Pimpl 方法有编译防火墙之说
                                             
                         
                    
            
    6. 在二进制兼容中 non-virtual 函数要比 virtual 函数健壮, 因为 virtual 函数是根据虚函数表的偏移(bind-by-vtable-offset),
       而 non-virtual 函数 则是根据函数名来定位(bind-by-name),而函数表的偏移在升级修改的过程中很容易移位.
                        
                    
                    
```