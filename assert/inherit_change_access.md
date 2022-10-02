# Change Member's Accessibility in Derived Class

Cpp能让我们在派生类中改变被继承过来的成员的访问性.

这个的实现还得归功于`using`声明. 通过它来重新指定在派生类中基类成员的访问等级.

例如这个:
```cpp
#include <iostream>

class Base
{
private:
    int m_value {};
public:
    Base(int value) : m_value(value) {}
protected:
    void printValue() const { std::cout << m_value << std::endl; }
};

class Derived : public Base
{
public:
    Derived(int value) : Base(value) {}
    
    // Base::printValue was inherited as protected, so the public has no access
    // But we're changing it to public via using declaration
    using Base::printValue;
};

int main()
{
    Derived derived(7);

    // it will work! printValue is public in Derived
    derived.printValue();

    return 0;
}
```
output:
```cpp
7
```
在这里通过`using`声明, 我们将原本在基类中是`protected`的`printValue`改造成在`Derived`是`public`的成员函数! 不过基类本身的访问结构是没有变化的, 在基类中, `printValue`依然属于`protected`成员!

## 隐藏成员函数
在cpp里, 我们不可能通过派生类为基类去除成员函数或者加上访问限制, 也就是说我们不能通过派生类改变基类, 除非直接修改基类源码.

但是,我们确实可以通过`using`声明做到在派生类中隐藏基类成员函数的效果.

看这段代码:
```cpp
#include <iostream>
class Base
{
public:
	int m_value {};
};

class Derived : public Base
{
private:
	using Base::m_value;

public:
	Derived(int value)
	// We can't initialize m_value, since it's a Base member (Base must initialize it)
	{
		// But we can assign it a value
		m_value = value;
	}
};

int main()
{
	Derived derived { 7 };

	// The following won't work because m_value has been redefined as private
	std::cout << derived.m_value;

	return 0;
}
```
在这段代码中, 我们做到了将`Base`类原本的公有成员`m_value`变成了`Derived`类的私有成员, 从而实现在外部不能直接通过`Derived`类对象来访问`m_value`的效果.

其实还有一种实现, 这个实现更加明显, 例如:
```cpp
#include <iostream>
class Base
{
private:
	int m_value {};

public:
	Base(int value)
		: m_value { value }
	{
	}

	int getValue() const { return m_value; }
};

class Derived : public Base
{
public:
	Derived(int value)
		: Base { value }
	{
	}


	int getValue() = delete; // mark this function as inaccessible
};

int main()
{
	Derived derived { 7 };

	// The following won't work because getValue() has been deleted!
	std::cout << derived.getValue();

	return 0;
}
```
我们在`Derived`类内部将`getValue`直接等于`delete`, 这里的意思就是`Derived`类对象禁用`getValue`函数.