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
                
        (3) 注意事项
                我们会在不经意间使用的　值传递　或者是　本身使用库函数的时候库函数参数使用的是值传递,
                这样就会导致对象信息丢失的问题,所以说我们尽可能的不要在重载()函数中去改变对象成员属性的值
                
                例如下面是错误代码:
                    class Average
                    {
                        public:
                            Average():count(0), sum(0){ }
                            void operator()(double num)   //每次调用这个仿函数就相当于往里面添加了一个数
                            {
                                count++;
                                sum += num;
                            }
                            double GetAverage(){ return sum / count; }     //最后由这个方法得到当前的平均值
                            
                        private:
                            int count;
                            double sum;
                    };
                    
                    template < class T ,class F>
                    void For_each(T begin, T end, F functor)         //这是一个手动实现的一个简单的for_each.
                    {
                    	for (T it = begin; it != end; it++)
                    		functor(*it);
                    		
                        return functor;
                    }
                    
                    int main()
                    {
                    	vector<int> arr = { 1, 2, 3, 4, 5 };
                    	Average result;
                    	For_each(arr.begin(), arr.end(), result);　// 这边是 值传递
                    	cout << result.GetAverage() << endl; //　错误，会出现 除0现象
                    	return 0;
                    }
                    
                    解决方法:
                    
                        //这里的Average()相当于一个临时对象
                        Average result = For_each(arr.begin(), arr.end(), Average()); 
                        cout << result.GetAverage() << endl
            
        (4) 
            A.生成器(generator)是不用参数就可以调用的函数符(仿函数)。
            
            一元函数(unary function) 是用一个参数就可以调用的函数符。
            
            二元函数(binary function)是用两个参数就可以调用的函数符。
            
            例如：供给给for_each()的函数符应该是一个一元函数，因为它每次用于一个容器元素
            
            实现Vector所有元素求积的功能
            
                vector<int> arr = { 1, 2, 3, 4, 5 };
                int ans = For_each(arr.begin(), arr.end(), Mul<int>())　　//　传入2个参数,靠考虑到放回参数的类型
                
            如何在For_each中知道我的第三个参数F functor 这个仿函数的执行的返回值呢(也就是class Mul 的模板类型参数T)？ 
            所以我们在这里我们需要对Mul这个类中引入三个类型。分别是first_argument_type,second_argument_type,
            result_type:
            
            方法一:
        
                    template<class T>
                    class Mul
                    {
                        public:
                            T operator()(T a, T b)
                            {
                                return a*b;
                            }
                            typedef T first_argument_type, second_argument_type, result_type;
                    };
                            
                    template < class T, class F>
                    //因为在这里编译器不知道result_type是F的成员变量还是类型,所以要用关键字typename限定。
                    typename F::result_type For_each(T begin, T end, F functor) 
                    {
                        typename F::result_type ans = 1; 
                        for (T it = begin; it != end; it++)
                            ans = functor(*it, ans); 
                        return ans;
                    }
                    
                    
            方法二:
                    template<class T>
                    class Mul : binary_function<T,T,T>
                    {
                        public:
                            T operator()(T a, T b)
                            {
                                return a*b;
                            }
                    }; 
            
```

## 增减元素对迭代器的影响

```shell
    1.对于连续内存容器
        如vector、deque等, 增减元素均会使得当前之后的所有迭代器失效.因此，以删除元素为例：
        由于erase()总是指向被删除元素的下一个元素的有效迭代器,因此,
        可以利用该连续内存容器的成员erase()函数的返回值。
        常见的编程写法为：
        
            for(auto iter = myvec.begin(); iter != myvec.end())  //另外注意这里用 "!=" 而非 "<"
            {
                if(delete iter) //达到删除 iter的条件
                    iter = myvec.erase(iter);
                else 
                    ++iter;
            }
            
        注意:
            当增加元素后整个vector的大小超过了预设,这时会导致vector重新分分配内存,效率极低.
            因此习惯的编程方法为：
            在声明了一个vector后,立即调用reserve函数,令vector可以动态扩容.
            通常vector是按照之前大小的2倍来增长的
            
    2.对于非连续内存容器
        如set、map等.增减元素只会使得当前迭代器无效,仍以删除元素为例,由于删除元素后,erase()返回的迭代器将是无效的迭代器。
        因此，需要在调用erase()之前，就使得迭代器指向删除元素的下一个元素。常见的编程写法为：
        
            for(auto iter = myset.begin(); iter != myset.end())  //另外注意这里用 "!=" 而非 "<"
                {
                    if(delete iter)
                        myset.erase(iter++);  //使用一个后置自增就OK了
                    else 
                        ++iter;
                }
```