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
                            类图中:
                                宝马汽车 <----Aggregation  宝马引擎
                            
                    (4) 关联关系之一: 组合
                            代表整体的对象负责代表部分对象的生命周期, 整体没有了, 对应的部分也就没有了
                            例如百度公司只能用自身百度的部门
                            类图中:
                                公司 <----- Composition 部门
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
                       
        (6) 应用场景：　日志的打印可以使用单例模式.
            

```