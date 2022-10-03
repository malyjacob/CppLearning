# Virtual Table

这一部分将谈谈虚函数实现的大致原理.

## 早期联编和晚期联编 (early binding and late binding)
当Cpp程序执行遇到函数调用时, 程序会跳转到这个函数的体内执行, 执行完毕后再跳转回来.

这是怎么实现的呢?

这其实是编译器的功劳. 编译器把一条条语句转化为机器码, 而当遇到某些变量或者函数调用时, 编译器并不是将其转化为执行的机器码, 而是将其转化为一个地址或者是相对地址, 供之后运行程序时跳转用的.

而联编就是指把标识符转化为地址的过程.

而这种转换分为两种, 一种是静态联编或者说早期联编, 另一种是动态联编或者说晚期联编.

### 早期联编
大多是的函数调用都是用静态联编的, 就是说,这个地址早在编译的时候就确定好了, 按照正常的顺序执行程序就好了.

典型的例子如下:
```cpp
#include <iostream>

int add(int x, int y)
{
    return x + y;
}

int subtract(int x, int y)
{
    return x - y;
}

int multiply(int x, int y)
{
    return x * y;
}

int main()
{
    int x{};
    std::cout << "Enter a number: ";
    std::cin >> x;

    int y{};
    std::cout << "Enter another number: ";
    std::cin >> y;

    int op{};
    do
    {
        std::cout << "Enter an operation (0=add, 1=subtract, 2=multiply): ";
        std::cin >> op;
    } while (op < 0 || op > 2);

    int result {};
    switch (op)
    {
        // call the target function directly using early binding
        case 0: result = add(x, y); break;
        case 1: result = subtract(x, y); break;
        case 2: result = multiply(x, y); break;
    }

    std::cout << "The answer is: " << result << '\n';

    return 0;
}
```
因为`add()`, `subtract()`, `multiply()`都是直接的函数调用, 没什么湾湾角角, 所以使用静态联编.

### 晚期联编
在某一些程序中, 我们很可能只有等到运行时才能明确函数调用的真实地址, 而这就叫做动态联编.

动态联编的例子如下:
```cpp
#include <iostream>

int add(int x, int y)
{
    return x + y;
}

int main()
{
    // Create a function pointer and make it point to the add function
    int (*pFcn)(int, int) { add };
    std::cout << pFcn(5, 3) << '\n'; // add 5 + 3

    return 0;
}
```
这个`pFcn`函数指针具体是什么值, 只有运行的时候才能知道, 因此通过函数指针的调用一般都属于动态联编.

再来个复杂的例子:
```cpp
#include <iostream>

int add(int x, int y)
{
    return x + y;
}

int subtract(int x, int y)
{
    return x - y;
}

int multiply(int x, int y)
{
    return x * y;
}

int main()
{
    int x{};
    std::cout << "Enter a number: ";
    std::cin >> x;

    int y{};
    std::cout << "Enter another number: ";
    std::cin >> y;

    int op{};
    do
    {
        std::cout << "Enter an operation (0=add, 1=subtract, 2=multiply): ";
        std::cin >> op;
    } while (op < 0 || op > 2);

    // Create a function pointer named pFcn (yes, the syntax is ugly)
    int (*pFcn)(int, int) { nullptr };

    // Set pFcn to point to the function the user chose
    switch (op)
    {
        case 0: pFcn = add; break;
        case 1: pFcn = subtract; break;
        case 2: pFcn = multiply; break;
    }

    // Call the function that pFcn is pointing to with x and y as parameters
    // This uses late binding
    std::cout << "The answer is: " << pFcn(x, y) << '\n';

    return 0;
}
```
这个的不确定性最明显. 只有通过用户的输入才能真正得知这个函数指针到底是什么, 而这肯定必须在运行时才能决定.

而我们的虚函数的实现就是依靠动态联编!

## 虚函数表

虚函数的实现是靠动态联编实现的. 而其中虚函数表扮演者重要角色.

泛泛的来说, 如果一个基类有虚函数时, 那么当这个基类被初始化为对象时, 会同时带有一个隐藏起来的指针, 而这个隐藏起来的指针则指向一个叫做虚函数表的东西.

这个虚函数表类同于一个数组, 里面储存着基类的虚函数的地址.

而当基类被派生类继承时, 这个隐藏的指针作为基类的一个成员也同样被继承过来同时指向自己开辟出来的一段内存, 这个内存作为自己的虚函数表.

如果派生类没有尝试去重写基类的虚函数时, 这个虚函数表就还是跟普通基类对象的虚函数表内容一样.

而当派生类尝试去重写基类虚函数, 并且格式正确时, 这个虚函数表的内容就会被改写. 也就是说, 原来储存的基类的这个要被改写的虚函数的地址被改写为派生类重写的版本的地址.

当然, 这个派生类自己也可以定义自己的虚函数, 那么虚函数表的条目就加上一条自己的虚函数的地址.

好的, 说完了虚函数表如何创建的过程.我们就进一步说说, 到底如何通过虚函数表实现多态的.

当我们的基类指针或者引用指向派生类时, 如果我们通过这个指针或者引用调用了虚函数, 那么这个指针或者说引用在本质上其实确实是只指向派生类的基类部分.

那么怎么就能看到派生类的重写版本呢?

那就是这个事实: 这个指向虚函数表的指针自己就属于基类! 

因此我们虽然不可以直接访问派生类部分, 但是我们可以通过这个隐藏指针来访问查询虚函数表!

程序会先查看这个调用的函数到底是不是虚的, 如果是虚的那么就走虚函数路线, 在虚函数表里面查询有无符合的函数, 如果有符合的就通过这个函数地址来调用这个符合的函数.

而我们知道, 假如派生类真的重写了这个被调用的虚函数, 那么这个虚函数表里的原来储存基类版本的函数地址,早就被改写为派生类版本的重写了, 那么这个函数调用就调用派生类版本的虚函数,而非基类版本的.

这样就实现了多态!

这段代码:
```cpp
class Base
{
public:
    virtual void function1() {};
    virtual void function2() {};
};

class D1: public Base
{
public:
    void function1() override {};
};

class D2: public Base
{
public:
    void function2() override {};
};
```
真实的面貌可能是这样子的:
```cpp
class Base
{
public:
    VirtualTable* __vptr;
    virtual void function1() {};
    virtual void function2() {};
};

class D1: public Base
{
public:
    void function1() override {};
};

class D2: public Base
{
public:
    void function2() override {};
};
```