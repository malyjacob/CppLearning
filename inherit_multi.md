# Multi-Inherit

Cpp支持多重继承, 就是一个类可以同时继承多个类.

一般来说, 多继承要谨慎使用.

使用多重继承很容易就会翻车, 特别是对于那些有状态的类的多重继承.

多重继承有两个主要问题:

1. 从两个不同的基类继承同名方法
2. 从两个或多个相关的基类那里继承同一个类的多个实例.

而为了解决这些难缠的问题, 就需要使用好多一些新规则和不同的方法.

这样的多继承十分耗费人的精力.

反正开篇明义, 慎用多重继承, 除非要继承的类本身就是无状态的, 也就是说是接口.

不过还是要讲一下.

这里我们主要探讨如何解决多重继承的一个重要难题-----菱形继承, 然后介绍虚拟继承这个解决办法.

## 菱形继承问题

什么是菱形继承呢?

就是一个类派生出两个或多个类, 然后又有一个新类同时继承其中的两个或两个以上的类.

现在就举个例子:
```cpp
#include <iostream>

class PoweredDevice
{
public:
    PoweredDevice(int power)
    {
		std::cout << "PoweredDevice: " << power << '\n';
    }
};

class Scanner: public PoweredDevice
{
public:
    Scanner(int scanner, int power)
        : PoweredDevice{ power }
    {
		std::cout << "Scanner: " << scanner << '\n';
    }
};

class Printer: public PoweredDevice
{
public:
    Printer(int printer, int power)
        : PoweredDevice{ power }
    {
		std::cout << "Printer: " << printer << '\n';
    }
};

class Copier: public Scanner, public Printer
{
public:
    Copier(int scanner, int printer, int power)
        : Scanner{ scanner, power }, Printer{ printer, power }
    {
    }
};
```
`Copier`继承了`Scanner`和`Printer`这两个类, 而这两个类又是从同一个类`PoweredDevice`派生出来的.

我们写个验证代码:
```cpp
int main()
{
    Copier copier{ 1, 2, 3 };

    return 0;
}
```
output:
```cpp
PoweredDevice: 3
Scanner: 1
PoweredDevice: 3
Printer: 2
```
我们可看到, `PoweredDevice`被构造了两次, 也就是说, 在`Copier`内部又两份`PoweredDevice`相同的副本.

但其实我们的`Copier`只需要一份数据就可, 并且当我们去调用`PoweredDevice`部分的成员时, 到底是调用从`Scanner`继承过来的副本还是`Printer`的? 这也就造成了二义性!

而这就是菱形继承问题.


## 虚继承与虚基类

为了解决这种问题, Cpp给出了虚继承和虚基类的方案.

这里主要解决的问题就是让`Copier`只继承一份`PoweredDevice`部分而不是两份.

现在想想, 到底是直接控制只令`Copier`继承`Scanner`继承的`PoweredDevice`的部分, 还是`Printer`的来保证只取一份`PoweredDevice`?

答案是都不是, 而是直接让`Copier`越过`Scanner`和`Printer`来继承`PoweredDevice`的部分.

而为了实现这一步, 就需要虚继承, 让`PoweredDevice`成为虚基类.

那么, 如何实现虚继承呢?

那么可以先看看如下代码:
```cpp
class PoweredDevice
{
};

class Scanner: virtual public PoweredDevice
{
};

class Printer: virtual public PoweredDevice
{
};

class Copier: public Scanner, public Printer
{
};
```
实现虚继承语法, 就是在继承的冒号后面加上`virtual`关键字.

从以上代码我们可以看出, `Scanner`和`Printer`都虚继承`PoweredDevice`, 因此`PoweredDevice`就是虚基类.

一般来说, 虚继承的效果只能在下一代继承者才能显现出来, 而我们直接实例化一个`Scanner`或`Printer`对象其实就等同于普通的`public`继承.

把焦点放在`Copier`上, 根据此时的继承关系, 此时的`Scanner`和`Printer`其实是共有一套`PoweredDevice`的副本.

然后就是新的继承规则.

在使用虚基类时, 对于非虚基类, 唯一可以出现在初始化列表上的构造函数就是基类构造函数.

但这些构造函数可能需要将信息传递给其基类.

而由于虚基类的存在, 在这个时候, 构造函数传参并且构造的细节会又不同.

最大的不同就是禁止信息通过中间类自动传递给基类, 需要自己将信息传递给虚基类, 如果没有专门指定何种虚基类构造函数, 那么就会自动调用虚基类的默认构造函数.

话还是有些说不明白, 还是上代码实在:
```cpp
#include <iostream>

class PoweredDevice
{
public:
    PoweredDevice(int power)
    {
		std::cout << "PoweredDevice: " << power << '\n';
    }
};

class Scanner: virtual public PoweredDevice // note: PoweredDevice is now a virtual base class
{
public:
    Scanner(int scanner, int power)
        : PoweredDevice{ power } // this line is required to create Scanner objects, but ignored in this case
    {
		std::cout << "Scanner: " << scanner << '\n';
    }
};

class Printer: virtual public PoweredDevice // note: PoweredDevice is now a virtual base class
{
public:
    Printer(int printer, int power)
        : PoweredDevice{ power } // this line is required to create Printer objects, but ignored in this case
    {
		std::cout << "Printer: " << printer << '\n';
    }
};

class Copier: public Scanner, public Printer
{
public:
    Copier(int scanner, int printer, int power)
        : PoweredDevice{ power }, // PoweredDevice is constructed here
        Scanner{ scanner, power }, Printer{ printer, power }
    {
    }
};
```
测试代码还是一个样:
output:
```cpp
PoweredDevice: 3
Scanner: 1
Printer: 2
```
这样的`Copier`对象就只有一个副本了!


## 多重继承只能讲到这里了! 总之使用多重继承要慎之又慎!!!!