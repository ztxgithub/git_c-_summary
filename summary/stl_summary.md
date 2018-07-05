# C++ stl 

## stl简介

```c++
    1.STL中相等(equality)与等价(equivalence)的区别
    
        (1) 相等(equality) 【针对无序数据】
                 相等关系是以operator ==为基础的。
                 对象a与对象b相等: a == b 
                 注意: 对象a和对象b有相等的值并不意味着它们的所有成员数据都相等, 主要还得看 operator== 运算符的实现方式
                 例子：find,count,count_if等等大部分函数
                 
                 顺序容器采用相等
                 
                 
        (2) 等价(equivalence) 【已序数据】
                等价是基于在一个有序区间中 对象值 的相对位置
                两个对象a和b如果在关联容器set的排序顺序中　没有哪个排在另一个之前,那么它们在容器set使用的排序顺序 有等价的值
                a与b等价: !(a < b) && !(b < a) 
                         
                在一般的情况下,一个关联容器的 比较函数 并不是operator<,甚至不是less,
                他是用户自定义的判别式,每个关联容器都是通过 key_comp成员函数 使判别式可在外部被使用,
                因此如果下边的表达式为true，两个对象 x和y是等价的
                    set<string> c;
                    if (!c.key_comp()("X","x" ) && !c.key_comp()("x", "X"))
                    
               表达式!c.key_comp()(x, y)看起来很丑陋,但一旦你知道c.key_comp()返回一个函数(或一个函数对象),丑陋就消散了.
               !c.key_comp()(x, y)只不过是调用key_comp返回的函数(或函数对象),并把x和y作为实参.
               然后对结果取反,c.key_comp()(x, y)仅当在c的排序顺序中x在y之前时返回真,
               所以!c.key_comp()(x, y)仅当在c的排序顺序中x不在y之前时为真。
               
               标准关联容器采用等价: set/multiset/map/multimap 
                                  成员函数: binary_search,low_bound/upper_bound/equal_range函数.

                       
        (3) find算法和set的insert成员函数是很多必须 判断两个值是否相同的函数的代表.但它们以不同的方式完成,
            find对“相同”的定义是 "相等",基于 operator==
            set::insert对“相同”的定义是"等价",通常基于 operator<
            
            
    2.仿函数
        (1) 仿函数(functor),就是使一个类的使用看上去像一个函数.其实现就是类中实现一个operator(),
            这个类就有了类似函数的行为,就是一个仿函数类
            
            STL不使用函数指针而使用仿函数呢,因为函数指针不能满足STL对抽象性的要求,
            无法和STL的其他组件搭配以产生更加灵活的效果
            
        (2) 仿函数的应用
                通过在一个类中重载括号运算符( operator() )的方法使用一个函数对象而不是一个普通函数
                template<typename T>
                class display
                {
                    public:
                        void operator()(const T &x)
                        {
                            cout<<x<<" "; 
                        } 
                };
                
                int i[]={1,2,3,4,5};
                for_each(i, i + 5, display<int>());
            
        
                    
```
