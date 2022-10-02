# Inheritance and access speciigiers

在这一章节, 我们会探究包括`public`继承的三种继承方式, 并且了解这三种不同的继承方式如何影响派生类的访问机制.

## `protected`访问标识符

`protected`是区别于`public`和`private`的第三种访问标识符.

而这个标识符只用于处于继承关系上的类.

`protected`的作用是, 标有`protected`的类成员, 在类外的访问机制跟`private`一样, 都不能直接访问, 要靠公有方法才能在外部访问, 但与`private`不同的是, `protected`的成员除了能够在本类内可以直接访问外, 还可以在派生类中直接访问, 类同于`public`. 总之,`protected`就是介于`private`和`public`的存在.

这下面是验证代码:
```cpp
#include <iostream>

using namespace std;

class Base
{
public:
    int m_public {};
protected:
    int m_protected {};
private:
    int m_private {};
};

class Derived : public Base
{
public:
    Derived()
    {
        m_public = 1; // allowed
        m_protected = 2; // allowed
        m_private = 3; // not allowed
    }
};

int main()
{
    Base base;
    base.m_public = 1;
    base.m_protected = 2; // not allowed
    base.m_private = 2; // not allowed

    return 0;
}
```

`protected`的一个妙用就是让一个类成为只能被继承而不能被实例化的类, 用法就是把这个类的构造函数放到`protected`里面去.这样我们就不能在外面去使用构造函数实例化这个类, 但我们可以在派生类里面使用这个`protected`构造函数, 在派生类里面构造出基类的部分.

那么何时我们应该使用它呢?

这首先要明确了解, 相比于`private`, 我们的派生类可以直接访问`protected`对象, 而不用像`private`成员一样, 需要靠基类的公有方法才能访问到, 而构建这些公有方法是有开销的.

而相比于`public`, 我们可以相对有效的屏蔽外面对`protected`成员的直接影响, 一定程度上说也确保了数据的安全性.

一般来说,除了出现继承现象并且构建公有方法访问成员的开销比较大时我们可以选择`protected`, 我们用`private`.
<br/>

## 不同继承类型和它们对访问权限的影响
首先, 让我们这三个继承啥样, 这三者是`private`继承, `public`继承和`protected`继承:

```cpp
class Pub : public Base
{};

class Pro : protected Base
{};

class Pri : private Base
{};

class Def : Base
{}; // Default to private inhertance
```
如果我们不指定继承类型的话, 编译器会认为这是`private`继承.

### public inhertance

`public inhertance`是我们最常用到的继承类型, 并且它也是三者中最为简单的继承模式.

放一张表来直接看看公有继承对派生类的访问性的影响:

| Access specifier in base class | Access specifier when inherited publicly |
| :---: | :---: |
| public | public |
| protected | protected |
| private| inaccessible |

具体的验证代码我不做了, 反正公有继承是我们最应该使用的, 除非我们面临某些特殊原因.

### protected inhertance

`protected inhertance`是最不常用的继承方式, 它的继承规律就是把基类原来是公有的成员统一变为派生类的`protected`成员.

| Access specifier in base class | Access specifier when inherited protectedly |
| :---: | :---: |
| public | protected |
| protected | protected |
| private| inaccessible |

### private Inheritance

至于私有继承, 则凡是原来基类公有或者保护的成员在派生类里都是私有成员.

| Access specifier in base class | Access specifier when inherited privately |
| :---: | :---: |
| public | private |
| protected | private |
| private| inaccessible |

私有继承有时候挺有用的, 特别是当遇到基类与派生类关系不大的时候或者基类就是个小型类, 我们可以通过私有多继承把基类包起来, 实现类似组合的效果.

## 总结

继承最好别乱玩花样, 非必要不使用非`public`继承.