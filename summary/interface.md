# C++ 接口函数

## std

```c++
    1. 像 list, 这些没有办法像 vector 这样按下标[]访问,可以使用 std::next() 函数
        std::list<Object>::iterator it = std::next(l.begin(), N);
    2. vector.data()返回的是该数组第一个元素的地址
            
                    
```

### <algorithm>

```shell
    1. std::transform
            (1) std::transform 在指定的范围内应用于给定的操作，并将结果存储在指定的另一个范围内
            (2) std::transform 有一元操作，将 op 应用于[first1, last1]范围内的每个元素，并将每个操作返回的值存储在
                以 result 开头的范围内。给定的 op 将被连续调用 last1 - first1 + 1 次。op 可以是函数指针或函数对象
                或lambda表达式
                
                    int op_increase(int i) {return (i + 5)};
                    调用方式:
                    std::transform(first1, last1, result, op_increase);
                    将容器 suffix 内所有字母转大写
                    std::transform(suffix.begin(), suffix.end(), suffix.begin(), (int (*)(int))toupper);
                    
            (3) 对于二元操作，使用[first1, last1]范围内的每个元素作为第一个参数调用 binary_op,并以 first2 开头的范围内的
            　　每个元素作为第二个参数调用 binary_op,每次调用返回的值都存储在以 result 开头的范围内。给定的 binary_op 将被
               连续调用 last1 - first1 + 1次。binary_op 可以是函数指针或函数对象或 lambda 表达式。
               
                    int op_add(int, a, int b) {return (a + b)};
                   调用方式:
                    std::transform(first1, last1, first2, result, op_add);
                    
            (4) result 和 first1指向的位置可以是相同的
                
```

