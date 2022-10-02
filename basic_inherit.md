# Basic Inheritance in Cpp 

继承常出现在两个或多个类之间. 在`is-a`关系中, 类可以被继承, 而这个类一般就叫做基类, 而继承的类就叫做子类或者派生类.

最简单的例子就苹果和水果之间的关系. 苹果具有水果的一切特征, 并且相比于水果, 它还有自己独特的特征. 而这个时候我们就可以使用继承了, 因为苹果和水果的关系就是典型的`is-a`关系. 我们可以从水果类派生出苹果类来, 苹果类继承了水果类的所有实现和接口, 并且还有专属于自己的数据成员和接口.

## 如何实现继承

这里有一个简单的`person`类:
```cpp
class Person
{
public:
    std::string m_name {};
    int m_age {};

    Person(const char *name = "", int age = 0)
        : m_name(name), m_age(age) {}

    const std::string & getName() const { return m_name; }
    int getAge() const { return m_age; }
};
```

然后我们又再写出一个类`BaseballPlayer`:
```cpp
class BaseballPlayer
{
public:
    double m_battingAverage {};
    int m_homeRuns {};

    BaseballPlayer(double batingAverage = 0.0, int homeRuns = 0)
        : m_battingAverage(batingAverage), m_homeRuns(homeRuns) {}
};
```

好的, 我们现在想要给`BaseballPlayer`类添加`name` 和 `age`成员来表示棒球员的名字
和年龄.

而我们又三种选择:

1. 为`BaseballPlayer`类直接加上这两个成员. 但这个选择似乎不太明智, 因为这两个早就再`Person`类声明好了.

2. 把`Person`类对象作为成员. 这个想法虽然能复用`Person`类, 但是代价是让代码变得比较别扭, 你不会以为`BaseballPlayer`有`Person`吧? 而应该是`BaseballPlayer`是`Person`.

3. 让`BaseballPlayer`继承`Person`, 使得它能够获得`Person`的属性.

很显然, 第三种选择才是正确的. 那么如何实现`BaseballPlayer`继承`Person`呢?

就是这样:
```cpp
class BaseballPlayer : public Person
{
public:
    double m_battingAverage {};
    int m_homeRuns {};

    BaseballPlayer(double batingAverage = 0.0, int homeRuns = 0)
        : m_battingAverage(batingAverage), m_homeRuns(homeRuns) {}
};
```
我们通过在类名后面加上冒号和`public`关键字和`Person`名就可以实现简单的继承了.

那么, 我们就看看效果如何:
```cpp
int main()
{
    BaseballPlayer joe {};
    joe.m_name = "Joe";

    std::cout << joe.getName() << std::endl;
}
```
outpout:
```cpp
Joe
```
我们看到, `BaseballPlayer`对象`joe`成功拥有了name属性! 说明`BaseballPlayer`类确实继承了`Person`类.
<br/>

## 派生类的构造函数执行顺序
让我们再构造一个继承的例子:
```cpp
class Base
{
public:
    int m_id {};
    Base(int id = 0) : m_id(id) { std::cout << "Base" << std::endl; }
    int getId() const { return m_id; }
};


class Derived: public Base
{
public:
    double m_cost {};
    Derived(double cost = 0.0) : m_cost(cost) { std::cout << "Derived" << std::endl; }
    double getCost() const {  return m_cost; }
};
```
因为`Derived`继承了`Base`, 我们很可能认为是`Base`的成员是直接复制进`Derived`的.

但是, 这是不对的. 真正的情况应该是`Derived`有两个部分, 一部分是自有的部分, 一部分是`Base`的内容.

我们可以验证一番:
```cpp
int main()
{
    Derived derived;
    return 0;
}
```
outpout:
```cpp
Base
Derived
```
我们惊奇的发现, 但我们构造`Derived`对象时, `Base`的部分先被构造, 然后才轮到自己!

实际上, 当存在继承关系的类被构造时, 在继承树最根部的类的部分总是最先被构造,然后依次才轮到此类的部分的构造.


## 派生类的初始化细节

让我们来看看, 没有继承时候, 类对象是如何被构建出来的:
```cpp
int main()
{
    Base base(5);
    return 0;
}
```
outpout:
```cpp
Base
```
下面是这个对象初始化的细节过程:

1. 为base开出一段足够的空间
2. 调用`Base`构造函数
3. 成员初始化列表初始化成员
4. 执行构造函数体内的语句
5. 最后将控制权返回给调用者

而对于上面的派生类的构建过程则相对复杂一些:

1. 为derived开出一段足够的空间(包括基类Base部分)
2. 调用`Derived`构造函数
3. 先调用基类部分的构造函数, 如果没有显式指出要用哪个构造函数, 就使用基类的默认构造函数
4. 成员初始化列表初始化成员
5. 执行构造函数体内的语句
6. 最后将控制权返回给调用者 

我们可以看到, 派生类对象的构建过程多了一个调用基类构造函数的步骤, 并且基类的构造函数总是先行于派生类的.


## 派生类初始化基类部分成员
现在这个`Derived`有一个缺点, 那就是无法通过自己的构造函数去初始化基类部分的成员`m_id`. 那么我们应该如何解决这个问题呢?

试看看这个:
```cpp
class Derived : public Base
{
public:
    double m_cost {};
    Derived(double cost = 0.0, int id = 0)
        : m_cost(cost), m_id(id) {}
    //.....
} 
```
这个是个好的尝试, 但是Cpp却不支持这种行为, 因为Cpp禁止派生类的初始化列表初始化被继承而来的成员.

为什么这样呢? 因为`Derived`构造函数调用开始时就要先调用基类的构造函数初始化基类部分, 而基类的构造函数已经初始化了`m_id`了, 而一个变量是不允许被初始化两次的! 被初始化后, 这个变量就只能通过赋值或者其他诸如指针等手段来改变.

好的, 知道这个教训后, 我们又有了一个新的写法:
```cpp
class Derived : public Base
{
public:
    double m_cost {};
    Derived(double cost = 0.0, int id = 0)
        : m_cost(cost) { m_id = id; }
    //.....
}
```
这次, 这个能正常工作了, 但不具有普适性.

如果, `m_id`是`const`值或者引用, 就巴比Q了, 因为, `const`值和引用必须在声明时就初始化! 也就是说, 必须通过构造函数的成员初始化列表来初始化.

并且这样搞效率不高, 因为这个`m_id`被赋予内容两次, 第一次是调用基类构造函数时, 第二次就是在派生类函数体内部.

并且, 如果这个`m_id`是基类的`private`成员这个也不成立, 因为`private`成员只能被基类的公共成员和在基类内部直接使用. 而派生类很显然不是基类, 因此,`Derived`类就无权访问修改`m_id`!

那么到底该怎么办?

答案就存在基类的构造函数里! 

我们可以在调用派生类构造函数时直接指定我们具体要调用哪个版本的基类构造函数, 然后把派生类构造函数参数表中要传送到基类构造函数的参数传入到这个刚指定的基类构造函数, 就像我们的成员初始化列表的操作那般.

因为, 一般构造函数都是可以被外部访问到的, 所以我们就可以这样合法的操作, 并且也避开了前面那个版本的问题.

好的, 闲话少说, 上代码:
```cpp
class Derived : public Base
{
public:
    double m_cost {};
    Derived(double cost = 0.0, int id = 0)
        : Base(id), m_cost(cost) { m_id = id; }
    //.....
}
```
我们来测试一番:
```cpp
int main()
{
    Derived derived(1.3, 5);
    std::cout << "Id: " << derived.getId() << std::endl;
    std::cout << "Cost: " << derived.getCost() << std::endl;

    return 0;
}
```
outpout:
```cpp
Id: 5
Cost: 1.3
```
我们成功了!

现在我们可以不用再把数据成员暴露在外面了, 把它放到`private`也是完全没有问题的, 并且也符合封装理念.
```cpp
#include <iostream>

class Base
{
    int m_id {};
public:
    Base(int id = 0) : m_id(id) { std::cout << "Base" << std::endl; }
    int getId() const { return m_id; }
};


class Derived: public Base
{
    double m_cost {};
public:
    Derived(double cost = 0.0, int id = 0) : Base(id), m_cost(cost) { std::cout << "Derived" << std::endl; }
    double getCost() const {  return m_cost; }
};

int main()
{
    Derived derived(1.3, 5);
    std::cout << "Id: " << derived.getId() << std::endl;
    std::cout << "Cost: " << derived.getCost() << std::endl;

    return 0;
}
```

## 析构函数执行顺序

当我们的派生类过期时, 程序会先调用派生类的析构函数, 然后再调用基类的构造函数, 这是与构造函数的顺序反过来的.

我们可以专门为`Derived`和`Base`配置一个析构函数:
```cpp
class Base
{
    int m_id {};
public:
    Base(int id = 0) : m_id(id) { std::cout << "Base" << std::endl; }
    int getId() const { return m_id; }
    ~Base() { std::cout << "delete Base...." << std::endl; } // add destructor
};


class Derived: public Base
{
    double m_cost {};
public:
    Derived(double cost = 0.0, int id = 0) : Base(id), m_cost(cost) { std::cout << "Derived" << std::endl; }
    double getCost() const {  return m_cost; }
    ~Derived() { std::cout << "delete Derived...." << std::endl; } // add destructor
};
```
测试代码还是一样, 输出结果是这样子的:
```cpp
Base
Derived
Id: 5
Cost: 1.3
delete Derived....
delete Base....
```
果然如此!
<br/>

## 继承类动态内存管理
现在来看看继承类的动态内存管理如何.

一般情况下, 会分为两种情况. 不同的情况, 继承类的动态内存分配也有所不同.

### 情况一: 派生类成员不使用`new`分配内存
假设基类是这样子的:
```cpp
class BaseDMA
{
private:
    char *label;
    int rating;
public:
    BaseDMA(const char *lb = "null", int rt = 0) : rating(rt)
    {
        int len = strlen(lb);
        label = new char[len + 1];
        strcpy(label, lb); 
    }
    BaseDMA(const BaseDMA & base) : rating(base.rating)
    {
        int len = strlen(base.label);
        label = new char[len + 1];
        strcpy(label, base.label);
    }
    BaseDMA & operator= (const BaseDMA & base)
    {
        rating = base.rating;
        delete[] label;
        int len = strlen(base.label);
        label = new char[len + 1];
        strcpy(label, base.label);
        return *this;
    }
    ~BaseDMA() { delete[] label; }
};
```

而我们的派生类是这样子的:
```cpp
class LackDMA : public BaseDMA
{
    char color[40];
public:
    //.....
};
```
那么, 这个派生类是否需要自定义显式的析构函数, 复制构造函数和赋值运算符吗?

答案是不需要.

首先, 如果没有定义析构函数, 编译器会定义一个不执行任何操作的析构函数, 但这个析构函数总要进行一些必要的操作, 在派生类这里就是,在析构函数执行完后, 它会调用基类的析构函数. 所以基类部分的堆资源可以得到释放.

接下来看复制构造函数, 默认复制构造函数执行成员复制, 而对于派生类的默认复制构造函数, 它只能操作自己的那个部分, 没有权限去复制基类的那个部分, 于是就需要调用基类的复制构造函数. 因此编译器给派生类的复制构造函数是会自己调用基类的复制构造函数的, 而这也是合适的.

最后来看赋值运算符. 同派生类的默认复制构造函数一个道理, 编译器会贴心的给我们会自己调用基类赋值运算符的赋值运算符函数, 因此这也是合适的.

### 第二种情况: 派生类成员使用了`new`分配内存
假设基类跟第一种情况一样, 而派生类是这样子的:
```cpp
class HasDMA : public BaseDMA
{
    char *style;
public:
    //...
}
```
在这种情况下就必须要显式指定派生类的析构函数, 复制构造函数和赋值运算符.

因为`style`是动态内存分配的, 所以必须要指定析构函数释放掉`style`所指向的资源.
```cpp
HasDMA::~HasDMA() { delete[] style; }
```

当然复制构造函数也要特别指定, 并且我们要自己手动去调用基类的复制构造函数:
```cpp
HasDMA::HasDMA(const HasDMA & has) : BaseDMA(has) // `has` upcasting..
{
    int len = strlen(has.style);
    style= new char[len + 1];
    strcpy(style, has.style); 
}
```
这里面直接使用了`BaseDMA`的复制构造函数, 这里面的`has`虽然不是`BaseDMA`类型, 但由于是派生类型对象, 因此这里面进行了隐式向上类型转换为`BaseDMA`对象, 这个具体原理会在之后的章节再来细讲.

然后是赋值运算符:
```cpp
HasDMA::HasDMA & operator= (const HasDMA & has)
{
    if(this == &has)
        return *this;
    BaseDMA::operator=(has); // copy base portion
    delete[] style;
    style = new char[strlen(has.style) + 1];
    strcpy(style, has.style);
    return *this;
}
```
这里面也需要手动调用基类的赋值运算符, 并且也进行了跟复制构造函数一样的隐式向上类型转换.