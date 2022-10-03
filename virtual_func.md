# Virtual Function

## 指针, 引用和派生类
一般来说, 什么类型的指针或者引用对应什么类型, 但在继承关系中, 却并非如此的严格.

我们其实可以将基类的指针或者引用指向派生类!

比如说这个例子:
```cpp
#include <iostream>

class Base
{
protected:
    int m_value {};
public:
    Base(int value = 0) : m_value(value) {}
    void showType()
    {
        std::cout << "I am Base, and my value is: " << m_value << std::endl;
    }
};

class Derived : public Base
{
public:
    Derived(int value = 0) : Base(value) {}
    void showType()
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
};

int main()
{
    Derived derived(5);
    derived.showType();

    Base & rb_derived = derived;
    rb_derived.showType();

    Base *pb_derived = &derived;
    pb_derived->showType();

    return 0;
}
```
output:
```cpp
I am Derived, and my value is: 5
I am Base, and my value is: 5
I am Base, and my value is: 5
```
我们确实可以将基类引用或者指针指向派生类对象, 但是输出的结果却似乎不是我们所期望的.

为何?

因为`rb_derived`和`pb_derived`是基类的引用和指针, 它们都只能看到所指向的派生类的基类部分, 即使我们的`derived`有自己的`showType`但它们却用不了, 更确切地说是看不到.

## 为何需要基类指针和引用指向派生类

为什么要用基类指针和引用指向派生类呢?

以下是理由:

第一,假如基类的派生类有好几十个,而并非这里的只有一个, 并且这些派生类也都有个自定义的`showType`, 那么如果我们要写一个函数要使用每个派生类的`showType`, 我们就必须为每一个派生类写一个函数重载, 这样的工程量巨大, 并且重复工作不少.

而如果我们用统一的基类指针或者引用代指这些繁复的派生类,我闷就只需要写一个接受基类指针或者引用作为参数的函数就可以了!

不过, 这样似乎也不行, 因为正如上面这个例子表明, 基类指针或者引用只能看到所指向的派生类的基类部分, 也就是说,当我们尝试用指向派生类的基类指针或者引用来调用`showType`时, 我们调用的`showType`版本不是派生类自定义的, 而是基类版本的`showType`.

不过, 有了虚函数, 这个问题就能迎刃而解.

## 虚函数和多态
虚函数是一种特殊的函数, 专用于类继承中.

当我们调用虚函数时, 虚函数会被解析为在基类与调用的派生类之间的最派生的函数版本.

像这种行为就会被成为多态, 如果一个派生类的函数有跟基类版本的相同的特征值和返回值, 那么这样的函数就会被成为`override`函数.

我们只需要将`virtual`加到目标函数声明前面,就可以把这个函数变成虚函数.

于是, 我们可以把前面那个例子改造成这样
```cpp
#include <iostream>

class Base
{
protected:
    int m_value {};
public:
    Base(int value = 0) : m_value(value) {}
    virtual void showType()
    {
        std::cout << "I am Base, and my value is: " << m_value << std::endl;
    }
};

class Derived : public Base
{
public:
    Derived(int value = 0) : Base(value) {}
    virtual void showType()
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
};

int main()
{
    Derived derived(5);
    derived.showType();

    Base & rb_derived = derived;
    rb_derived.showType();

    Base *pb_derived = &derived;
    pb_derived->showType();

    return 0;
}
```
output:
```cpp
I am Derived, and my value is: 5
I am Derived, and my value is: 5
I am Derived, and my value is: 5
```
这次我们成功了!当`rb_derived`和`pb_derived`调用`showType`时, 一般来说都会被解析成`Base::showType`. 但是这个虚函数机制改变了这一切!

因为`Base::showType`是虚的, 所以`rb_derived`和`pb_derived`调用的`showType`的版本是最新的版本重载, 也就是`Derived::showType`版本.

需要注意的是, 当函数被声明为虚函数后, 派生类如果再有相匹配的函数, 这个相匹配的函数也是虚函数!因此, `Derived`里面的`showType`前面可以不用加上`virtual`, 当然, 还是建议加上, 这样更加清楚的表明这个函数是虚函数.

另外, 虚函数除了要求特征值需要匹配, 还要求返回值匹配! 如果其中一个不匹配, 那么这个派生类的函数就不会被看作是虚函数重写!

还有,就是千万不要在构造函数和析构函数里面使用虚函数, 这样会产生很多的未定义行为!

最后, 我们需要版所有的函数虚化吗? 没有必要, 非必要,不虚化, 除非自己想好了这个函数一定要被派生类重写才用, 否则一律用普通函数. 

因为首先虚函数的心智负担比较重, 第二虚函数在时间上还是在空间上都是不高效的, 因为确定函数版本需要在运行时查表, 并且我们原来的类会大一个指针大小, 这个隐藏的指针指向一个叫做虚函数表的东西.


## `override`和`final`标识符

正如前面所说, 虚函数除了要求特征值需要匹配外, 还需要返回值匹配. 一旦有其中一个不符合就不会被认定为虚函数, 而被当作普通函数处理. 有些时候我们明明想要让这个函数作为基类的虚函数的重写, 但是一不小心就搞砸了, 还没有错误提醒.

不过如果我们用上了`override`标识符, 就可以了. 一旦我们在目标函数使用了`override`, 如果`它不符合虚函数重写, 就会报错.

因此, 我们上面的代码要严谨一些就可以这样:
```cpp
class Derived : public Base
{
public:
    Derived(int value = 0) : Base(value) {}
    virtual void showType() override // add override to specify this function is a override to showTtype
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
};
```

而如果我们不想要这个虚函数继续被派生类重写, 就可以使用`final`标识符, 一旦这个虚函数被重写, 就会报错.

例如如果,我们不想要`Derived`类的`showTtype`继续被重写, 就可以这样:
```cpp
class Derived : public Base
{
public:
    Derived(int value = 0) : Base(value) {}
    virtual void showType() override final // add override to specify this function is a override to showTtype, and add final to specify this function cannot be overriden again!
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
};
```

当然这个标识符也可以用在类上面. 一旦在类名后面加上`final`, 这个类就不能被继承, 例如我不想让`Derived`被继承, 于是可以这样:
```cpp
class Derived final : public Base // add final after the class name to declare that this class cannot be inherited
{
public:
    Derived(int value = 0) : Base(value) {}
    virtual void showType() override // add override to specify this function is a override to showTtype
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
};
```

## 虚析构函数和越过虚拟化访问

在我们使用继承的时候, 我们最好就把我们的基类虚构函数声明为虚函数, 特别是当我们尝试要使用指针和引用多态的时候.

如果没有将基类析构函数, 我们很可能会遇到内存泄漏的问题.

来看看这下面的代码:
```cpp
#include <iostream>

class Base
{
protected:
    int m_value {};
public:
    Base(int value = 0) : m_value(value) {}
    virtual void showType()
    {
        std::cout << "I am Base, and my value is: " << m_value << std::endl;
    }
    ~Base() { std::cout << "destoryed Base..." << std::endl; }
};

class Derived final : public Base
{
public:
    Derived(int value = 0) : Base(value) {}
    virtual void showType() override
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
    ~Derived() { std::cout << "destoryed Derived..." << std::endl; }
};

int main()
{
    Base * pb_derived = new Derived(5);
    pb_derived->showType();

    delete pb_derived;
    return 0;
}
```
output:
```cpp
I am Derived, and my value is: 5
destoryed Base...
```
我们看到, 当要销毁`pb_derived`所指向的资源时, 我们只看到资源的`Base`部分才得以通过`Base`的析构函数释放, 但却没有调用到`Derived`自己部分的析构函数, 也就意味着`Derived`的资源并没有资源被销毁. 虽然这个例子中`Derived`本身并不包含资源, 也就不会出现问题, 但如果`Derived`有储存在堆的资源时, 就危险了, 妥妥的内存泄漏.

为什么会没有调用`Derived`的析构函数呢?

答案是`Base`本身的析构函数是非虚函数, 因此当我们使用`delete`运算符时, 调用的析构函数是基类的析构函数, 没有调用`Derived`的析构函数.

如果我们把`Base`的析构函数声明为虚时, 就不同了.当调用`delete`运算符时, 调用的析构函数就是`Derived`版本的析构函数, 并且之前我们说过, 派生类的析构函数会自动调用基类的虚构函数, 即使我们没有显式指定.因此, 这样无论是`Derived`自己的资源还是`Base`的资源都会得到释放.

我们这就改一下:
```cpp
#include <iostream>

class Base
{
protected:
    int m_value {};
public:
    Base(int value = 0) : m_value(value) {}
    virtual void showType()
    {
        std::cout << "I am Base, and my value is: " << m_value << std::endl;
    }
    virtual ~Base() { std::cout << "destoryed Base..." << std::endl; } // set Base::~Base as virtual
};

class Derived final : public Base
{
public:
    Derived(int value = 0) : Base(value) {}
    virtual void showType() override
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
    virtual ~Derived() override { std::cout << "destoryed Derived..." << std::endl; }
};

int main()
{
    Base * pb_derived = new Derived(5);
    pb_derived->showType();

    delete pb_derived;
    return 0;
}
```
output:
```cpp
I am Derived, and my value is: 5
destoryed Derived...
destoryed Base...
```
果然, 这两个析构函数都得到执行了!

当然, 我这个析构函数是有内容的, 这只是为了能更好的检测, 如果我们不想要虚析构函数有具体内容, 可以这样:
```cpp
virtual ~Base() = defalut; // generate a virtual default destructor
```

有某些时候, 我们就偏偏要让指向派生类的基类指针或者引用能够在虚函数被设置的情况下逆势调用基类的版本.

这中情况其实真的很少出现, 不过稍微提一下方法也行.

那就是使用作用域解析符直接指定目标版本!
还是前面最新的代码, 验证代码我们改一下, 就能够调用基类的版本函数:
```cpp
int main()
{
    Base * pb_derived = new Derived(5);
    pb_derived->Base::showType(); // use the scope operator

    delete pb_derived;
    return 0;
}
```
output:
```cpp
I am Base, and my value is: 5
destoryed Derived...
destoryed Base...
```
根据输出结果, 我们确实调用基类的`showTtype`.

会过来, 我们再来想想到底在什么情况下使用虚析构函数呢?

以下是建议:

- 如果我们真的想要让这个类之后会被继承, 那么就把析构函数虚化
- 如果我们真的不想要这个类被继承, 那么首先就要把这个类设定为`final`, 不用虚化析构函数.
