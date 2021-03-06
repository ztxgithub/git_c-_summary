# C++ 智能指针

## 简介

```shell
    1.指针问题
        (1) 挂起引用(dangling references)
                如果一块内存被多个指针引用,但其中的一个指针释放且其余的指针并不知道,这样的情况下,就发生了挂起引用
                
    2.智能指针
        智能指针是一个RAII(Resource Acquisition is initialization）类模型,用来动态的分配内存.
        它提供所有普通指针提供的接口,在构造中,它分配内存,当离开作用域时,它会自动释放已分配的内存.
        
    3.智能指针就是一个类,当超出了类的作用域是,类会自动调用析构函数,析构函数会自动释放资源
    
    4. get_pointer(share_ptr<T>) : 获取智能指针的 raw 指针(T *)
            传入参数可以是　share_ptr<T>, scoped_ptr<T>

```

## auto_ptr (C++98) (不推荐使用)

```c++
    1.当p离开作用域时,它所指向的内存块也会被自动释放.
        class Test
        {
            public: 
            Test(int a = 0 ) : m_a(a) { }
            ~Test( )
            { 
               cout << "Calling destructor" << endl; 
            }
            public: int m_a;
        };
        void main( )
        { 
            std::auto_ptr<Test> p( new Test(5) ); 
            cout << p->m_a << endl;
        }
        
    2.异常被抛出,对应的智能指针正确地被释放,因为当异常抛出时,栈松绑（stack unwinding）,当try 块中的所有对象destroy后,
       p 离开了该作用域，所以它绑定的内存也就释放
        例如：
            class Test
            {
                public:
                 Test(int a = 0 ) : m_a(a)
                 {
                 }
                 ~Test( )
                 {
                  cout<<"Calling destructor"<<endl;
                 }
                public:
                 int m_a;
            };
            //***************************************************************
            void Fun( )
            {
             int a = 0, b= 5, c;
             if( a ==0 )
             {
              throw "Invalid divisor";
             }
             c = b/a;
             return;
            }
            //***************************************************************
            void main( )
            {
                 try
                 {
                  std::auto_ptr<Test> p( new Test(5) ); 
                  Fun( );
                  cout<<p->m_a<<endl;
                 }
                 catch(...)
                 {
                  cout<<"Something has gone wrong"<<endl;
                 }
            }
            
    问题:
    
    1. 在函数间传递auto_ptr时,它的所有权(ownship)也转移,例如在　Foo()中有一个auto_ptr,然后在Foo()中我把指针传递给了
       Fun()函数,当Fun()函数执行完毕时,指针的所有权不会再返还给Foo(即执行完Fun()后其auto_ptr对应的内存就会被释放),所以
       在Fun()函数内继续引用auto_ptr时会出现段错误.
       
            class Test
            {
                public:
                 Test(int a = 0 ) : m_a(a)
                 {
                 }
                 ~Test( )
                 {
                  cout<<"Calling destructor"<<endl;
                 }
                public:
                 int m_a;
            };
             
             
            //***************************************************************
            void Fun(auto_ptr<Test> p1 )
            {
                 cout<<p1->m_a<<endl;
            }
            //***************************************************************
            void main( )
            {
             std::auto_ptr<Test> p( new Test(5) ); 
             Fun(p);  //执行完Fun函数后p指向的内存会被释放
             cout<<p->m_a<<endl;　　//再引用已经释放的指针会出现段错误
            }       
                   
    2.auto_ptr只能指向一个实例对象,而不能指向一组对象，就是说它不能和操作符new[]一起使用,当auto_ptr离开作用域时,默认是调用
    　delete auto_ptr, 但无法调用 delete [] auto_ptr
            void main( )
            {
                std::auto_ptr<Test> p(new Test[5]);　　//错误,有问题
            }
            
    3.auto_ptr不能和标准容器（vector,list,map....)一起使用
    4.    
          auto_ptr<Test> ptest(new Test("123"));
          auto_ptr<Test> ptest2(new Test("456"));
          ptest2 = ptest;
          ptest2->print();
          if(ptest.get() == NULL)cout<<"ptest = NULL\n";
          
          当我们对智能指针进行赋值时,如ptest2 = ptest,ptest2会接管ptest原来的内存管理权,
          ptest的原始指针变为空指针(ptes.get()为NULL),
          如果ptest2原来的原始指针不为空,则它会释放原来的资源,基于这个原因,应该避免把auto_ptr放到容器中,因为算法对容器操作时,
          很难避免STL内部对容器实现了赋值传递操作,这样会使容器中很多元素被置为NULL.
          判断一个智能指针是否为空不能使用if(ptest == NULL),应该使用if(ptest.get() == NULL)
```

## shared_ptr（引用计数型智能指针）

```shell
    1.shared_ptr：　shared_ptr 控制对象的生命期
                 　多个指针可以同时指向一个对象,当最后一个shared_ptr离开作用域时,内存才会自动释放
    2. shared_ptr创建
            (1) shared_ptr<int> sptr1( new int );
            (2) make_shared宏来创建(推荐使用)
                    传递给make_shared函数的参数必须和shared_ptr所指向类型的某个构造函数相匹配
                    shared_ptr<int> sptr1 = make_shared<int>(100);
            (3) 
                sp3.reset(new int(3));// 通过reset重置内部数据指针
                sp3.reset();// 通过reset重置内部数据指针为空
                    
    3.shared_ptr默认调用delete释放资源,如果shared_ptr指向一组对象,则需要调用delete[]来销毁这个数组.
    　用户可以通过调用一个函数,例如一个lamda表达式,来指定一个通用的释放步骤.
            shared_ptr<Test> sptr1( new Test[5], 
                    [ ](Test* p) { delete[ ] p; } );
                    
    4.接口
        (1) get(): 返回智能指针的内部数据的指针(获取shared_ptr绑定的资源)
        (2) reset(): 释放关联内存块的所有权，如果是最后一个指向该资源的shared_ptr,就释放这块内存。
        (3) unique: 判断是否是唯一指向当前内存的shared_ptr.
        (4) operator bool : 判断当前的shared_ptr是否指向一个内存块，可以用if 表达式判断.
        (5) use_count() : 对同一资源引用的个数,可能效率很低，应该只把它用于测试或调试
        (6) release() : 只释放所有权,而不对资源内存进行释放
        
    5.问题
        问题1: 从同一裸指针(naked pointer)创建多个shared_ptr,导致多次释放内存
            (1) 正常的代码,所有的shared_ptrs拥有相同的引用计数，属于相同的组
                     shared_ptr<int> sptr1( new int );
                     shared_ptr<int> sptr2 = sptr1;
                     shared_ptr<int> sptr3;
                     sptr3 =sptr1
                     
            (2) 不正常的代码,两个来自不同组的shared_ptr指向同一个资源,所以当sptr1离开作用域时,会释放对应的内存,而当sptr2也离开
            　　　作用域时,同样释放内存(而此时内存已经被sptr1释放掉了)，尽量不要从一个裸指针(naked pointer)创建shared_ptr
                     int* p = new int;
                     shared_ptr<int> sptr1( p);
                     shared_ptr<int> sptr2( p );
                     
        问题2: 循环引用
        
        　　class B;
            class A
            {
                public:
                 A(  ) : m_sptrB(nullptr) { };
                 ~A( )
                 {
                  cout<<" A is destroyed"<<endl;
                 }
                 shared_ptr<B> m_sptrB;
            };
            
            class B
            {
                public:
                 B(  ) : m_sptrA(nullptr) { };
                 ~B( )
                 {
                  cout<<" B is destroyed"<<endl;
                 }
                 shared_ptr<A> m_sptrA;
            };
            
            //***********************************************************
            void main( )
            {
                 shared_ptr<B> sptrB( new B );　// share_ptrB->ref_count = 1
                 shared_ptr<A> sptrA( new A );  // share_ptrA->ref_count = 1
                 sptrB->m_sptrA = sptrA;        // share_ptrA->ref_count = 2
                 sptrA->m_sptrB = sptrB;        // share_ptrB->ref_count = 2
            }
            
        这个时候如果sptrB离开了作用域,则 share_ptrB->ref_count减少为1,sptrA离开了作用域,则 share_ptrA->ref_count减少为1,
        它们指向的资源并没有释放
        
        问题3: 意外的延长对象的生命周期
                (1) 如果有变量类似 vector<shared_ptr<T> > 看是不是可以考虑用 vector<weak_ptr<T> > 防止没有用的资源一直得不到
                　　 释放
                (2) boost::bind 会把 shared_ptr 类型的实参拷贝一份，对象的生命期不会短于 boost::function 对象
                        class Foo
                        {
                            void do_somethind();
                        };
                        
                        shared_ptr<Foo> pFoo(new Foo());
                        boost:function<void()> func = boost:bind(&Foo::do_somethind, pFoo);
        
    6.适用场景
        (1) 一个对象同时被多个线程使用，如何保证对象的安全使用(调用析构函数时确保该对象不被任何线程使用，使用该对象时还没有调用
        　　析构函数)，可以使用 shared_ptr
        
    7.注意
        (1) 如果多个线程要对同一个 shared_ptr 对象进行读写，需要加锁
        (2) shared_ptr 的拷贝开销要比拷贝原始指针要大，不过一般情况下都是以常引用(const reference),所以不存在
            反复拷贝导致性能问题．
            
                void save(const shared_ptr<Foo> &pFoo);
                bool validate(const shared_ptr<Foo> &pFoo);
                
                void onMessage(const string &msg)
                {
                	shared_ptr<Foo> pFoo(new Foo(msg));
                	if(validate(pFoo))  //没有拷贝 pFoo
                	{
                		save(pFoo);     //没有拷贝 pFoo
                	}
                }
```

### enable_shared_from_this 

```shell
    1. 防止在将对象的 this 指针作为返回值返回给了调用者时,无法判断该对象是否已经被析构掉，引用该对象时出现段错误
       A.错误的代码:            
            class Bad {
            public:
                Bad() { std::cout << "Bad()" << std::endl; }
                ~Bad() { std::cout << "~Bad()" << std::endl; }
                std::shared_ptr<Bad> getPtr() {
                    return std::shared_ptr<Bad>(this);
                }
            };
            
            int main(int argc, char const *argv[])
            {
                std::shared_ptr<Bad> bp1(new Bad);
                /*
                 * 这里正确的方法为　bp2 = bp1
                 * 而使用　bp2 = bp1->getPtr()　使得智能指针 bp2 认为引用该资源就它一个
                 * 导致重复释放了 2 次资源，析构 bp1 和析构 bp2
                 */
                std::shared_ptr<Bad> bp2 = bp1->getPtr();
                std::cout << "bp2.use_count: " << bp2.use_count() << std::endl;  // bp2.use_count: 1
                return 0;
            }
            
       B. 解决方法： 使用 enable_shared_from_this, 将this指针就能变成一个 shared_ptr 智能指针对象
       
                class Good: public std::enable_shared_from_this<Good>
                    {
                        public:
                            Good() { std::cout << "Good()" << std::endl; }
                            ~Good() { std::cout << "~Good()" << std::endl; }
                        
                            std::shared_ptr<Good> getPtr() {
                                return shared_from_this(); 
                            }
                    };
                
                int main(int argc, char const *argv[])
                {
                    std::shared_ptr<Good> bp1(new Good);
                    std::shared_ptr<Good> bp2 = bp1->getPtr();
                    std::cout << "bp2.use_count: " << bp2.use_count() << std::endl;
                    return 0;
                }
                
                结果:
                    Good()
                    bp2.use_count: 2
                    ~Good()
                    
    2.正确的使用　shared_from_this(), 对象不能是 stack object , 必须是 heap object 且由 shared_ptr 管理
    　其生命期

```

## weak_ptr

```shell
    1.为了解决循环引用,C++提供了weak_ptr智能指针
        (1) weak_ptr没有重载*、->操作符，所以不能通过*、->操作符操作智能指针的内部数据
    2.创建
        以shared_ptr作为参数构造weak_ptr.　从shared_ptr创建一个weak_ptr增加了共享指针的弱引用计数(weak reference)，
        意味着 weak_ptr 可以知道对象是否被释放.但是当shared_ptr离开作用域时,这个计数(weak reference)不作为
        是否释放资源的依据.也就是除非强引用计数变为0，才会释放掉指针指向的资源,弱引用计数(weak reference)不起作用.
        
             shared_ptr<Test> sptr( new Test );
             weak_ptr<Test> wptr( sptr );
             weak_ptr<Test> wptr1 = wptr;
             
    3.将一个weak_ptr赋给另一个weak_ptr会增加弱引用计数(weak reference count)
    4.当shared_ptr离开作用域时,其内的资源释放了,这时候指向该shared_ptr的weak_ptr失效了(expired)
    5.判断weak_ptr是否指向有效资源,有两种方法：
        (1) 调用use-count()去获取引用计数,该方法只返回强引用计数,并不返回弱引用计数
        (2) 调用expired()方法(查看引用的内存资源是否被释放),比调用use_count()方法速度更快
        
    6.从weak_ptr调用lock()可以得到shared_ptr或者直接将weak_ptr转型为shared_ptr
    　 lock() 是线程安全的
             shared_ptr<Test> sptr( new Test );
             weak_ptr<Test> wptr( sptr );
             shared_ptr<Test> sptr2 = wptr.lock( );  //这样强引用计数加1，
                                                     // 可以判断　sptr2　是否为 NULL 来看对应的资源是否释放
             
    7.weak_ptr解决循环引用问题(根据weak_ptr的弱引用计数不作为是否释放资源的依据)
            class B;
            class A
            {
                public:
                 A(  ) : m_a(5)  { };
                 ~A( )
                 {
                  cout<<" A is destroyed"<<endl;
                 }
                 void PrintSpB( );
                 weak_ptr<B> m_sptrB;
                 int m_a;
            };
            
            class B
            {
                public:
                 B(  ) : m_b(10) { };
                 ~B( )
                 {
                  cout<<" B is destroyed"<<endl;
                 }
                 weak_ptr<A> m_sptrA;
                 int m_b;
            };
            
            void A::PrintSpB( )
            {
                 if( !m_sptrB.expired() )
                 {  
                  cout<< m_sptrB.lock( )->m_b<<endl;
                 }
            }
            
            void main( )
            {
                 shared_ptr<B> sptrB( new B );　　// share_ptrB->ref_count = 1
                 shared_ptr<A> sptrA( new A );   // share_ptrA->ref_count = 1
                 sptrB->m_sptrA = sptrA;         //  weak_ptrA->ref_count = 1
                 sptrA->m_sptrB = sptrB;         //  weak_ptrB->ref_count = 1
                 sptrA->PrintSpB( ); 
            }
            
            当share_ptrB离开作用域时,强引用B的个数减少为0,弱引用B的个数为1(weak_ptr的弱引用计数不作为是否释放资源的依据),
            释放share_ptrB执行的内存
       
```

## unique_ptr (独占使用资源)

```shell
    1.unique_ptr是取代c++98的auto_ptr的产物,unique_ptr遵循着独占语义.在任何时间点,资源只能唯一地被一个unique_ptr占有.
      (只能有一个智能指针对象指向某块内存)
      unique_ptr不支持复制,但是支持通过move转移内部指针
    　当unique_ptr离开作用域，所包含的资源被释放
    2.创建
        
        (1) unique_ptr 不像 shared_ptr 一样拥有标准库函数 make_shared 来创建一个 shared_ptr 实例。
            要想创建一个 unique_ptr，我们需要将一个 new 操作符返回的指针传递给 unique_ptr 的构造函数
                unique_ptr<int> up(new int(5));
                cout << *pInt;
        (2) unique_ptr提供了创建数组对象的特殊方法,当指针离开作用域时，调用delete[]代替delete
                unique_ptr<int[ ]> uptr( new int[5] );
                
    3.接口
    　　
      (1) unique_ptr<T> up 
            空的 unique_ptr，可以指向类型为T的对象，默认使用delete来释放内存
      
      (2) up = nullptr 
            释放up指向的对象，将up置为空
      
      (3) up.release() 
            up放弃对它所指对象的控制权，并返回保存的指针，将up置为空，不会释放内存
      
      (4) up.reset() 或则　
            参数可以为 空、内置指针，先将up所指对象释放，然后重置up的值.
            
        unique_ptr 提供一个release()的方法,释放所有权.
        release和reset的区别在于,release仅仅释放所有权但不释放资源,reset也释放资源
        
        unique_ptr不允许两个独占指针指向同一个对象，在没有裸指针的情况下，
        我们只能用release获取内存的地址，同时放弃对对象的所有权，这样就有效避免了多个独占指针同时指向一个对象。
        
    4.unique_ptr 可以实现如下功能：
        (1) 为动态申请的内存提供异常安全
        (2) 将动态申请的内存所有权传递给某函数
        (3) 从某个函数返回　动态申请内存的所有权
        (4) 在容器中保存指针
            
    5. unique_ptr 的特性
       (1) 无法进行复制构造与赋值操作
                 unique_ptr<int> ptr(new int(88));
                 unique_ptr<int> ptr_copy(ptr) ; // 会出错, unique_ptr 无法进行拷贝构造函数　
                 unique_ptr<int> ptr_assign = ptr; //会出错　unique_ptr 无法进行赋值操作
                 
       (2) 可以进行移动构造和移动赋值操作
                A. 移动构造(在函数中作为返回值)
                        unique_ptr<int> GetVal()
                        {
                            unique_ptr<int> ptr_move(new int(88 );
                            return ptr_move;
                        }
                        
                        /*
                         * 编译器可以识别进行移动构造函数，达到指针的转移
                         */
                        unique_ptr<int> Ptr = GetVal();   //ok
                        
                B. 移动赋值操作
                
                        unique_ptr<int> ptr(new int(88));
                        
                        //这里是显式的所有权转移. 把 ptr 所指的内存转给 ptr_move ,而 ptr 不再拥有该内存
                        unique_ptr<int> ptr_move = std:move(ptr); 
                        
       (3) 可做为容器元素
                auto_ptr不可做为容器元素,而unique_ptr也同样不能"直接"做为容器元素,
                但可以通过一点间接的手段
                
                unique_ptr<int> sp(new int(88));
                
                vector<unique_ptr<int> > vec;
                
                vec.push_back(std::move(sp));
                
                //vec.push_back( sp ); 这样不行,会报错的.
 
                //cout<<*sp<<endl;但这个也同样出错,说明sp添加到容器中之后,它自身报废了.
                
    6.
            unique_ptr<Test> ptest(new Test("123"));
            unique_ptr<Test> ptest2(new Test("456"));
            ptest->print();
            ptest2 = std::move(ptest);//不能直接ptest2 = ptest, unique_ptr指针 无法进行赋值操作
            if(ptest == NULL)cout<<"ptest = NULL\n";
            Test* p = ptest2.release();
            p->print();
            ptest.reset(p);
            ptest->print();
            ptest2 = fun(); //这里可以用=，因为使用了移动构造函数
            ptest2->print();
            
            unique_ptr 和 auto_ptr用法很相似，不过不能使用两个智能指针赋值操作,应该使用std::move; 
            而且它可以直接用if(ptest == NULL)来判断是否空指针；release、get、reset等用法也和auto_ptr一致,
            使用函数的返回值赋值时,可以直接使用=, 这里使用c++11 的移动语义特性。
            另外把它当做参数传递给函数时,传实参时也要使用std::move,比如foo(std::move(ptest)).
            它还增加了一个成员函数swap用于交换两个智能指针的值
            
            //创建一个指向int的空指针
            std::unique_ptr<int> fPtr1;
            std::unique_ptr<int> fPtr2(new int(4));
            auto fPtr3 = std::make_unique<int>();
            
            //fPtr2释放指向对象的所有权，并且被置为nullptr
            std::cout << "fPtr2 release before:" << fPtr2.get() << std::endl;
            int *pF = fPtr2.release();
            std::cout << "fPtr2 release before:" << fPtr2.get() << " and pF value:" << *pF << std::endl;
            结果:
                fPtr2 release before:00EFB120
                fPtr2 release before:00000000 and pF value:4
            
            //所有权转移，转移后fPtr3变为空指针
            std::cout << "move before fPtr1 address:" << fPtr1.get() << " fPtr3 address:" << fPtr3.get() << std::endl;
            fPtr1 = std::move(fPtr3);
            std::cout << "move after  fPtr1 address:" << fPtr1.get() << " fPtr3 address:" << fPtr3.get() << std::endl;
            结果:
                　move before fPtr1 address:00000000 fPtr3 address:00EFEC60
                　move after fPtr1 address:00EFEC60 fPtr3 address:00000000
            
            std::cout << "move before fPtr1 address:" << fPtr1.get() << std::endl;
            fPtr1.reset();
            std::cout << "move after  fPtr1 address:" << fPtr1.get() << std::endl;
            结果:
                move before fPtr1 address:00EFEC60
                move after fPtr1 address:00000000
                
    7. 总结
            unique_ptr和auto_ptr真的非常类似.其实你可以这样简单的理解,auto_ptr是可以说你随便赋值,
            但赋值完了之后原来的对象就不知不觉的报废.搞得你莫名其妙.而unique就干脆不让你可以随便去复制,赋值.
            如果实在想传个值就哪里,显式的说明内存转移std:move一下.然后这样传值完了之后,之前的对象也同样报废了.
            只不过整个move你让明显的知道这样操作后会导致之前的unique_ptr对象失效.

```

## scoped_ptr 

```shell
    1.boost::scoped_ptr的实现和std::auto_ptr非常类似,都是利用了一个栈上的对象去管理一个堆上的对象，
      从而使得堆上的对象随着栈上的对象销毁时自动删除。不同的是，boost::scoped_ptr有着更严格的使用限制——不能拷贝。
    　这就意味着　boost::scoped_ptr　指针是不能转换其所有权的．
    2.特点
        (1) 不能转换所有权
                boost::scoped_ptr所管理的对象生命周期仅仅局限于一个区间（该指针所在的"{}"之间），无法传到区间之外，
                这就意味着boost::scoped_ptr对象是不能作为函数的返回值的（std::auto_ptr可以）
                
        (2) 不能共享所有权
                这点和std::auto_ptr类似。这个特点一方面使得该指针简单易用。另一方面也造成了功能的薄弱——不能用于stl的容器中。
                
        (3) 不能用于管理数组对象
                由于boost::scoped_ptr是通过delete来删除所管理对象的，而数组对象必须通过deletep[]来删除，
                因此boost::scoped_ptr是不能管理数组对象的，如果要管理数组对象需要使用boost::scoped_array类。
                
    3.常用操作
        A. operator*() : 以引用的形式访问所管理的对象的成员
        B. operator->(): 以指针的形式访问所管理的对象的成员
        C. reset() : 释放所管理的对象，管理另外一个对象
        D. swap(scoped_ptr& b) : 交换两个boost::scoped_ptr管理的对象
        
    4.
        #include <boost/scoped_ptr.hpp>
        
            long * lp = new long;
            boost::scoped_ptr<long> sp ( lp );
            BOOST_TEST( sp.get() == lp );
            BOOST_TEST( lp == sp.get() );
            BOOST_TEST( &*sp == lp );
            sp.reset(NULL);
```

## 注意事项

```shell
    1. 在默认情况下，你最好使用unique_ptr
    2. 使用make_shared而不是裸指针来初始化共享指针
    3. 在你从共享指针(share_ptr)中获取对应的裸指针之前请仔细考虑清楚.你永远不知道别人什么时候会调用delete来删除这个裸指针,
       到那个时候你的共享指针(shared_ptr)就会出现Access Violate（非法访问）的错误。
    4.在对unique_ptr使用Release()方法后,记得一定要删除对应的裸指针.如果你是想要删掉unique_ptr指向的对象,
      可以使用unique_ptr.reset()方法
      
    5.调用weak_ptr.lock()的时候检查它的有效性(expired()方法)
```