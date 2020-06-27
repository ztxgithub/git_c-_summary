# C++ 设计模式

## UML 

```shell
    1. UML (United Modeling Language): 统一建模语言, 是一种基于面向对象的可视化建模语言
    2. 软件工程：对软件开发全过程进行建模和管理．　模型其实是对现实的简化.
    
```

## StartUML

```shell
    1. 在创建完 Model -> Use case Diagram(用例图), 左侧的图标可以双击产生一个锁，这个时候就可以不用每个都拖
    　　点击画板就可以了
```

## 用例图
```shell
    1. 用例图主要是给客户看的，较好了解到各个模块功能及需求，　从客户的角度来描述系统功能
    2. 用例图包含 3 个基本组件:
            (1) 参与者(Actor): 与本系统打交道的人或则其他其他系统(即使用该系统的人或则事物), UML 中是参与者用人形图标
            (2) 用例(Use Case): 代表系统的某项完整的功能．　在 UML 中使用椭圆表示
            (3) 关系：　定义用例之间的关系, 泛化关系, 扩展关系, 包含关系
                    a. 泛化关系：就是父子关系(功能的父子表现). 
                                UML 中用例泛化用一个三角箭头从子指向父(即子用例继承与父用例,或则子参与者继承父参与者 Generalization)
                                例如: 网上支付　Generalization-> 支付
                    b. 包含关系: 一个用例可以包含其他用例具有的行为,并把它包含的用例行为作为自身行为的一部分.
                                UML 中包含关系用虚线箭头加 “<<include>>”, 箭头指向被包含的用例
                                例如：　添加学生信息　Include　操作数据库
                    c. 扩展关系: 完成某一功能偶尔(有条件)会执行另外一个功能. 
                    　　　　　　　UML 中扩展关系⽤用虚线箭头加 “<<extend>>”, 箭头指向被扩展的⽤用例
                                例如: 
                                　　　老师在保存成绩时发现分数不及格，打印补考通知单
                                     保存成绩 Extend 补考通知单
```

## 类图(重要)
```shell
    1. 类图主要是用来显示系统中的类, 接口以及它们之间的关系.
       类图包含元素有类, 接口和关系. 其中关系有泛化关系,关联关系, 依赖关系和实现关系. 在类图中也可以包含注释和约束
    2. 类图是给开发人员看的.
    3. 类是类图的主要组件, 由 3 部分组成: 类名, 属性和方法
    4. 在 UML 中, 类用矩形来表示, 顶端区域存放类的名称, 中间区域存放类的属性, 属性的类型及值(+id: int), 底部区域存放类的方法, 
       方法的参数和返回(+getName(): string) 其中方法　: 后面是返回值
    5. 在 UML 中, Public 有 + 表示, private 用 – 表示, protect 用 # 表示
    6. 类之间的关系
            (1) 泛化关系
                    就是继承关系, 也称为 "is a kind of" 关系. 在类图中是派生类指向基类.　Generalization
            (2) 实现关系
                    相当于基类是纯虚函数, 需要子类去实现接口.
                    例如:
                        car ---->(Realization) 交通工具
            (3) 依赖关系
                    一个 A 类中的方法的传入参数为另外一个 B 类的实例或则一个方法的返回值是 B, 而且 A 类中成员变量没有 B 类, 则 A 依赖与 B
                    A ---> (Dependency) B
                    
                    依赖关系是五种关系中耦合最小的一种关系, 使用虚线加箭头表示
                    
                    class B
                    {
                    public:
                        void run()
                        {
                            cout<< "xxxx" << endl;
                        }
                    };
                    class A
                    {
                    public: 
                        void goWork(B *car)  // 传入参数为 B 的指针，并且没有成员变量 B
                        {
                            car->run();
                        }
                        
                        B* fix()  // 返回值为 B 的指针，并且没有成员变量 B
                        {
                            
                        }
                    };
            (4) 关联关系
                    就是 A 类中方法需要用到 B 类，但不要通过参数传入, A 类本身有 B 的成员变量
                    关联关系要比依赖关系更加紧密
                    
                    关联关系是实线加箭头表示, 表示类之间的关系比依赖要强
                                        s
                    class B
                    {
                    public:
                        void run()
                        {
                            cout<< "xxxx" << endl;
                        }
                    };
                    class A
                    {
                    public: 
                        void goWork()  // 本身有成员变量 B
                        {
                            car->run();
                        }
                        
                    private:
                        B *car;
                    };

                    (1) 关联关系名称，　关联关系可以有一个名称, 用于描述该关系的性质.
                        关联名称应该是动词短语, 表明源对象正在目标对象上执行动作
                        例如:           drive
                                Person ----->  Car
                                
                    (2) 关联可以是双向的，也可以是单向的。双向的关联可以有两个箭头或者没有箭
                        头，单向的关联有一个箭头
                    (3) 关联关系之一: 聚合
                            聚合是整体和部分之间的关系，例如汽车由引擎、轮胎以及其它零件组成. 整体没有了，部分也可以存在．
                            例如宝马汽车可以用宝马引擎，也可以用奔驰引擎.
                            聚合关系中，两个类处于不同的层次上，一个代表整体，一个代表部分
                            聚合里面都定义内部的成员变量, 只不过内部成员的变量是由传入参数进行赋值.所以整体没有了, 部分还是存在的.
                            类图中:
                                宝马汽车 <----Aggregation  宝马引擎
                            
                    (4) 关联关系之一: 组合
                            代表整体的对象负责代表部分对象的生命周期, 整体没有了, 对应的部分也就没有了.
                            
                            组合里面也都定义内部的成员变量, 只不过内部成员的变量是由内部成员方法 new 出来的.所以整体没有了, 
                            部分也就不存在的.
                            
                            例如百度公司只能用自身百度的部门
                            类图中:
                                公司 <----- Composition 部门
                                
            (5) 关系的区别
                    a. 聚合关系与组合关系
                            聚合关系中，整件不会拥有部件的生命周期，所以整件删除时，部件不会被删除。再者，多个整件可以共享同一个部件.
                            
                            组合关系中，整件拥有部件的生命周期，所以整件删除时，部件一定会跟着删除。而且，
                            多个整件不可以同时间共享同一个部件。
                    
                            聚合关系是“has-a”关系，组合关系是“contains-a”关系
```

## 时序图

```shell
    1. Add -> Model -> Add Diagram -> Sequence Diagram(时序图)
    2. 在 UML 中, 纵轴是时间轴, 时间延竖线向下延伸. 横轴代表在协作中各个独立的对象.
```

# 面向对象的设计模式

## 设计模式概述
```shell
    1. 软件模式与具体的应用领域无关，是个通用的思想方法, 跟编程语言也无关, 只要是面向对象语言的.
    2. 使用设计模式是为了可重用代码、让代码更容易被他人理解并且保证代码可靠性
    3. 通常包含 4 大类:
            (1) 创建型(Creational) 模式: 如何创建对象
            (2) 结构型(Structural) 模式: 如何实现类或则对象的组合
            (3) 行为型(Behavioral) 模式: 类或对象怎样交互以及怎样分配职责
            (4) 简单工厂模式
```

## 面向对象设计原则
```shell
    1. 设计原则的目的：　高内聚(一个模块, 一个接口功能要非常单一), 低耦合(一个模块尽量与其他模块较少进行交互)
    2. 
        (1) 单一职责原则(SRP, Single Responsibility Principle), 类的职责单一，对外只提供一种功能, 提高安全性和可维护性.
        (2) 开闭原则(OCP, Open-Closed Principle, 重要), 类的改动是通过增加代码进行的，而不是修改源代码, 主要通过新增新的继承类, 
                                                      原来的类不进行改动. 作用: 提高可维护性, 安全性, 可读性, 可复用性.
        (3) 里氏代换原则(Liskov Substitution Principle, LSP), 任何抽象类出现的地方都可以用他的实现类进行替换，实际就是虚拟机制，
                                                            语言级别实现面向对象功能, 实际上就是用多态, 继承类继承基类.
        (4) 依赖倒转原则(Dependence Inversion Principle, DIP, 重要), 依赖于抽象(接口实现层用基类指针指向派生类)，
            不要依赖具体的实现(类)， 也就是针对接口编程.
            为了解决刚开始张三开宝马, 之后张三又开奔驰，之后又开其他的车. 如果不用多态, 则接口函数越来越多.
             需要定义
                    高层业务逻辑层
                    
                    抽象层  (对应的车的抽象类)
                    
                    实现层　(实现层继承于抽象层)
                    
             将业务层和实现层通过抽象层隔离(解耦合)
             
             例如:
                    
                //------抽象层------
                
                class Car
                {
                public:
                    virtual void run() = 0;
                };
                
                class Driver {
                public:
                    virtual void drive(Car *car) = 0;
                };
                
                //实现层-----
                class Zhang3 :public Driver{
                public:
                	virtual void drive(Car *car)
                	{
                		cout << "涨3 开车了" << endl;
                		car->run();
                	}
                };
                class Li4 :public Driver{
                public:
                	virtual void drive(Car *car)
                	{
                		cout << "Li4 开车了" << endl;
                		car->run();
                	}
                };
                
                class Benz :public Car
                {
                public:
                	virtual void run() {
                		cout << "benz 启动了" << endl;
                	}
                };
                class BMW :public Car
                {
                public:
                	virtual void run() {
                		cout << "BMW 启动了" << endl;
                	}
                };

                // 业务层
                //张三去开奔驰
                Car * benz = new Benz;
                Driver* zang3 = new Zhang3;
            
                zang3->drive(benz);
                
        (5) 接口隔离原则(Interface Segregation Principle, ISP), 不应该强迫用户的程序依赖他们不需要的接口方法。
            一个接口应该只提供一种对外功能，不应该把所有操作都封装到一个接口中去。
        (6) 合成复用原则 (Composite Reuse Principle, CRP, 主要), 如果使用继承，会导致父类的任何变换都可能影响
            到子类的行为。如果使用对象组合，就降低了这种依赖关系。对于继承和组合(依赖或则关联)，优先使用组合. 如果使用继承方式, 
            那么新的子类将会和所有父类高耦合.而使用组合方式, 则可以减低耦合度.
        (7) 迪米特法则(Law of Demeter, LoD), 一个对象应当对其他对象尽可能少的了解，从而降 低各个对象之间的耦合，提高系统的可维护性。
            例如在一个程序中，各个模块之间相互调用时，通常会提供一个统一的接口来实现。这样其他模块不需要了解另外一个模块的内部实现细节，
            这样当一个模块内部的实现发生改变时，不会影响其他模块的使用。（黑盒原理）
```

### 创建型模型
```shell
    1. 简单工厂模式, 只通过向工厂类传入具体标签, 得到具体继承对象的基类指针(抽象类). 而工厂类则负责创建不同的继承类.
                    //抽象的水果类
                    class Fruit
                    {
                    public:
                    	virtual void getName() = 0;
                    };
                    
                    class Apple :public Fruit
                    {
                    public:
                    	virtual void getName() {
                    		cout << "我是苹果" << endl;
                    	}
                     };
                    
                    class Banana :public Fruit
                    {
                    public:
                    	virtual void getName() {
                    		cout << "我是香蕉" << endl;
                    	}
                    };
                    
                    //工厂
                    class Factory {
                    public:
                    	//水果生产器
                    	Fruit * createFruit(string kind) {
                    		Fruit *fruit = NULL;
                    
                    		if (kind == "apple") {
                    			fruit =  new Apple;
                    		}
                    		else if(kind == "banana"){
                    			fruit =  new Banana;
                    		}
                    		return fruit;
                    	}
                    };
                    
                   //人们是跟工厂打交道
                   Factory *factory = new Factory; //创建一个工厂
                   //给我来一个苹果
                   Fruit *apple = factory->createFruit("apple");
                
                   apple->getName();
                       A. 简单工厂模式的优缺点
                               优点: 实现了对象创建和使用的分离(通过多态的形式), 不需要记住具体类名,只需要向工厂类传入相应的参数.
                               缺点: 对工厂类职责过重，一旦不能工作，系统受到影响, 通过针对工厂类违反“开闭原则”，添加新产品需要修改工厂逻辑，
                               　　　工厂类越来越复杂
                       B. 适用场景
                               (1) 工厂类负责创建的对象比较少，由于创建的对象较少，不会造成工厂方法中的业务逻辑太过复杂.
                               (2) 客户端只知道传入工厂类的参数，对于如何创建对象并不关心
                               
    2. 工厂方法模式
            (1) 主要将工厂进行　"开闭原则", 由原先简单工厂中的类复杂业务判断, 抽象成一个工厂基类, 再定义该工厂的派生类, 
                这个工厂派生类只能创建具体的类(例如苹果类), 只针对特定的一个类.
                简单工厂模式 + "开闭原则"  = 工厂方法模式
            (2) 例子:
                    //抽象的水果类
                    class Fruit
                    {
                    public:
                    	virtual void getName() = 0;
                    };
                    
                    class Apple :public Fruit {
                    public:
                    	virtual void getName() {
                    		cout << "我是苹果" << endl;
                    	}
                    };
                    
                    class Banana :public Fruit {
                    public:
                    	virtual void getName() {
                    		cout << "我是香蕉 " << endl;
                    	}
                    };
                    
                    //抽象的工厂类
                    class AbstractFactory
                    {
                    public:
                    	virtual Fruit * createFruit() = 0;//抽象的水果生产器
                    };
                    
                    //苹果的工厂
                    class AppleFactory :public AbstractFactory
                    {
                    public:
                    	virtual Fruit * createFruit() {
                    		return new Apple;
                    	}
                    };
                    
                    //香蕉工厂 
                    class BananaFactory : public AbstractFactory {
                    public:
                    	virtual Fruit *createFruit() {
                    		return new Banana;
                    	}
                    };
                    
                    //1 给我来一个香蕉的工厂
                    AbstractFactory * bananaFactory = new BananaFactory;
                    //2 给我来一个水果
                    Fruit * banana = bananaFactory->createFruit(); //只能够生成香蕉 //多态
                    banana->getName(); //多态
                    
            (3) 优缺点:
                    a. 优点: 实现工厂类的抽象, 每个具体工厂符合开闭原则.同时拥有了简单工厂模式的优点(对象创建和使用分离)
                    b. 缺点: 随着业务增多, 具体的工厂类也会很多．
            (4) 适用场景
                    规模中等的模型
                    
    3. 抽象工厂模式
            (0) 工厂方法模式 + "产品族" = 抽象工厂方法模式
            (1) 产品族: 这一系列产品, 它们都来自同一厂商, 并且拥有各自的功能.
                产品等级结构: 如果一系列产品, 它们拥有相同的功能, 并且来自不同的厂商．
            (2) 抽象工厂模式是针对产品族来划分的,负责生产同一产地的所有产品,
            (3) 针对产品族(例如添加一个日本的工厂和日本的苹果, 香蕉)，　符合开闭原则, 只需要额外新建一个日本具体工厂, 日本的苹果类，
            　　　香蕉类.
            (4) 针对产品等级结构(例如添加鸭梨), 则不符合开闭原则
            
            (5) 例子:
                    class Fruit
                    {
                    public:
                    	virtual void getName() = 0;
                    };
                    
                    class USAApple :public Fruit
                    {
                    public:
                    	virtual void getName()  {
                    		cout << "我是美国的苹果" << endl;
                    	}
                    };
                    
                    class USABanana :public Fruit
                    {
                    public:
                    	virtual void getName()  {
                    		cout << "我是美国的香蕉" << endl;
                    	}
                    };
                    
                    class ChinaApple :public Fruit
                    {
                    public:
                    	virtual void getName()  {
                    		cout << "我是中国的苹果" << endl;
                    	}
                    };
                    
                    class ChinaBanana :public Fruit
                    {
                    public:
                    	virtual void getName()  {
                    		cout << "我是中国的香蕉" << endl;
                    	}
                    };
                    
                    //定一个抽象的工厂，是面向产品族进行生产
                    class AbstractFactory {
                    public:
                    	virtual Fruit *createApple() = 0;
                    	virtual Fruit *createBanana() = 0;
                    	//添加一个鸭梨的生成器
                    	virtual Fruit* createPear() = 0;
                    };
                    
                    //美国的具体工厂
                    class USAFactory :public AbstractFactory
                    {
                    public:
                    	virtual Fruit *createApple() {
                    		return new USAApple;
                    	}
                    	virtual Fruit *createBanana()  {
                    		return new USABanana;
                    	}
                    
                    	//添加一个美国的生产器
                    	virtual Fruit * createPear() {
                    		return new USAPear;
                    	}
                    	
                    };
                    //中国的工厂
                    class ChinaFactory :public AbstractFactory
                    {
                    public:
                    	virtual Fruit *createApple()
                    	{
                    		return new ChinaApple;
                    	}
                    
                    	virtual Fruit *createBanana()  {
                    		return new ChinaBanana;
                    	}
                    
                    	//添加一个的生产器
                    	virtual Fruit * createPear() {
                    		return new ChinaPear;
                    	}
                    };
                    
                    //想要一个美国的香蕉
                    //1 来一个美国的工厂
                    AbstractFactory * usaF = new USAFactory;
                    //2 来一个香蕉
                    Fruit *usaBanana = usaF->createBanana();
                
                    usaBanana->getName();
                    
            (6)  优缺点:
                        a. 优点: 拥有工厂方法模式的优点.
                                当一个产品族中的多个对象被设计成一起工作时，它能够保证客户端始终只使用同一个产品族中的对象.
                                增加新的产品族，不需要修改已有的系统，符合开闭原则.
                        b. 缺点: 如果要新增产品等级结构就比较麻烦，需要修改系统每一个产品族类, 不符合开闭原则.
            (7) 适用场景:
                    a. 系统中有多于一个的产品族．
                    b. 产品等级结构稳定(比如电脑中定下来就是内存，显卡，CPU)
                    c. 复杂的产品等级和产品族
                    
    4. 单例模式
        (1) 单例模式是全局唯一, 也是单例类, 该类只有一个对象.
        (2) 如何创建单例模式, 可以把构造函数放在 pirvate 下, 来限制类的外部创建本类的实例(避免调用 2 次构造函数)
        　　 并且该单例类提供对外的获取指针方法.
        (3) 三个要点
                1. 某个类只能有一个实例；
                2. 自行创建这个实例(程序运行时就创建)
                3. 向整个系统提供这个实例(全局性)
        (4) 单例模式的使用步骤：
                a) 构造函数私有化。(为了防止类的外部额外创建其他实例)
                b) 提供一个全局的静态方法（全局访问点）来获取单例对象。
                c) 在类中定义一个私有静态指针，指向本类的变量的静态变量指针
        (5) 单例模式类型
                a. 饿汉式
                        不管这个实例用不用, 程序编译时就已经被创建了．饿汉式创建这个实例不需要考虑多线程．
                        class Singleton
                        {
                        public:
                        
                        	static Singleton* getInstance()
                        	{
                        		return instance;
                        	}
                        
                        private:
                        	//不让类的外部再创建实例
                        	Singleton() {
                        
                        	}
                        	static Singleton* instance;//指向本类唯一实例的指针。
                        };
                        
                        /*
                        饿汉式  - 在编译期间就已经确定这个唯一的实例了。
                        */
                        Singleton*  Singleton::instance = new Singleton; //类的内部
                b. 懒汉式
                       程序只有在调用 getInstance() 过程中才被创建这个实例.
                       
                       class Singleton
                       {
                           public:
                            static Singleton* getInstance()
                                {
                                    //加锁
                                    if (instance == NULL) {
                                        instance = new Singleton2;
                                    }
                                    //解锁
                                    return instance;
                                }
                           
                           private:
                            Singleton() {
                           
                            }
                            static Singleton * instance;
                       };
                       
                       
                       //懒汉式的初始化方式
                       Singleton *Singleton::instance = NULL;
                       
        (6) 应用场景：　日志的打印可以使用单例模式, 系统要求提供一个唯一的序列号生成器或资源管理器，
                      或者需要考虑资源消耗太大而只允许创建一个对象
        (7)  优缺点:
                    a. 优点: 单例模式提供了对唯一实例的受控访问
                            节约系统资源
                    b. 缺点: 扩展略难,单例模式中没有抽象层
                             单例类的职责过重
```

### 结构型模式(类与类的组合, 获得更大的结构)
```shell
    1. 代理模式
            (1) 为其他对象提供一种代理(Proxy)以控制对这个对象的访问
            (2) 代理，是指具有与代理元（被代理的对象）具有相同的接口的类， 客户端必须通过代理与被代理的目标类交互，
                而代理一般在交互的过程中（交互前后），进行某些特别的处理。
            (3) 例子
                    // 抽象的购物方式
                    class Shopping
                    {
                    public:
                    	virtual void buy(Item *it) = 0;//抽象的买东西方法
                    };
                    
                    //韩国购物
                    class KoreaShopping :public Shopping
                    {
                    public:
                    	virtual void buy(Item *it)  {
                    		cout << "去韩国买了" << it->getKind()<< endl;
                    	}
                    };
                    
                    //海外代理
                    class OverseasProxy :public Shopping
                    {
                    public:
                    	OverseasProxy(Shopping *shpping)  // 构造函数传入实际 buy
                    	{
                    		this->shopping = shpping;
                    	}
                    
                    	virtual void buy(Item *it)  {
                    
                    		//1 辨别商品的真假，
                    		//2 进行购买（）
                    		//3 通过海关安检，带回祖国
                    
                    		if (it->getFact() == true)
                    		{
                    			cout << "1 发现正品， 要购物" << endl;
                    
                    			//用传递进来的购物方式去购物
                    			shopping->buy(it);
                    
                    
                    			//3 安检
                    			cout << "2 通过海关安检， 带回祖国" << endl;
                    		}
                    		else {
                    			cout << "1 发现假货，不会购买" << endl;
                    		}
                    		
                    	}
                    private:
                    	Shopping *shopping; //有一个购物方式
                    };
                    
                    Shopping *usaShopping = new USAShopping;
                    // 代理类也继承抽象类, 重写 buy 接口(前后进行相关的操作)
                    Shopping *overseaProxy = new OverseasProxy(usaShopping); 
                    overseaProxy->buy(&it1);
                    
            (4) 优缺点:
                    a. 优点: 可以针对抽象类进行编程，增加和更换代理类无须修改源代码，符合开闭原则，系统具有较好的灵活性和可扩展性
                    b. 缺点: 代理实现较为复杂
            (5) 适用场景
                    为其他对象提供一种代理以控制对这个对象的访问
            (6) 代理模式与装饰器的区别是
                    代理模式是静态给一个类添加功能, 一般不进行嵌套,如果一个类中的添加功能比较复杂, 则考虑采用代理模式.
                    而装饰器则是动态给一个类添加功能,如果一个类比较简单,同时需要动态嵌套.
            　　　
                    
    2. 装饰模式
            (1) 装饰器是一个类, 拥有一个基本功能类对象(成员变量中有　ConcreteComponet(具体构件)). 这个是额外新的新的对象.
            (2) 
                //抽象的手机类
                class Phone
                {
                public:
                	virtual void show() = 0;
                };
                
                class iPhone :public Phone
                {
                public:
                	virtual void show() {
                		cout << "秀出了iphone" << endl;
                	}
                };
                
                //写一个抽象的装饰器
                class Decorator :public Phone
                {
                public:
                	Decorator(Phone *phone) {
                		this->phone = phone;
                	}
                
                	virtual void show() = 0;
                protected:
                	Phone * phone; //拥有一个 所有手机的父类指针
                };
                
                //具体的手机贴膜装饰器
                class MoDecorator :public Decorator
                {
                public:
                	MoDecorator(Phone *phone) : Decorator(phone) {}
                	virtual void show() {
                		this->phone->show();  //保持原有的show
                
                		this->mo(); //额外添加一个 mo的方法
                	}
                
                	//膜装饰器，可以修饰添加的方法
                	void mo() {
                		cout << "手机有了贴膜" << endl;
                	}
                };
                
                //皮套的装饰器类
                class TaoDecorator :public Decorator
                {
                public:
                	TaoDecorator(Phone *phone) : Decorator(phone) {}
                	virtual void show() {
                		this->phone->show();
                		tao();
                	}
                
                	void tao() {
                		cout << "手机有了皮套" << endl;
                	}
                };
                
                Phone *phone = new iPhone;
                phone->show();
                
                Phone * moPhone = new MoDecorator(phone);
                moPhone->show();
                
                Phone *moTaoPhone = new TaoDecorator(moPhone);
                moTaoPhone->show(); //moPhone.show() + tao() ==  phone.show() + mo() + tao();
                
    3. 外观模式
            (1) 外观模式相当于接口统一化, 隐藏了接口实现的细节, 对外就统一一个接口.
    4. 适配器模式
            (1) 将一个类的接口转换成客户希望的另外一个接口, 使得原本由于接口不兼容而不能一起工作的那些类可以一起工作。
            
            (2) 实例
                　　/**Target 类(目标抽象类)
                       目标抽象类定义客户所需接口
                    */
                    class V5
                    {
                    public:
                    	virtual void useV5() = 0; 
                    };
                    
                    //目前只有v220的类 没有v5
                    /*
                        适配者类, 被适配的角色, 
                     */
                    class V220
                    {
                    public:
                    	void useV220() {
                    		cout << "使用了220v的电压" << endl;
                    	}
                    };
                    
                    //定义一个中间的适配器类, 
                    // 适配器类 Adapter 继承与 Target 类, 并且重写虚函数方法,　虚函数内调用了 Adaptee(适配者类)的具体方法
                    class Adapter :public V5
                    {
                    public:
                    	Adapter(V220 * v220)
                    	{
                    		this->v220 = v220;
                    	}
                    	~Adapter() {
                    		if (this->v220 != NULL) {
                    			delete this->v220;
                    		}
                    	}
                    
                    	virtual void useV5() {
                    		v220->useV220(); //调用需要另外的方法
                    	}
                    
                    private:
                    	V220 *v220;
                    };
                    
                    class iPhone
                    {
                    public:
                    	iPhone(V5 *v5)
                    	{
                    		this->v5 = v5;
                    	}
                    	~iPhone()
                    	{
                    		if (this->v5 != NULL) {
                    			delete this->v5;
                    		}
                    	}
                    
                    	//充电的方法
                    	void charge()
                    	{
                    		cout << "iphone手机进行了充电" << endl;
                    		v5->useV5();
                    	}
                    private:
                    	V5*v5; 
                    };
                    
                    iPhone *phone = new iPhone(new Adapter(new V220));
                    phone->charge();
                    
            (3) 使用场景
                    当一个类已经写好后, 调用原来的虚函数接口. 如果不想改变这个类的任何接口细节, 则可以采用适配器类的桥梁，
                    适配器类继承这个基类, 再构造函数内传入另外一个类, 重写虚函数
```

### 行为型模式(用来对类或对象怎样交互和怎样分配职责进行描述)
```shell
    1. 模板方法模式
            (1) AbstractClass（抽象类）：在抽象类中定义了一系列基本操作(PrimitiveOperations)，这些基本操作可以是具体的，
            　　　　　　　　　　　　　　　　也可以是抽象的(纯虚函数), 每一个基本操作对应算法的一个步骤, 在其子类(继承类)中可以重定义或
                                       实现这些步骤. 在抽象类中实现了一个模板方法(Template Method), 用于定义一个算法
                                       的框架，模板方法不仅可以调用在抽象类中实现的基本方法，也可以调用在抽象类的子类(继承类)中
                                       实现的基本方法，还可以调用其他对象中的方法。
                ConcreteClass（具体子类）：它是抽象类的子类，用于实现在父类中声明的抽象基本操作(纯虚函数)以完成子类特定算法的步骤，
                                         也可以覆盖在父类中已经实现的具体基本操作.
                                       
            (2) 实例
                    //抽象的制作饮料方法
                    class MakeDrink
                    {
                    public:
                    	//1 把水煮开
                    	void boil() {
                    		cout << "把水煮开" << endl;
                    	}
                    	//2 冲某物
                    	virtual void brew() = 0;
                    	//3 从大杯倒入小杯
                    	void putInCup()
                    	{
                    		cout << "把冲泡好的饮料 从大杯倒入小杯" << endl;
                    	}
                    	//4 加一些酌料
                    	virtual void addThings() = 0;
                    
                    	//钩子函数， hook
                    	virtual bool CustomWantAddThings() {
                    		return true;
                    	}
                    
                    
                    	//业务的逻辑的统一模板 
                    	void make() {
                    		boil();
                    		brew(); //子类
                    		putInCup(); 
                    
                    		if (CustomWantAddThings() == true) {
                    			addThings(); //子类的多态
                    		}
                    	}
                    };
                    
                    //制作咖啡
                    class MakeCoffee :public MakeDrink
                    {
                    public:
                    	MakeCoffee(bool isAdd)
                    	{
                    		this->isAdd = isAdd;
                    	}
                    	//2 冲某物
                    	virtual void brew()
                    	{
                    		cout << "冲泡咖啡豆" << endl;
                    	}
                    
                    	//4 加一些酌料
                    	virtual void addThings()  {
                    		cout << "添加糖和牛奶" << endl;
                    	}
                    
                        // 重写钩子函数, 与具体类自适应
                    	virtual bool CustomWantAddThings() {
                    		return isAdd;
                    	}
                    
                    private:
                    	bool isAdd;
                    };
                    
            
            (4) 优缺点:
                     a. 优点: 
                              (1) 模板方法模式是一种代码复用技术, 提取类库总的公共行为放在父类中, 通过其子类来实现不同的行为.
                              (2) 在模板方法模式中可以通过子类来覆盖父类的基本方法，不同的子类可以提供基本方法的不同实现，
                                  更换和增加新的子类很方便，符合单一职责原则和开闭原则
                              (3) 在抽象类中统一操作步骤，并规定好接口；让子类实现接口。这样可以把各个具体的子类和操作步骤解耦合
                              
                     b. 缺点: 需要为每一个基本方法的不同实现提供一个子类，如果父类中可变的基本方法太多，将会导致类的个数增加，
                              系统更加庞大，设计也更加抽象
            (5) 适用场景
                    (1) 具有统一的操作步骤或操作过程;
                    (2) 具有不同的操作细节;
                    (3) 存在多个具有同样操作步骤的应用场景，但某些具体的操作细节却各不相同; 
    2. 命令模式
            (1) 命令模式是一种对象行为型模式, 也为　动作(Action)模式或　事务(Transaction)模式
            (2) 命令模式可以将请求发送者和接收者完全解耦，发送者与接收者之间没有直接引用关系，发送请求的对象只需要知道如何发送请求，
                而不必知道如何完成请求。
            (3) 适用与发送一系列的命令, 处理函数在另外一个类, 低耦合
            (4) 实例
                    //命令的最终执行者, Receiver(接收者)
                    class Cooker
                    {
                    public:
                    	//烤串
                    	void makeChuaner() {
                    		cout << "烤串师傅进行了烤串" << endl;
                    	}
                    
                    	//烤鸡翅
                    	void makeChicken() {
                    		cout << "烤串师傅进行了烤鸡翅" << endl;
                    	}
                    };
                    
                    
                    //烤串的  抽象的 菜单,  Command（抽象命令类）
                    class Command
                    {
                    public:
                    	Command(Cooker *cooker)
                    	{
                    		this->cooker = cooker;
                    	}
                    	~Command() {
                    		if (this->cooker != NULL) {
                    			delete this->cooker;
                    			this->cooker = NULL;
                    		}
                    	}
                    
                    	//菜单让最终的执行者干活的方法
                    	virtual void execute() = 0;
                    
                    protected:
                    	Cooker *cooker;
                    };
                    
                    //烤串的菜单,  ConcreteCommand（具体命令类）
                    class CommandChuaner :public Command
                    {
                    public:
                    	CommandChuaner(Cooker *cooker) : Command(cooker) {}
                    
                    	virtual void execute()  {
                    		//命令 最终让执行者干的工作。
                    		this->cooker->makeChuaner();
                    	}
                    };
                    
                    //烤鸡翅的菜单
                    class CommandChicken :public Command
                    {
                    public:
                    	CommandChicken(Cooker * cooker) : Command(cooker) {}
                    
                    	virtual void execute() {
                    		//命令 最终让执行者干的工作。
                    		this->cooker->makeChicken();
                    	}
                    };
                    
                    //管理所有命令的一个模块,  Invoker（调用者）
                    //服务员MM
                    class Waitress
                    {
                    public:
                    	//给服务员添加菜单的方法
                    	void setCmd(Command *cmd)
                    	{
                    		this->cmd_list.push_back(cmd);
                    	}
                    
                    	//让服务员mm 下单
                    	void notify() {
                    		list<Command *>::iterator it = cmd_list.begin();
                    		for (; it != cmd_list.end(); it++) {
                    			(*it)->execute(); //在此发生了多态
                    		}
                    	}
                    private:
                    	list<Command *> cmd_list;
                    };
                    
                    Waitress *mm = new Waitress;
                
                    Command *chuanger = new CommandChuaner(new Cooker);
                    Command *chicken = new CommandChicken(new Cooker);
                
                    //把订单都给服务员
                    mm->setCmd(chuanger);
                    mm->setCmd(chicken);
                
                    //让服务员下单，最终让师傅干活
                    mm->notify();
                    
            (4) 优缺点:
                     a. 优点: 
                          (1) 降低系统的耦合度, 请求者和接受者不存在直接的引用, 它们之间完全耦合.
                          (2) 新的命令可以很容易地加入到系统中, 只需要新增具体命令类, 无须修改原来的代码, 符合开闭原则
                          (3) 可以比较容易地设计一个命令队列或宏命令（组合命令）
                              
                     b. 缺点: 使用命令模式可能会导致某些系统有过多的具体命令类
            (5) 适用场景
                    (1) 系统需要将请求调用者和请求接收者解耦，使得调用者和接收者不直接交互。请求调用者无须知道接收者的存在，
                        也无须知道接收者是谁，接收者也无须关心何时被调用
                    (2) 系统需要在不同的时间指定请求、将请求排队和执行请求。一个命令对象和请求的初始调用者可以有不同的生命期，
                        最初的请求发出者可能已经不在了，而命令对象本身仍然是活动的，可以通过该命令对象去调用请求接收者(具体执行对象),
                        而无须关心请求调用者的存在性，可以通过请求日志文件等机制来具体实现。
                    (3) 系统需要将一组操作组合在一起形成宏命令
                    
    3. 策略模式
            (1) 
                a. Context（环境类）：环境类是使用算法的角色，它在解决某个问题（即实现某个方法）时可以采用多种策略。
                  在环境类中维持一个对抽象策略类的引用实例，用于定义所采用的策略
                b. Strategy（抽象策略类）：它为所支持的算法声明了抽象方法，是所有策略类的父类，环境类通过抽象策略类中声明的
                   方法在运行时调用具体策略类中实现的算法
                c. ConcreteStrategy（具体策略类）：它实现了在抽象策略类中声明的算法(重写虚函数)，在运行时，
                　　具体策略类将覆盖在环境类中定义的抽象策略类对象，使用一种具体的算法实现某个业务处理。
            (2) 实例
                    //抽象的策略（抽象的武器）
                    class AbstractStrategy
                    {
                    public:
                    	//纯虚函数， 使用具体武器的策略,
                    	virtual void useWeapon() = 0;
                    };
                    
                    class KnifeStrategy :public AbstractStrategy
                    {
                    public:
                    	virtual void useWeapon() {
                    		cout << "使用匕首，进行近战攻击" << endl;
                    	}
                    };
                    
                    class AkStrategy :public AbstractStrategy
                    {
                    public:
                    	virtual void  useWeapon() {
                    		cout << "使用ak 进行远程攻击" << endl;
                    	}
                    };
                    
                    
                    class Hero
                    {
                    public:
                    	Hero()
                    	{
                    		strategy = NULL;
                    	}
                    
                    	void setStrategy(AbstractStrategy *strategy)
                    	{
                    		this->strategy = strategy;
                    	}
                    
                    	//攻击方法
                    	void fight() {
                    		cout << "英雄开始战斗了" << endl;
                    		this->strategy->useWeapon();
                    	}
                    
                    private:
                    	//拥有一个 使用攻击策略的抽象成员
                    	AbstractStrategy *strategy;
                    };
                    
                    AbstractStrategy *knife = new KnifeStrategy;
                    AbstractStrategy *ak47 = new AkStrategy;
                
                    Hero *hero = new Hero;
                
                    cout << "远程兵来了， 更换远程攻击" << endl;
                    hero->setStrategy(ak47);
                    hero->fight();
                
                    cout << "近战兵 来了， 更换近战的攻击" << endl;
                    hero->setStrategy(knife);
                    hero->fight();
                    
            (3) 优缺点:
                     a. 优点: 
                          (1) 用户可以在不修改原有系统的基础上选择算法或行为，也可以灵活地增加新的算法或行为。
                          (2) 使用策略模式可以避免多重条件选择语句
                          (3) 策略模式提供了一种算法的复用机制
                              
                     b. 缺点: 策略模式将造成系统产生很多具体策略类
            (4) 适用场景
                    
                    
    3. 观察者模式
            (1) 观察者模式是用于建立一种对象与对象之间的依赖关系，一个对象发生改变时将自动通知其他对象，其他对象将相应作出反应。
            　　 在观察者模式中，发生改变的对象称为观察目标，而被通知的对象称为观察者.
            (2) 
                a. Subject（被观察者或目标，抽象主题）：被观察的对象。当需要被观察的状态发生变化时，需要通知队列中所有观察者对象。
                                                   Subject需要维持（添加， 删除，通知）一个观察者对象的队列列表。
                b. ConcreteSubject（具体被观察者或目标，具体主题）：被观察者的具体实现。包含一些基本的属性状态及其他操作。
                c. Observer（观察者）：接口或抽象类。当 Subject 的状态发生变化时， Observer 对象将通过一个 callback 函数得到通知。
                d. ConcreteObserver（具体观察者）：观察者的具体实现。得到通知后将完成一些具体的业务逻辑处理
            (3) 实例
                // 抽象的观察者， （监听者）
                class Listenner
                {
                public:
                	//老师来了 我改怎么办
                	virtual void onTeacherComming() = 0;
                
                	//学生干坏事的方法
                	virtual void doBadthing() = 0;
                };
                
                //抽象的 被观察者， （通知者）
                class Notifier
                {
                public:
                	//添加观察者的方法
                	virtual void addListenner(Listenner *listenner) = 0;
                	//删除观察者的方法
                	virtual void delListenner(Listenner *listenner) = 0;
                
                	//通知所有观察者的方法
                	virtual void notify() = 0;
                };
                
                //具体的观察者
                class Student :public Listenner
                {
                public:
                	Student(string name, string badthing)
                	{
                		this->name = name; 
                		this->badthing = badthing;
                	}
                
                	//老师来了学生该怎么办
                	virtual void onTeacherComming()
                	{
                		cout << "学生"<<name  <<"发现班长给我使眼神了， 停止" << badthing << endl;
                		cout << "改为写作业" << endl;
                	}
                
                	virtual void doBadthing() {
                		cout << " 学生 " << name << "目前正在 " << badthing << endl;
                	}
                private:
                	string name;
                	string badthing;
                };
                
                
                //具体的通知者(班长)
                class Monitor :public Notifier
                {
                public:
                	//添加观察者的方法
                	virtual void addListenner(Listenner *listenner)  {
                		this->l_list.push_back(listenner);
                	}
                	//删除观察者的方法
                	virtual void delListenner(Listenner *listenner)  {
                		this->l_list.remove(listenner);
                	}
                
                	//通知所有观察者的方法
                	//班长使眼神的方法
                	virtual void notify()  {
                		//广播信息，让每一个学生都执行各自的重写的onTeacherComming方法
                		for (list<Listenner *>::iterator it = l_list.begin(); it != l_list.end(); it++) {
                			(*it)->onTeacherComming();
                			//在此处如果触发班长的notify（）
                		}
                	}
                private:
                	list<Listenner *> l_list; //班长手中所有的学生(观察者)
                };
                
                Listenner *s1 = new Student("张三", "抄作业");
                Listenner *s2 = new Student("李四", "打lol");
                Listenner *s3 = new Student("王五", " 看李四玩lol");
            
                Notifier *bossXu = new Monitor;
            
                //将所有的学生列表告知通知者，好让通知者进行通知 
                bossXu->addListenner(s1);
                bossXu->addListenner(s2);
                bossXu->addListenner(s3);
            
            
            
                cout << "教师一片和谐，老师没有来 " << endl;
                s1->doBadthing();
                s2->doBadthing();
                s3->doBadthing();
            
                cout << "班长突然发现老师来了，给学生们使了一个眼神" << endl;
                bossXu->notify();
                
            (4) 优缺点
                     a. 优点: 
                          (1) 观察者模式在观察目标和观察者之间建立一个抽象的耦合。观察目标只需要维持一个抽象观察者的集合，
                              无须了解其具体观察者。由于观察目标和观察者没有紧密地耦合在一起，因此它们可以属于不同的抽象化层次
                          (2) 观察者模式支持广播通信，观察目标会向所有已注册的观察者对象发送通知，简化了一对多系统设计的难度
                              
                     b. 缺点: 如果在观察者和观察目标之间存在循环依赖，观察目标会触发它们之间进行循环调用，可能导致系统崩溃。
            (5) 适用场景
                    
```