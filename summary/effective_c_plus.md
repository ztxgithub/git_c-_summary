# effective c++ 

## 赋值运算符的自赋值问题

```c++
    1.自赋值问题的原因
        为C++允许变量有别名（指针和引用）,这使得一个数据可以有多个引用
        
    2. 赋值运算符的重载要注意 自赋值安全 和 异常安全
       有三种方法:
            (1) 判断两个地址是否相同 (自赋值安全)
            (2) 仔细地排列语句顺序 (同时兼顾　自赋值安全 和 异常安全)
            (3) Copy and Swap　(同时兼顾　自赋值安全 和 异常安全)
            
    3. 判断两个地址是否相同
            (1) 错误的用法
                    Widget& Widget::operator=(const Widget& rhs)　{
                        delete pb;                   // stop using current bitmap
                        pb = new Bitmap(*rhs.pb);    // start using a copy of rhs's bitmap
                        return *this;                // see Item 10
                    }
                    
                    如果　rhs == *this时, delete pb使得rhs.pb成为空值,接下来new的数据便是空的
                    
            (2) 解决方法(自赋值安全的实现,但并没有实现异常安全)
            
                    Widget& Widget::operator=(const Widget& rhs){
                        if (this == &rhs) return *this;
                        delete pb;                   // stop using current bitmap
                        pb = new Bitmap(*rhs.pb);    // start using a copy of rhs's bitmap
                        return *this;                // see Item 10
                    }
                    
                    注意：
                        若new出现了异常,当前对象的pb便会置空(已经执行delete pb)
                        
    4.仔细地排列语句顺序
    
            Widget& Widget::operator=(const Widget& rhs){
                Bitmap *pOrig = pb;               // remember original pb
                pb = new Bitmap(*rhs.pb);         // make pb point to a copy of *pb
                delete pOrig;                     // delete the original pb
                return *this;
            }
            
    5.Copy and Swap　
    
            Widget& Widget::operator=(Widget rhs){
                swap(rhs);                // swap *this's data with
                return *this;             // the copy's
            }
            
           这里是借助了swap的异常安全性,注意到这里是传值而不是传引用,传参时编译器会自动copy一份rhs进来
                    
```

