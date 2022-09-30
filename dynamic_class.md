# Class And Dynamic Memory Allocation

本章主要讲讲对类使用`new`和`delete`以及如何处理由于动态内存分配而引起的一些微妙问题.

来看看一个具体的例子.

假如我们要设计一个类, 而这个类有一个成员是用来储存某人的名字的.

很显然这个成员类型不是`string`类就是C风格字符串.

我们先暂时排除`string`哈, 如果用C风格字符串, 也就是字符数组的话, 有两种选择:

- 直接在声明这个类时,就分配好这个成员的大小, 比如我就预先分配给它16个字节.
- 在运行时, 通过`new`操作符动态分配这个成员的大小, 这个成员的内容储存在堆内存上面.

对于第一种, 虽然方便, 但是有很大的局限性, 就是这个名字的长度必须在15个以内(PS: 只就英语而言), 当然我们也可以一次性分配64个字节给它, 但是这个大小似乎对于大多数人的名字都是很大的, 这会浪费内存.

最好的解决办法就是第二种了, 我把这个成员的存储大小拖到运行时才决定大小, 这样就不会出现上面这两个致命问题.

不过, 我们直接使用`string`类是最方便的途径, 但是这就让我们没有机会了解其中的实现特别是动态内存分配的细节.

## 类与动态内存分配

为了测试我们上面这个思路, 我们可以先编写一个简陋的类`string`类型, 这个类型叫做`StringBad`, 这个类添加了一些辅助的东西, 并且简化了接口, 一些比较有用的例如`+`重载被忽略.\, 以此来更加明显地看到其中地实现细节和一些缺陷.

我们先来声明并且`实现这个类:
```cpp
#include <iostream>
#include <cstring>

using ::std::cout;
using ::std::endl;

class StringBad
{
private:
    char * str; // pointer to string
    int len;  // the length of string
    static int num_strings;
public:
    StringBad(const char *s)
    {
        len = strlen(s);
        str = new char[len + 1];
        strcpy(str, s);
        num_strings++;
        cout << num_strings << ": \"" << str << "\" object created" << endl;
    }

    StringBad()
    {
        len = 3;
        str = new char[4];
        strcpy(str, "Cpp");
        num_strings++;
        cout << num_strings << ": \"" << str << "\" default object created" << endl;
    }

    ~StringBad()
    {
        cout << "\"" << str << "\" object deleted, ";
        num_strings--;
        cout << num_strings << " left" << endl;
        delete [] str;
    }

    friend std::ostream & operator<< (std::ostream & os, const StringBad & sb)
    {
        os << sb.str;
        return os;
    }
};

int StringBad::num_strings = 0;
```
需要注意的是, 这个声明使用了`char`指针来表示字符串, 而这就意味着这个类声明并没有为这个字符串分配内存.

还有就是这个静态成员, 这个静态成员被这个类地所有对象共享, 无论创建多少个这个类的对象, 程序都之创建一个静态类成员副本.

我们是在类的构造函数那里分配了字符串内存, 而可以看到, 每次我们创建一个对象, 静态成员`num_strings`都会+1, 而每次被销毁, `num_strings`就会-1.

不过要初始化这个静态成员`num_strings`不能在构造函数里, 因为这个成员属于全不类对象.

对于这个非`const`静态成员, 就只能在类外初始化它. 而`const`静态成员可以在类内部声明的时候初始化它.

另外, 析构函数现在并不是像之前那样空着的了, 在调用它时,需要及时把分配给`str`的内存释放掉, 防止内存泄漏.

好的, 现在就来试试这个类:
```cpp
int main()
{
    StringBad sb1; // use defalut constructor
    StringBad sb2("Rust");
    StringBad sb3("Csharp");

    cout << sb1 << endl;
    cout << sb2 << endl;
    cout << sb3 << endl;

    cout << "assign sb2 to sb1" << endl;
    sb1 = sb2;
    cout << "sb1: " << sb1 << endl;

    cout << "create new StringBad object sb4 using sb3" << endl;
    StringBad sb4 = sb3;
    cout << "sb4: " << sb4 << endl;

    return 0;
}
```
output:
```cpp
1: "Cpp" default object created
2: "Rust" object created
3: "Csharp" object created
Cpp
Rust
Csharp
assign sb2 to sb1
sb1: Rust
create new StringBad object sb4 using sb3
sb4: Csharp
"Csharp" object deleted, 2 left
"Csharp" object deleted, 1 left
"Rust" object deleted, 0 left
"Rust" object deleted, -1 left
```

这个输出结果出乎意料的糟糕, 真的名副其实`StringBad`!

单单从输出,我们可以看到, 第一就是我们明明创建了"Cpp", 但是到后面的调用析构函数销毁却没有看到"Cpp"被销毁, 这造成了内存泄露了.

第二, 我们发现"Rust"和"Csharp"结尾释放了两次, 但是我明明就只有一个"Rust"和"Csharp"分配了空间, 这个就是导致程序崩溃的直接原因!

第三, 我们发现, 为什么我的`num_strings`到最后怎么变成负数了?

这个`StringBad`类贼多问题, 我们就来一一排查出现错误的原因.

首先来看第一个问题, 为什么"Cpp"没有被释放? `sb1 = sb2`是出错点, 这个就是把`sb2`赋值给`sb1`. 而这种赋值其实就是简单的把等式右边的对象的成员数据一一对应复制覆盖等式左边的成员数据.

由于`str`成员是指针, 因此, 只是把`sb2`的`str`成员这个指针值覆盖了原来指向"Cpp"的指针值.但是在覆盖之前我们并没有释放掉原来指针指向的"Cpp".而覆盖掉就是把原有的指针值覆盖掉, 等到后面要去释放它也找不到入口了!

第二个问题, 就是当我们通过这两个语句, 使得`sb1`跟`sb2`的`str`指向了同一个资源"Rust", 使得`sb3`跟`sb4`的`str`指向了同一个资源"Csharp", 这样`sb4`先过期, 调用析构函数释放了"Csharp", 后面"sb3"也过期了, 也调用析构函数, 尝试去释放指向"Csharp"的指针, 但是这个资源已经释放掉了! `sb1`和`sb2`也是同一个道理.

第三个问题, 这是因为当我们创建`sb4`这个对象时候, 并没有调用我们自定义的构造函数, 而是调用了叫做复制构造函数的东西, 而这个是编译器默认提供的, 那么`num_strings`就不会随着`sb4`的创建而更新, 但是当`sb4`过期时, `sb4`却实实在在的调用了析构函数, 让`num_strings`-1, 就是这个少了的+1, 使得最后`num_strings`成了负数!

## 复制构造函数与赋值运算符

这出现一切问题的关键就是我们没有自定义复制构造函数和重载赋值运算符, 而自动调用了编译器默认给我们的复制构造函数和赋值运算符重载.

具体而言C++会自动提供下面成员函数:

- 默认构造函数, 如果没有定义构造函数
- 默认析构函数, 如果没有定义
- 复制构造函数, 如果没有定义
- 赋值运算符, 如果没有定义
- 地址运算符, 如果没有定义

地址运算符基本没什么问题, 而默认析构函数不执行任何操作, 在此就不讨论这两个, 默认构造函数也讲过了, 所以这里也不讨论.

而Cpp还另外提供了`两个特殊成员函数: 移动构造函数和移动赋值运算符.而这个后面章节会仔细讲.

复制构造函数的原型是这样子的:
```cpp
Class_name(const Class_name &);
```
对于复制构造函数, 需要知道这两点: 何时会调用以及有何功能.

一般情况下, 当我们新建一个对象并将其初始化为同类现有的对象时, 复制构造函数就会被调用.

而这种函数的最主要的功能就是将新对象初始化为现有的对象.

由于默认的复制构造函数只是逐个复制非静态成员, 所以`StringBad`的`str`只是复制了指针, 也就是浅复制.

而为了修复错误, 就要实现深度复制! 复制源对象的`str`的内容到新开辟的内存, 然后把指针指向新开辟的内容.

好的, 我们就来实现`StringBad`的深度复制的复制构造函数吧!
```cpp
StringBad(const StringBad & sb)
{
    len = strlen(sb.str);
    str = new char[len + 1];
    strcpy(str, sb.str);
    num_strings++;
}
```

至于第二个跟第三个问题就是跟赋值运算符有关了.

而赋值运算符的原型是这样子的:
```cpp
Class_name & operator= (const Class_name &);
```
当我们通过一个同类对象赋值给已有的对象时, 就会调用到赋值运算符.

而隐式赋值运算符跟前面的复制构造函数一样, 就是浅赋值.`StringBad`的第二跟第三出错点就在这里.

那么我们怎么改造呢?

还是跟复制前面的思路差不多, 但是要记住在赋值时, 需要及时释放原来的资源! 千万不要忘记, 要不又会内存泄漏.

现在就给`StringBad`加上赋值运算符函数.
```cpp    
StringBad & operator= (const StringBad sb)
    {
        len = strlen(sb.str);
        delete[] str;  // don't forget it!!!!!
        str = new char[len + 1];
        strcpy(str, sb.str);
        num_strings++;
        return *this;  // don't forget it!!!!!
    }

```
好的, 现在这个`StringBad`不再这么`bad`了.