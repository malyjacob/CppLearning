# Reference As Returned Type

**前情:**
> [Reference As Parameter](./ref_as_param.md)
<br/>

现在让我们来看看当引用作为函数返回值的情况:

```javascript
#include <iostream>

using std::cout;
using std::endl;

struct Time
{
public:
    int hour = 0;
    int min = 0;
    int second = 0;
    Time(int h = 0, int m = 0, int s = 0)
        : hour(h), min(m), second(s) {}
    friend void display(const Time &t)
    {
        cout << t.hour << "h : ";
        cout << t.min << "m : ";
        cout << t.second << "s;" << endl;
    }
    friend Time &plus(Time &des, const Time &src)
    {
        int ih = 0;
        int im = 0;

        int s = des.second + src.second;
        if (s < 60)
        {
            des.second = s;
        }
        else
        {
            des.second = s % 60;
            im = 1;
        }

        int m = des.min + src.min + im;
        if (m < 60)
        {
            des.min = m;
        }
        else
        {
            des.min = m % 60;
            ih = 1;
        }

        des.hour += ih + src.hour;
        return des;
    }
};

int main()
{
    Time t1(1, 23, 54);
    Time t2(2, 53, 12);

    display(t1);
    display(t2);

    /*  equal to:
     *  plus(t1, t2);
     *  display(t1);
     */
    display(plus(t1, t2));

    plus(t1, t2);
    display(t1);
}
```
output:
```javascript
1h : 23m : 54s;
2h : 53m : 12s;
4h : 17m : 6s;
7h : 10m : 18s;
```
这里的`display`运用了引用形参, 这样就不用另外复制, 直接使用源值.

而我们的关注点是这个`plus`函数. 这个函数返回一个`Time`类型的引用.

因此 `display(plus(t1, t2))`是能够生效的.

而这个`display(plus(t1, t2))`其实就等价于这下面两条语句: <br/>
```javascript
plus(t1, t2);
display(t1);
```
我们甚至可以这样: `plus(t1, t2) = t2`
这样的话, `t1`就被重置为与`t2`相等的值.

如果想要阻止这种别扭的语句泛滥, 办法也很简单, 就是在把`plus`的返回值前面加个`const`.
使得这函数的返回值是不可被修改的.
## 返回引用 vs 返回值
传统的返回机制与`按值传递`类似: <br/>
计算`return`后面的表达式的值, 并将结果返回给调用函数.

按概念来说, 就是把函数内部的return的值被放在一个临时的位置. <br/>
如果这个值没有被使用, 则会随之销毁.

而返回引用的函数不同, 函数的返回值是指向`return`后面的表达式, 也就是说, 函数最终会返回一个引用, <br/>
这个引用是指向`return`后面的表达式的.

## 相关建议

- 返回值最好就是指向函数的引用参数, 别玩花活.

这下面就是教训:
```javascript
const Time &clone_er(Time &t)
{
    Time t_temp;
    t_temp = t;
    return t_temp;
}
```
这个函数是有问题的, 因为函数的返回值是指向`t_temp`的, 但是这个`t_temp`是在函数内部的临时变量,
而这个临时变量会在走出函数作用域时被释放销毁.
于是这个返回值就是未定义的.

而如果像上面的`plus`函数一样, 返回值也还是指向参数里的引用, 那么这个返回值在函数结束时就有效.

