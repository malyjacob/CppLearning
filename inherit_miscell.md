# 继承相关的杂项

这部分就讲讲与继承相关的边角.

## 类型转换时的裁剪
前面我们已经知道了如何使用基类指针或引用指向派生类, 那么假如说如果我把一个派生类初始化或者赋值给一个基类对象会怎么样?

> 首先提醒一下, 上面这种情形是合法的, 但反过来将一个基类对象初始化或者赋值给派生类对象一般是不被允许的, 除非我们自己提供相关的复制构造函数和赋值运算符.

答案是会被裁剪成基类的模样.

要具体说来, 就需要知道, 当我们遇到这种情况, 一般会调用基类的复制构造函数或者赋值运算符函数.

而这两个长啥样?

    复制构造函数: Type(const Type & obj);
    复制运算符 :  Type & operator= (const Type & obj);

那么在这个具体情况下, 这个`obj`是个基类引用, 而这个基类引用则指向派生类对象.

这个传参是合法的, 然后就是执行这个基类的这两个函数的内容.

就是这么一回事:
```cpp
#include <iostream>

class Base
{
private:
    int m_value {};
public:
    Base(int value) : m_value(value) {}
    virtual ~Base() = default;
    Base(const Base & base)
    {
        std::cout << "using copy constructor..." << std::endl;
        m_value = base.m_value;
    }
    Base & operator= (const Base & base)
    {
        std::cout << "using operator = ..." << std::endl;
        m_value = base.m_value;
        return *this;
    }
};

class Derived final : public Base
{
public:
    Derived(int value) : Base(value) {}
    virtual ~Derived() override = default; 
    Derived(const Derived &) = delete;
    Derived & operator=(const Derived &) = delete;
};

int main()
{
    // using copy constructor
    Base base1 = Derived(5);

    Derived derived(7);

    // using operator =
    base1 = derived;
}
```
output:
```cpp
using copy constructor...
using operator = ...
```
从这里我们可以断定就是调用了基类这两个函数!

只复制或复制`Derived`的`Base`部分给这个`Base`对象, 而`Derived`的部分则被忽略.

## Dynamic Casting

在处理多态性时，我们经常会遇到这样的情况：我们有一个指向基类的指针，但我们想访问一些只存在于派生类中的信息。

先来看看这段代码:
```cpp
#include <iostream>
#include <string>

class Base
{
protected:
	int m_value{};

public:
	Base(int value)
		: m_value{value}
	{
	}

	virtual ~Base() = default;
};

class Derived : public Base
{
protected:
	std::string m_name{};

public:
	Derived(int value, const std::string& name)
		: Base{value}, m_name{name}
	{
	}

	const std::string& getName() const { return m_name; }
};

Base* getObject(bool returnDerived)
{
	if (returnDerived)
		return new Derived{1, "Apple"};
	else
		return new Base{2};
}

int main()
{
	Base* b{ getObject(true) };

	// how do we print the Derived object's name here, having only a Base pointer?

	delete b;

	return 0;
}
```
在这段代码中, `getObject()`总是返回`Base`指针, 但是这个指针所指向的对象却是多样的, 可能是`Base`对象, 还有可能是`Derived`对象.

那么就存在一个问题我们该怎样才能通过这个指针调用`Derived::getName()`?

其中一个办法就是将`getName`声明为虚函数, 这样确实行, 但是这样又会出现一个问题, 那么我们该怎么通过这个指针调用`Base:getName()`呢?

更何况这样做还会破坏原有的代码结构, 并且如果我们继承的是不展示实现细节或者无法修改源码的对象, 那么这个方法也没用了.

所以我们需要一个类型转换, 其中有个转换函数是常用到的, 那就是`dynamic_cast`.

当我们需要向下转换时, 也就是由基类转换为派生类时, `dynamic_cast`最为有用.

让我们看看`dynamic_cast`的用法:
```cpp
int main()
{
	Base* b{ getObject(true) };

	Derived* d{ dynamic_cast<Derived*>(b) }; // use dynamic cast to convert Base pointer into Derived pointer

	std::cout << "The name of the Derived is: " << d->getName() << '\n';

	delete b;

	return 0;
}
```
output:
```cpp
The name of the Derived is: Apple
```

当然这种转换会有转换失败的情况, 例如当这个`Base`类指针确实是指向`Base`类对象时, 如果我们用`dynamic_cast`尝试转换为`Derived`就会转换失败.

而对于指针类型的转换来说, 转换失败并不终止程序报错, 而是会返回一个`null`指针.

因此,当我们使用`dynamic_cast`向下转换时, 要在使用转换结果前检验这个结果是否为`null`, 这是十分重要的.

因此, 上面的验证代码可以改进为这样:
```cpp
int main()
{
	Base* b{ getObject(true) };

	Derived* d{ dynamic_cast<Derived*>(b) }; // use dynamic cast to convert Base pointer into Derived pointer

	if (d) // make sure d is non-null
		std::cout << "The name of the Derived is: " << d->getName() << '\n';

	delete b;

	return 0;
}
```

`dynamic_cast`还支持引用值得转换, 一般也用于向下转换.
不过此时当`dynamic_cast`转换失败时, 却不会返回空指针, 因为转换结果期待的时引用, 而引用不是指针.
它此时会生成一个异常, 这个异常我们可以`去捕获它, 不过异常是后面章节的内容了.

> 注意! `dynamic_cast`向下转换会不起作用的情况有:

- 非`public`继承
- 对于没有声明或继承任何虚函数的类（因此没有虚表)
- 在某些涉及虚拟基类的情况下

## dynamic_cast vs static_cast

其实我们也可以使用`static_cast`类型转换, `dynamic_cast`与其的最为重要的区别在于`static_cast`是在编译期间就完成了转换, 而`dynamic_cast`需要在运行时阶段转换. 因此,使用`static_cast`转换就比较快.

但反过来, 使用`static_cast`转换就相对不是十分安全, 因为如果我们用它来把`Base`指针转换为`Derived`指针, 而这个`Base`指针确实是指向`Base`对象本身时, 在`dynamic_cast`会返回一个空指针, 但在`static_cast`这里, 由于没有动态监测, 无论如何都会转换"成功", 而如果我们使用这个`static_cast`就会产生未定义行为.

综上, 当我们考虑向上转换时, 用`static_cast`, 当尝试向下转换时, 用`dynamic_cast`.