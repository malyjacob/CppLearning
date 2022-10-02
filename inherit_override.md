# Overriding Behavior in Inheritance

在这部分, 我们将讨论在继承中的成员函数的重写行为.

当一个成员函数被调用时, 编译器会先检查这个成员函数在不在这个类当中.

如果不在, 那么编译器将会跳转到它的父类去寻找匹配的成员函数并且去调用这个成员函数.

来看看这个例子:
```cpp
#include <iostream>

class Base
{
protected:
    int m_value {};

public:
    Base(int value) : m_value(value) {}

    void identity() const { std::cout << "I am a Base" << std::endl; }
};

class Derived : public Base
{
public:
    Derived(int value) : Base(value) {}
};

int main()
{
    Base base(5);
    base.identity();

    Derived derived(7);
    derived.identity();

    return 0;
}
```
output:
```cpp
I am a Base
I am a Base
```
果然如此, `derived`没有自己对`indentity`的定义, 因此这个`indentity`其实是`Base`的成员函数, 因为被`Derived`继承, 所以`derived`可以去调用它.

## 重定义行为
然而, 我们似乎对这个不是十分满意, 因为我们想要让`identity`在`Derived`类对象中的表现跟`Base`的不同.

那么, 我们可以在`Derived`里重新定义`identity`函数.
看看这个改进版`Derived`类:
```cpp
class Derived : public Base
{
public:
    Derived(int value) : Base(value) {}
    void identity() const { std::cout << "I am a Derived" << std::endl; }
};
```
还是原来的验证代码, 但结果就有所不同了:
```cpp
I am a Base
I am a Derived
```
当`derived`调用`identity`时, `Derived`的`identity`的版本掩盖了`Base`类的`indentity`, 因此这个输出就与上面的不同.

## 派生类函数调用基类被掩盖的函数
有些时候, 我们并不想要完全让派生类的同名同特征值的函数取代基类版的, 而是想要在新版本函数中使用基类被掩盖的版本.

这其实也很简单, 就是在这个函数前面加上基类的作用域解析运算符:
```cpp
class Derived : public Base
{
public:
    Derived(int value) : Base(value) {}
    void identity() const 
    { 
        Base::identity(); // if identity(), error!!!!!!, must add the Base:: before it. 
        std::cout << "I am a Derived" << std::endl; 
    }
};
```
output:
```cpp
I am a Base
I am a Base
I am a Derived
```
我们一定要加上这个作用域解析运算符, 要是不加上的话, 那个`indentity`就会被编译器认定为`Derived`版本的`indentity`, 从而导致无限循环!

## 继承与友元
有些时候我们想要让派生类成员函数能够去调用基类的友元函数, 而上面的这几种方法都是无效的.

因为友元函数并不是基类的一部分, 于是基类的友元函数也不会被派生类所继承.
于是我们无法通过上面的作用域解析运算符解决问题.

并且基类的友元函数只认基类!

那么该怎么办呢?

答案是让派生类强制类型转换为基类, 这样派生类就可以调用基类的友元函数了!
```cpp
#include <iostream>

class Base
{
protected:
    int m_value {};

public:
    Base(int value) : m_value(value) {}

    void identity() const { std::cout << "I am a Base" << std::endl; }

    friend std::ostream & operator<< (std::ostream & os, const Base & b)
    {
        os << "In Base" << std::endl;
        os << b.m_value << std::endl;
        return os;
    }
};

class Derived : public Base
{
public:
    Derived(int value) : Base(value) {}
    void identity() const { Base::identity(); std::cout << "I am a Derived" << std::endl; }
    friend std::ostream & operator<< (std::ostream & os, const Derived & d)
    {
        os << "In Derived" << std::endl;
        os << static_cast<const Base&>(d); // static_cast Derived to a Base object, so we call the right version of operator<<
        return os;
    }
};

int main()
{
    Derived derived(7);

    std::cout << derived << std::endl;
    
    return 0;
}
```
output:
```cpp
In Derived
In Base
7

```
