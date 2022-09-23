# Smart Pointer

# 引言
Cpp 最令人头疼的其中一个问题就是内存泄漏, 让人防不胜防, 即使是Cpp高手也不免会在这里踩坑. <br/>

而内存泄漏的主要元凶就是指针的误用, 这些误用总结如下: <br/>
- 指针没有初始化分配内存就给他赋值
- 指针忘了释放内存
- 没有检测指针是否为空, 就用上了
- 指针所指内存释放后没有把指针设为`nullptr`, 然后又再去用这个指针
- 指针所指的内存二次释放

虽然Cpp有RAII原则, 但我们有时候不免要操作裸指针, 这些问题总是要面对.

幸运的, Cpp 贴心的给了我们智能指针这个神器, 让我们能最大限度的去解决以上几个难缠的问题.

在介绍智能指针前, 我们先看看以下两段代码:
```javascript
// 代码一 
void fn1(std::string & str)
{
    std::string *ps = new std::string(str);
    std::cout << *ps << std::endl;
}
```

```javascript
// 代码二
void fn2(std::string & str)
{
    std::string *ps = new std::string(str);

    if(ps->length() > 16)
        return;
    else
    std::cout << *ps << std::endl;

    delete ps;
}
```
这两段代码都是有问题的.

第一段, 在函数体内动态分配了内存, 但在走出函数作用域时, 我们却没有把这个指针释放掉,这就造成了内存泄漏.

第二段, 更加隐蔽, 也是动态分配内存, 他也想到了要在走出函数体时释放内存, 但注意: 当那个输入字符串的长度大于16时,
函数就直接退出作用域, 而后面的释放语句就被跳过了! 最终也会导致内存泄漏.

这时候我们就要放出智能指针这个大杀器了, `auto_ptr`, `unique_ptr`, `shared_ptr`. 这个`auto_ptr`存在许多问题, 因此已经被弃用了. 因此, 下面我们将只介绍另外这两个智能指针和辅助智能指针`weak_ptr`.

这两段用智能指针是这样的:
```javascript
void smart_fn1(std::string & str)
{
    std::unique_ptr<std::string> ps(new std::string(str)); // !!!!!!
    std::cout << *ps << std::endl;
}

void smart_fn2(std::string & str)
{
    std::unique_ptr<std::string> ps(new std::string(str)); // !!!!!!!!!
    if(ps->length() > 16)
        return;
    else
    std::cout << *ps << std::endl;
}
```
这里面的`ps`就是智能指针, 我们可以看到, 这个函数体内却没有释放语句.

因为, 这个智能指针会在走出作用域后自动释放所分配的内存 , 无需我们手动释放!
<br/>

## 智能指针的大致原理
智能指针其实也是应用了RAII原则, 智能指针其实就是包裹着指针的一个对象, 由类初始化.

类自带析构函数,会在走出函数域或者被`delete`的时候自动调用析构函数.

因此,当我们用构造函数生成一个对象时, 这个对象就会有自己的生命周期.

而我们知道, 有运算符重载这个功能, 于是我们就可以重载这个包装了指针的类的`*`和`->`运算符. <br/>

这就是智能指针的大致原理, 我们可以用类来帮我们管理内存!

现在我们就走个简陋的智能指针来!

```javascript
template<class T>
class Smart_ptr
{
    T *m_ptr;
public:
    Smart_ptr(T *ptr = nullptr) : m_ptr(ptr) {}
    ~Smart_ptr() { delete m_ptr; }

    // overload the operators
    T& operator*() const { return *m_ptr; }
    T *operator->() const { return m_ptr; }
};

class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
};

int main()
{
    Smart_ptr<Resource> res(new Resource());
    return 0;
} // res goes out of scope here, and destorys the allocated Resource for us.
```
输出结果是:
```javascript
Resource acquired
Resource destoryed

```
<br/>

## 隐藏的问题
当然上面这个智能指针并不是没有问题的, 看这个情况:
```javascript
int main()
{
    Smart_ptr<Resource> res(new Resource());
    Smart_ptr<Resource> res2 = res // Error!
    return 0;
}
```

输出结果:
```javascript
Resource acquired
Resource destoryed
Resource destoryed
free(): double free detected in tcache 2
已放弃 (核心已转储)

```
这个分配的内存释放了两次! 这个问题其实就是被弃用的`auto_ptr`的原因, 取而代之的`unique_ptr`解决了这个问题.

还是回来分析一下到底为何会这样.

因为我们没有特别定制复制构造函数和重载赋值运算符, 因此编译器为我们提供了默认的复制构造函数和重载赋值运算符, <br/>
而,这个默认的赋值运算符,只是把我们`res`内部的指针值直接复制到了`res2`里面去. <br/>
也就是说, 这两个智能指针所指向的是同一个`Resource`. 而`res2`先出栈, 然后调用析构函数释放掉`Resource`, 然后`res`再出栈,然后也调用析构函数去释放里面的指针. <br/>
但这个指针所指向的`Resource`早就被释放了, 再尝试去释放就会造成二次释放的错误! <br/>
然后,Crash!!!!!
<br/>

## 移动语义, 转移所有权
其实, 给出对应的赋值运算符重载, 另外分配一个新内存, 然后把目标智能指针里面的指针所指向的内存赋值给这个新内存就可以了.

不过,多了复制内存这个步奏其实是要耗费性能的, 为何我们不直接把目标智能指针的指针所指向的内存直接拿过来, 然后把这个指针贴个`nullptr`呢? <br/>
这样就省下了复制这个费性能的步奏, 并且因为原来的智能指针里面的指针已经是`nullptr`了, 而释放空指针是完全没有问题的.

而以上的这个想法就是 ***移动语义***!

***移动语义***就是转移所有权而不是另外开辟一个内存再去复制.

好! 现在就来改进我们的`Smart_ptr`吧!
```javascript
#include <iostream>
#include <memory>

class Resource
{
public:
    Resource() { std::cout << "Resource aquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
};

template<class T>
class Smart_ptr
{
    T *m_ptr;
public:
    Smart_ptr(T *ptr = nullptr) : m_ptr(ptr) {}
    ~Smart_ptr() { delete m_ptr; }

    // overload the operators
    T& operator*() const { return *m_ptr; }
    T *operator->() const { return m_ptr; }
    operator bool() const { return m_ptr == nullptr ? false : true; } // test if the smart pointer is null

    // a copy constructor that implements move semantics
    Smart_ptr(Smart_ptr& a) // note: no const
    {
        m_ptr = a.m_ptr;
        a.m_ptr = nullptr;
    }

    Smart_ptr& operator=(Smart_ptr& a)
    {
        if(&a == this)
            return *this;

        delete m_ptr;  // don't forget!
        m_ptr = a.m_ptr;
        a.m_ptr = nullptr;
        return *this;
    }
};


int main()
{
    Smart_ptr<Resource> res1(new Resource());
    Smart_ptr<Resource> res2; // start with null

    std::cout << "res1 is " << (res1 ? "not null\n" : "null\n");
    std::cout << "res2 is " << (res2 ? "not null\n" : "null\n");

    res2 = res1;

    std::cout << "ownership has been transferred\n";

    std::cout << "res1 is " << (res1 ? "not null\n" : "null\n");
    std::cout << "res2 is " << (res2 ? "not null\n" : "null\n");

    return 0;
}

```

output:
```javascript
Resource aquired
res1 is not null
res2 is null
ownership has been transferred
res1 is null
res2 is not null
Resource destoryed

```

以上这个解决方案其实为`unique_ptr`所用.