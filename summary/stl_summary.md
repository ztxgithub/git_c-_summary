# C++ stl 

## 概要

```shell
    1. 智能指针(share_ptr)也算是 stl
    2. stl 容器提供都是"值"语义, 并不是"引用"语义.
    3. 通常 stl 不会抛出异常, 需要使用者传入正确参数.

```

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

                       
        (3) find 算法和 set 的 insert 成员函数是很多必须 判断两个值是否相同的函数的代表.但它们以不同的方式完成,
            find对“相同”的定义是 "相等",基于 operator==
            set::insert对“相同”的定义是"等价",通常基于 operator<
            
            
    2.仿函数
        (1) 仿函数(functor),就是使一个类的使用看上去像一个函数.其实现就是类中实现一个 operator(),
            这个类就有了类似函数的行为,就是一个仿函数类
            
            STL 不使用函数指针而使用仿函数呢,因为函数指针不能满足STL对抽象性的要求,
            无法和 STL 的其他组件搭配以产生更加灵活的效果
            
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
            
            实现 Vector 所有元素求积的功能
            
                vector<int> arr = { 1, 2, 3, 4, 5 };
                int ans = For_each(arr.begin(), arr.end(), Mul<int>())　　//　传入2个参数,靠考虑到放回参数的类型
                
            如何在 For_each 中知道我的第三个参数 F functor 这个仿函数的执行的返回值呢(也就是class Mul 的模板类型参数T)？ 
            所以我们在这里我们需要对 Mul 这个类中引入三个类型。分别是 first_argument_type, second_argument_type,
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
                    //因为在这里编译器不知道 result_type 是 F 的成员变量还是类型,所以要用关键字 typename 限定。
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
        如 vector、deque 等, 增减元素均会使得当前之后的所有迭代器失效.因此，以删除元素为例：
        由于 erase() 总是指向被删除元素的下一个元素的有效迭代器,因此,
        可以利用该连续内存容器的成员 erase() 函数的返回值。
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
            在声明了一个 vector 后,立即调用 reserve 函数,令 vector 可以动态扩容.
            通常 vector 是按照之前大小的 2 倍来增长的
            
    2.对于非连续内存容器
        如 set、map 等.增减元素只会使得当前迭代器无效,仍以删除元素为例,由于删除元素后,erase()返回的迭代器将是无效的迭代器。
        因此，需要在调用 erase() 之前，就使得迭代器指向删除元素的下一个元素。常见的编程写法为：
        
            for(auto iter = myset.begin(); iter != myset.end())  //另外注意这里用 "!=" 而非 "<"
                {
                    if(delete iter)
                        myset.erase(iter++);  //使用一个后置自增就OK了
                    else 
                        ++iter;
                }
```

## 容器

### string

```shell
    1. string() 初始化
            (1) string(int n, char c)  // 使用 n 个字符 c 进行初始化
            (2) string (const char* s, size_t n);  // 从起始地址开始取 n 个字节
            (3) string (const string& str, size_t pos, size_t len = npos); 截取 str 字符串进行拷贝
                    pos : 从 0 开始
                    len: 可以缺省,指的是从 pos 一直到最后
                    
    2. string 的赋值
            (1) string& assign (const string& str, size_t subpos, size_t sublen);
                    subpos : 从 0 开始
                    sublen: string::npos 指的是从 subpos 一直到最后
                    
    3. [] 与 at 区别
            string s = "123456";
            s[5];  // 这个越界直接段错误
            s.at(5)  // 这个越界可以抛异常,通过 try-catch 来捕获
            
    4. 字符串拼接
            (1) string& append (const string& str, size_t subpos, size_t sublen); 同上
            (2) string& append (const char* s, size_t n);
            
    5. 查找
            (1) size_t  string::find_first_of(const string& compare_str, size_t pos = 0): 这个重点是 src 中字符
                 pos : 代表源 str 从 pos 开始进行
                查找字符串中第一个字符都不匹配输入参数 compare_str 中任何一个字符
                
            (2) size_t find_first_of (const string& str, size_t pos = 0) : 这个重点是 src 中字符
                 pos : 代表源 str 从 pos 开始进行
                 查找字符串中第一个字符都匹配输入参数 compare_str 中任何一个字符
                 
            (3) size_t find (const string& str, size_t pos = 0) : 这个重点是 src 中字符串
                pos : 代表源 str 从 pos 开始查找
                
    6. 替换
            (1) string& replace (size_t pos, size_t len, const string& str);
                pos, len 用来确定替换范围, 将 src 从 pos 开始 len 个字符替换为 str, 最终字符串长度可能减少也可能增加.
                
    7. 子串
            (1) string substr (size_t pos = 0, size_t len = npos) const;
                对 src_string 从 pos 开始 len 长度作为子串.
                    
```

### vector

```shell
    1. vector 是单口容器, 通过 push_back 将元素插入到尾部，防止之前元素移动位置导致降低效率, 提供迭代器
    2. void vector::pop_back(); 将尾部最后一个元素删除
    3. vector 容器类似与[栈]
    4. vector 初始化定义
            (1) vector.capacity(): 实际分配物理空间, 大于等于 vector.size()
            (2) vector.size() : 真实保存有效数据个数
    5. void vector::swap (vector& x);  // 2 个 vector 的互换
       swap 技巧: 将 vector.capacity() 置为　0
                    　vector <int>().swap(vecInt);  // 其中 vector <int>() 是定义临时对象再通过 swap 跟目标 vector 互换.
    6. 
        void vector::resize (size_type n); 
        只是重新调整 vector 中的有效值的个数, 如果调整的 n 要大于原来的, 则有效个数会增加 vector.size() 会增加.
        vector.capacity() 可能会增加, 其余增加的会按默认值初始化.
        
        void vector::resize (size_type n, const value_type& val);
        区别是其余增加的会按 val 进行填充.
       
        void vector::reserve (size_type n);  用于增加 vector.capacity() 个数, 不会影响有效数据个数 vector.size().
    7. inser 可以在某一个位置插入, 其余元素后移. 也可以插入一个范围的迭代器.
    8. vector 实现动态增长
```

### deque 

```shell
    1. deque(double-ended queue): 实现方式是链表的形式(以连续物理空间基础), 可以轻松实现两端的插入删除, 提供迭代器
    2. deque 的初始化
    3. deque 的赋值操作, assign 操作, swap 操作, =
    4. deque 的大小操作, deque.size() 有效个数 , 没有 capacity 接口
    5. deque 双端插入删除, push_back, push_front, pop_back, pop_front.
    6. deque 数据存取, deque.at(), [], deque.front(). deque.back()
    7. 实际应用
            (1) 去除最高分，去除最低分的操作，可以用 deque 的 pop_front, pop_back
```

### stack
```shell
    0. 规则是先进后出, 由于规则限制, 该类不提供迭代器
    1. void stack::pop();  从栈顶 pop 元素
    2. void stack::push();  从栈顶 push 元素
    3. void stack::top();  取栈顶元素
```

### queue
```shell
    1. 规则是先进先出, 由于规则限制, 该类不提供迭代器
    2. void queue::pop();  从队头 pop 元素
    3. void queue::push();  从队尾 push 元素
    4. void queue::front();  取队头元素
    5. void queue::back();  取队尾元素
```

### list
```shell
    1. list 容器：双向链表(有数据域和指针域), 方便插入删除, 提供迭代器
    2. list 可以从尾部删除(pop_back()), 插入(push_back(elem)), 也可以从头部插入(push_front()), 删除(pop_front())
    3. list::remove(elem) : 删除容器中所有与 elem 值匹配的元素
    4. list::reverse() : list 元素翻转
    5. (1) list::sort() : 默认排序从小到大
       (2) template <class Compare>
             void sort (Compare comp);
             
            也可以指定比较函数
            bool mycompare(int v1, int v2)
            {
                return v1 > v2;   // 从大到小, 含义是当为 true 时才进行交互, 所以本来 v1 在前, v2 在后, 当 v1 > v2 时, 进行交互
            }
```

### set/multiset
```shell
    0. set 和 multiset 结构体的头文件都是 #include<set>
    1. set/multiset 的特性是所有元素会根据元素的值自动进行排序, 遍历的时候默认从小到大，
    　　set 是红黑树为底层机制, 其查找效率高. set 容器不允许重复元素,　multiset 允许重复元素.
    2. size_type set::count (const value_type& val) const; 返回等于 val 的个数, 可以间接看看该 val 是否存在该 set
    3. set 查找操作
            (1) iterator set::find (const value_type& val)
            (2) iterator set::lower_bound (const value_type& val); 
                    返回第一个 key >= val 元素的迭代器, 这里有个前提条件是 set 使用默认的 comparison type (less) 从小到大
            (3)  iterator set::upper_bound (const value_type& val);
                    返回第一个 key > val 元素的迭代器
            (4) pair<const_iterator,const_iterator> equal_range (const value_type& val) const;
                    返回的是　pair::first is the lower bound of the range (the same as lower_bound),
                     and pair::second is the upper bound (the same as upper_bound).
                     
                     auto pos =  myset.equal_range(5)
                     if(pos == myset.end()) // 没有找到
    4. 
        
        (1) 使用 stl 的内置函数对象
        #include <function>  // 预定义函数对象
        #include <set>
        using namespace std;
        
            set<int, greater<int> > iVector;  // 从大到小, 第二个参数是函数对象, 每插入一个就会调用 greter 的函数对象用来插入顺序条件
        //    set<int, less<int> > iVector;  // 从小到大
        
        (2) 自定义函数对象(首先是一个类, 里面重载 operator() 方法, 调用 operator() 相当于函数调用, 函数对象又叫仿函数)
                class GreaterSimu
                {
                public:
                    bool operator()(int v1, int v2)
                    {
                        return v1 > v2;
                    }
                };
                
                set<int, GreaterSimu> iVector;  // 从大到小
                
        (3) 如果 set 中的元素是自定义的类型, 注意默认情况是 int/float 基本类型，编译器无法识别 class 内该如果用哪个成员变量排序,
            所以得 set<Teacher, 函数对象>, 自定义一个函数对象. 函数对象里面的 operator() 排序形式非常重要, set::find() 也会根据
            这个进行查找
                    class Teacher{
                    public:
                        Teacher(int tId, int tAge):id(tId), age(tAge){}
                        int id;
                        int age;
                    };
                    
                    class CmpTeacher
                    {
                    public:
                        bool operator()(Teacher v1, Teacher v2)
                        {
                            return v1.id > v2.id;
                        }
                    };
                    
                    
                    set<Teacher, CmpTeacher> teachers;  // 从大到小
                    teachers.insert(Teacher(1, 2));
                    // 因为是按照　CmpTeacher　函数对象,　Teacher.id 进行排序的,所以可以查找到
                    auto iterTeacher = teachers.find(Teacher(1, 3));  
                
```

## map/multimap
```shell
    1. 对组 pair<xxxx,xxxx>
        pair<string, int> mypair("abc", 123);
        cout << mypair.first << endl;
        
        auto mypair2 = make_pair("abc", 234);
    2.map 中所有元素根据键值(key) 自动排序, map 以红黑树为底层实现机制. map 中的键值(key)不允许重复
```

## 容器总结

```shell
    1. vector 和 deque　都是一次性分配内存,　可随机存取. vector.at() 比 deque.at() 效率高, 例如 vector.at(0) 是固定的,
        deque 的开始位置却是不固定.
        
```

## 算法

```shell
    1. 重载函数调用操作符() 的类, 其对象称为函数对象(function object), 是行为类似函数的对象,也叫仿函数(functor),使得类对象可以像
    　　函数那样调用.
    2. 一元, 二元函数对象和一元二元谓词
            (1) 一元函数对象：　写的函数对象接受一个参数, 没有返回值
                class Print{
                public:
                    void operator()(int v)
                    {
                        cout << v << endl;
                    }
                };
                
                void funPrint(int v))
                {
                    cout << v << endl;
                }
                // for_each 第三个参数是"一元函数对象"
                for_each(vector.begin(). vector.end(), Print()); // 其中 Print() 代表匿名一元函数对象
                  for_each(vector.begin(). vector.end(), funPrint); // 可以放置一个参数的普通函数
            (2) 二元函数对象：　写的函数对象接受两个参数.
                    class PlusDef{
                    public:
                        int operator()(int src1, int src2)
                        {
                            return src1 + src2;
                        }
                    };
                    // transform 将容器 vectorSrc1 和 vectorSrc2 依次将对应的迭代器相加再放到对应的目标 vectorDst 容器中.
                    // 这里注意 vectorDst 要提前申请好空间(vectorDst.resize()),不然会报错.
                    transform(vectorSrc1.begin(), vectorSrc1.end(), vectorSrc2.begin(), vectorDst.begin(), PlusDef());
            (3) 一元谓词: 写的函数对象或则普通函数接受一个参数, 并且放回值是 Bool
                // 一元谓词
                 class CompareDef{
                  public:
                    bool operator()(int v)
                    {
                        if(v > 0)
                            return true;
                        else
                            return false;
                    }
                  };
                
                    // find_if 第三个参数是一元谓词, 当一元谓词返回值为 true 时, find_if 停止遍历
                    vector<int>::iterator it = find_if(vector.begin(), vector.end(), CompareDef())  // CompareDef() 代表匿名一元谓词对象
                    if(it != vector.end())
                    {
                        .....
                    }
                    else // 找到了
                    {
                        cout << "*pos" <<  endl;
                    }
                
            (4) 二元谓词: 写的函数对象或则普通函数接受两个参数, 并且放回值是 Bool
                  class mycompare
                  {
                     bool operator()(int v1, int v2) // 从大到小
                    {
                        return v1 > v2;   // 含义是当为 true 时才进行交互, 所以本来 v1 在前, v2 在后, 当 v1 > v2 时, 进行交互
                    }
                  }
       
                  sort(vector.begin(), vector.end(), mycompare());
                  
    3. 内建函数对象
            (1) stl 内建一些函数对象, 分为算数类函数对象, 关系运算类函数对象, 逻辑运算类仿函数.使用内建函数对象，
                需要引入#include<functional>
                
            (2) 算数类函数对象
                    template<class T> T plus<T>   // 加法仿函数
                        plus<int> myplus;
                        int sum =  myplus(10, 20);
                        int sum  =  plus<int>()(10, 20);
                        
                    template<class T> T minute<T>       // 减法仿函数
                    template<class T> T multiplies<T>   // 乘法仿函数
                    template<class T> T divides<T>      // 除法仿函数
                    template<class T> T modulus<T>      // 取模仿函数
                    
                    
    4. 函数对象适配器
            (1) 函数对象适配器是完成一些配接工作, 包括绑定(bind), 否定(negate), 以及对一般函数或成员函数的修饰，
            　　　使其成为函数对象, 例如:
                        a. bind1st: 将参数绑定为函数对象的第一个参数
                        b. bind2nd: 将参数绑定为函数对象的第二个参数
                        c. not1: 对一元函数对象取反
                        d. not2: 对二元函数对象取反
                        e. ptr_fun: 将普通函数修饰成函数对象
                        f. mem_fun: 修饰成员函数
                        g. mem_fun_ref: 修饰成员函数
                        
                        
                  实例代码:
                  　　　I. 
                        /*
                         * 如果需要外部再传入一个参数 a, 使得遍历 vector 的元素大于 a 时, 才打印
                         * 第一步: 自定义函数对象去继承父类 binary_function(2 个参数) unary_function(1 个参数)
                         *        其中　binary_function<int, int, void>　第一个 int 代表 int v1, 第二个 int 代表 int v2, 第三个 void 代表返回值
                         * */
                        class printEx1:public binary_function<int, int, void>{
                        public:
                            void operator()(int v1, int v2) const  // 这里一定要加上 const
                            {
                                if(v1 > v2)
                                {
                                    cout << " v1 " << v1 << endl;
                                }
                        
                            }
                        };
                        
                            /*
                             * bind1st, bind2nd 调用后, 返回值变为一元函数对象.
                             * 
                             * */
                            printEx1 pEx;
                            // bind1st 把输入的参数绑到第一个位置 v1 
                            for_each(iVecs.begin(), iVecs.end(), bind1st(pEx, 10));
                            // bind2nd 把输入的参数绑到第二个位置 v2 
                            for_each(iVecs.begin(), iVecs.end(), bind2nd(pEx, 3));
                            
                        II.
                        
                            /*
                             * not1 是一元函数对象取反
                             * not2 是二元函数对象取反
                             * 其中 unary_function 第一个参数 int 是 重载操作符第一个参数 int i, 第二个参数是返回值. 
                             * */
                            class compareDefNot:public unary_function<int, bool>{
                            public:
                                bool operator()(int i) const 
                                {
                                    return i > 5;
                                }
                            };
                            
                            /*
                             * 找到第一个小于等于 5 
                             * */
                            auto itor = find_if(v.begin(), v.end(), not1(compareDefNot()));
                            if(itor != v.end())
                            {
                                cout << "find_if true " << *itor << endl;
                            } else{
                                cout << "find_if not true " << endl;
                            }
```