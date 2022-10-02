# Object and Class

> ***相关章节***
## [使用对象](./use_class.md)

## [类与动态内存分配](./dynamic_class.md)


## 面向对象编程

面向对象编程是一种特殊的， 设计程序的概念性方法， 它有以下重要特性：

- 抽象
- 封装和数据隐藏
- 多态
- 继承
- 代码的可复用性

这个部分将分为几章来介绍。

现在就来入门。

## 类

定义类一般由两个部分组成。

- 类声明： 数据成员描述类的数据部分， 成员函数描述类的公用接口。
- 类的方法定义： 描述如何实现类成员函数。

现在来现写一个例子：
```cpp
#ifndef CLASS_H_
#define CLASS_H_

#include <iostream>
#include <string>

using ::std::string;

class Circle
{
private:
    double rand_;
    string color_;
    double x_index_;
    double y_index_;
public:
    void init(const string &color, double rand, double x_index, double y_index);
    void change_color(const string &color);
    void move_to(double x_index, double y_index);
    void scalar(double d);
    void show();
    double square();
    const string &get_color() { return color_; }
    double get_rand() { return rand_; }
    double get_x_index() { return x_index_; }
    double get_y_index() { return y_index_; }
};

#endif
```
声明类需要关键词`class`, 这个类表示的是在坐标系里面的一个圆.

可以看到， 这里有`private`和`public`关键词， 这两个词是用来控制访问用的， 还有另外一个`protected`, 这个待会儿再讲。

使用类对象的程序都可以直接访问类对象的公有成员， 但只能通过共有成员函数来访问对象的私有成员。

一般来说， 类设计要尽量将公有接口与实现细节分离， 把数据项放进`private`里面，来实现数据隐藏。 而Cpp 默认类成员是私有的， 所以在声明私有成员时， 可以不用带`private`.

上面的是类的相关声明， 现在要来实现类成员函数了！
```cpp
void Circle::init(const string &color, double rand, double x_index, double y_index)
{
    if(rand_ < 0)
        abort();
    rand_ = rand;
    color_ = color;
    x_index_ = x_index;
    y_index_ = y_index;
}

void Circle::change_color(const string &color)
{
    color_ = color;
}

void Circle::move_to(double x_index, double y_index)
{
    x_index_ = x_index;
    y_index_ = y_index;
}

void Circle::scalar(double scale)
{
    if(scale <= 0)
        return;
    rand_ *= scale;
}

void Circle::show()
{
    cout << "rand: " << rand_ << endl;
    cout << "color: " << color_ << endl;
    cout << "x_index: " << x_index_ << endl;
    cout << "y_index: " << y_index_ << endl;
    cout << endl;
}

double Circle::square()
{
    return PI * rand_ * rand_;
}
```
首先， 成员函数的函数头使用作用域解析（::)来指出函数所属的类。

类方法的完整名中包括类名。 `Circle::show()`是函数的限定名， 而`show()`则是全名的缩写， 它只能用于类作用域中。

可以看到， 方法可以直接访问类的私有成员。

然后回到， 上面的类声明， 可以发现这里将一些函数直接定义在类作用域中， 这种函数称为`内联函数`, 类声明常常将短小的成员函数作为內联函数。

当然也可以类外定义内链函数， 例如：
```cpp
inline double Cricle::get_rand()
{
    return rand_;
}
```

由于內联函数要求每个使用它们的文件中都要对其定义， 所以最好就直接将类成员內联函数定义在类里面。

需要注意：
> 所创建的每一个新对象都有自己的存储空间， 用于储存内部变量和类成员， 但同一个类的所有对象都共用一套类方法。

## 使用类

现在就来使用以下类：
```cpp
#include "class.hpp"

int main()
{
    cout << "begin" << endl;
    Circle c1, c2;
    c1.init("red", 1, 0, 0);
    c2.init("blue", 2, 12, 5);

    c1.show();
    c2.show();

    cout << "c1's rand: " << c1.get_rand() << endl;
    cout << "c2's rand: " << c2.get_rand() << endl;

    cout << "scalar...." << endl;
    c1.scalar(1.5);
    c2.scalar(3);

    cout << "c1's rand: " << c1.get_rand() << endl;
    cout << "c2's rand: " << c2.get_rand() << endl;

    cout << "c1's square: " << c1.square() << endl;
    cout << "c2's square: " << c2.square() << endl;

    return 0;
}
```
output:
```cpp
begin
rand: 1
color: red
x_index: 0
y_index: 0

rand: 2
color: blue
x_index: 12
y_index: 5

c1's rand: 1
c2's rand: 2
scalar....
c1's rand: 1.5
c2's rand: 6
c1's square: 7.065
c2's square: 113.04
```

## 构造函数与析构函数

上面这个类， 确实不错， 但可惜的是我们不能在定义一个`Circle`类对象时就立马初始化它， 只能后面调用`int`方法初始化对象。

并且我们又不能像结构一样， 直接给数据成员赋值， 因为这些数据成员是私有的， 只能通过公有函数才能访问得到。

而现在我们就要学习构造函数， 它能够让我们实现这个目标。

先来看看构造函数怎么样：
```cpp
//我们先在类内部的`public`部分声明构造函数原型。
class Circle
{
    //....
public:
    // ....
    Circle(const string &color, double rand, double x_index, double y_index);
}

//然后在类外实现， 当然也可以直接在类内部定义, 在内部可以不用作用域解析符。
Circle::Circle(const string &color, double rand, double x_index, double y_index)
{
    if(rand_ < 0)
        abort();
    rand_ = rand;
    color_ = color;
    x_index_ = x_index;
    y_index_ = y_index;
}
 //.....
```
可以看到，`Circle`的构造函数与类名同名， 并且还没有返回值， 当然这并不是说构造函数是`void`函数。
这个在声明时还是定义时都是如此。

使用构造函数有两种：
- 显式调用：`Circle c1 = Circle("red", 4, 1, 5);`
- 隐式调用： `Circle c2("blue", 3, 12, 5);`

其实，在没有自定义构造函数的时候， 编译器已经给我们定义了一个默认构造函数， 这个构造函数支持我们不用输入参数就可以初始化对象。

因此， 那个时侯，可以这样`Circle c1, c2;`，这个写法是隐式调用了默认构造函数, 不用写括号。但是现在定义了这个构造函数后， 那样就会报错。

因为， 当我们自己定义了构造函数后， 编译器就不会给我们定义一个默认构造函数了， 于是初始化类对象， 就必须使用自己定义的构造函数， 但是现在这个构造函数是需要传参的！

那么， 只能我们自己为这个类定义一个默认构造函数。有两种途径：

- 重载一个构造函数， 这个构造函数没有参数
- 使用默认参数，  将之前定义的构造函数的参数都赋给一个默认值。

我现在选择使用第二个途径：
```cpp
class Circle
{
    //....
public:
    // ....
    Circle(const string &color = "red", double rand = 1, double x_index = 0, double y_index = 0);
}
```

## 析构函数
用构造函数创建对象后， 程序会追踪这个对象， 直到其过期为止。

而对象过期时， 程序会自动调用析构函数， 这个特殊的成员函数。

一般情况下， 跟构造函数的情况一样， 在没有自定义析构函数时， 编译器会给我们一个默认的析构函数， 因此在对象过期时， 总是能调用析构函数。

现在我们来自定义一个析构函数：
```cpp
class Circle
{
    //....
public:
    // ....
    ~Circle() {}
}
```

和构造函数一样， 没有返回值， 函数名是类名加上前面的`~`。

> 注意： 析构函数不接受参数

那么怎么使用析构函数呢？

答案是不要显式的调用析构函数， 程序会自动调用。

## this 指针

Cpp 还为类专门提供了一个`this`指针， 用来指向用来调用成员函数的对象。

例如我们再为`Circle`类加一个函数， 用来比较两个`Circle`对象的半径大小， 返回半径较大的那个圆。

```cpp
class Circle
{
    //.....
public:
    const Circle &biggerOne(const Circle &c) const
    {
        return this->rand_ > c.rand_ ? *this : c;
    }
}
```

注意到， 除了我们用了`this`, 还有个括号后面的`const`.

这个代表什么呢？

就是是代表这个对象函数的调用将不会改变调用这个函数的对象的数据。

如果不带有这个`const`, 那么编译器就默认这个成员函数会改变调用对象，而我的这个函数却要返回`const`的`Cricle`对象引用， 这个是矛盾的， 而编译器也会报错。

如果编写成员函数时， 如果你这个函数不打算改变调用对象， 就最好在函数的括号后面加上`const`,这绝对是有好处的。