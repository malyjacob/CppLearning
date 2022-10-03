# Pure Virtual Functions, Abstract Base Classes and Intereface Classes

Cpp 允许我们生成一种特殊的虚函数, 这个虚函数叫做纯虚函数, 或者说抽象函数.

这种函数的一个重要特质就是可以没有函数体!类同于一个占位符.

## 声明纯虚函数

我们只需要在虚函数声明后面加上`= 0`, 就可以让这个虚函数成为纯虚函数!
```cpp
class Base
{
public:
    const char* sayHi() const { return "Hi"; } // a normal non-virtual function

    virtual const char* getName() const { return "Base"; } // a normal virtual function

    virtual int getValue() const = 0; // a pure virtual function

    int doSomething() = 0; // Compile error: can not set non-virtual functions to 0
};
```

使用纯虚函数有以下两个结果:

1. 任何带有纯虚函数的类都会成为抽象基类.而抽象基类就是只能被继承而不能独自实例化的特殊类!

2. 而抽象基类就要求继承类如果不想要成为抽象基类的话, 就必须重写在抽象基类里面的纯虚函数!如果没有完全重写纯虚函数的派生类自己也会成为抽象基类.

抽象基类是十分有用的, 第一就是防止抽象基类被无端继承后就直接使用, 二是能够给我们以更高的抽象能力.

不过有趣的是, 纯虚函数自己是可以实现函数体的!
```cpp
#include <string>

class Animal // This Animal is an abstract base class
{
protected:
    std::string m_name;

public:
    Animal(const std::string& name)
        : m_name{ name }
    {
    }

    std::string getName() { return m_name; }
    virtual const char* speak() const = 0; // The = 0 means this function is pure virtual

    virtual ~Animal() = default;
};

const char* Animal::speak() const  // even though it has a definition
{
    return "buzz";
}
```
虽然如此, 但这并不意味着就可以实例化抽象基类, 并且派生类如果要实例化的话也还是要完全重写这个纯虚函数.


## 接口
Cpp不像Java和C#有专门的接口类型, 但我们可以通过纯虚函数来模拟出接口.

到底怎么模拟呢?

很简单, 就是创建一个没有数据成员只有纯虚函数的抽象基类.

这样的抽象基类就可以随便给派生类继承, 因为这个模拟接口本身就只是一个指针, 这个指针指向一个虚函数表.

一般情况下, 约定俗成, 作为接口的抽象基类的名称前面加上`I`比较好, 这样一看就知道只是用作接口用的.

这下面是一个表示错误记录的接口:
```cpp
class IErrorLog
{
public:
    virtual bool openLog(const char* filename) = 0;
    virtual bool closeLog() = 0;

    virtual bool writeError(const char* errorMessage) = 0;

    virtual ~IErrorLog() {} // make a virtual destructor in case we delete an IErrorLog pointer, so the proper derived destructor is called
};
```
然后我们可以以此来创建某一特定领域的错误记录, 比如说文件系统错误记录, 编译错误记录等等.
