# C++ boost库

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
