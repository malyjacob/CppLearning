# unique_ptr

**前情:**
> [Smart-pointer](./Smart-Pointer.md)
<br/>

`std::unique_ptr`是`std::auto_ptr`的替代品. 它比后者具有更高的安全性.

## unique_ptr
`std::unique_ptr`是智能指针, 而它所管理的内部指针是不能被都个同样的智能指针分享的.

也就是说, `unique_ptr`指针独占它所管理的资源, 它拥有对这资源的绝对的所有权.

上个使用的例子:
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
这两个函数体内的智能指针能保证当走出函数作用域的时候, 能释放所管理的资源.

> ***注意:*** `unique_ptr`不接受左值赋值运算符重载和左值复制构造函数. 但它能接受右值复制构造函数和右值赋值运算符重载.

`unique_ptr`的表现除了不用手动`delete`外, 与常规指针的表现几乎完全一样.

它重载了`*`和`->`运算符, 因此完全可以把它当作正常指针使用.

`unique_ptr`的模板原型如下:
```javascript
template <class T, class Deleter = std::default_delete<T>>
class unique_ptr;
```
这个后面的类型是用作释放所管理的资源的释放器, 是一个`函数对象`, 它其实已经给出了默认的释放器了, <br/>
因此我们无需特殊指定释放器.
<br/>

## `unique_ptr` 用于数组
`unique_ptr`还有一个偏特化版本:
```javascript
template <class T, class Deleter = std::default_delete<T>>
class unique_ptr<T[], Deleter>;
```
从中,我们可以看到, `unique_ptr`其实可以用于数组身上, 管理一个数组资源.

当然, 对于Cpp内置的诸如`vector` `array`类型, 就没有必要用`unique_ptr`来管理了.

```javascript
#include <iostream>
#include <memory>

using namespace std;

class Fraction
{
    int m_numberator = 0;
    int m_denominator = 1;
public:
    Fraction(int num = 0, int deno = 1)
        : m_numberator(num), m_denominator(deno) {}
    friend ostream& operator<<(ostream& os, const Fraction& f)
    {
        os << f.m_denominator << '/' << f.m_denominator;
        return os;
    }
};

int main()
{
    auto f1 = make_unique<Fraction>(3, 5);
    cout << *f1 << endl;

    auto f2 = make_unique<Fraction[]>(4);
    cout << f2[2] << endl;

    return 0;
}
```

output:
```javascript
5/5
1/1

```
在这个例子中,我没有直接用到`unique_ptr`构造函数, 而是用一个独立的函数`make_unique`去生成.

`make_unique`是一个模板函数, 最主要有两种:
```javascript
template<class T, class... Args>
unique_ptr<T> make_unique(Args&&... args);

template<class T>
unique_ptr<T> make_unique(std::size_t size);
```
第一种用在非数组的`unique_ptr`, 接受一串值, 这一串值其实就是这个`T`的构造函数所接受的值;

第二种用于数组形式的`make_unique`, 接受一个值,来指定数组大小.

在这里的建议是, 尽量使用这个生成函数.

因为用这个生成函数生成的智能指针是不会为空的, 并且不像原生构造函数接受指针, 这样更具有可读性.

另外, 这也防止了出现诸如下面的错误:
```javascript
int main()
{
    char *str = new char[12];
    unique_ptr<char[]> u_str(str);
    delete[] str; // !!!!!
}
```
这段函数是会报错的.

为何? 

因为我已经释放了`str`了, 但为我的`unique_ptr`还是指向着`str`. <br/>
等函数体结束时, `unique_ptr`尝试自动释放`str`, 但`str`已经被释放了....
<br/>

## 作为函数的返回值或者参数
### 作为返回值
`unique_ptr`可以安全的被函数返回:
```javascript
#include <iostream>
#include <memory>

using namespace std;

class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
};

unique_ptr<Resource> createResource()
{
    return make_unique<Resource>();
}

int main()
{
    auto ptr = createResource(); // it calls rvalue constructor not lvalue one;
    // do whatever
    if(ptr)
        cout << "no null" << endl;

    return 0;
}
```
output:
```javascript
Resource acquired
no null
Resource destoryed

```
上面的代码,如果`createResource`返回的`unique_ptr`没有被赋值, 那么,这个`unique_ptr`就会自动释放掉.

而如果赋值了或者作为初始化的右值, 正如这段代码所示, 就会调用右值的赋值或者复制构造函数, 因为`createResource`返回的是右值, 并且`unique_ptr`是不接受左值的赋值或者复制构造函数的.

也就是说, 在`auto ptr = createResource();`中,`createResource`返回的`unique_ptr`对其所管理的资源的所有权被转移到了`ptr`这个`unique_ptr`上了. <br/>
保证`unique_ptr`的独占属性.

### 作为函数参数
我们可以将`unique_ptr`传入函数中, 从而让函数获得`unique_ptr`所管理的所有权.

但要记住的是`unique_ptr`是拒绝左值的复制或赋值的, 因此, 有些时候就需要使用`std::move`.

```javascript
#include <iostream>
#include <memory>
#include <utility> // for std::move
using namespace std;

class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
    friend ostream& operator<<(ostream& os, const Resource& res)
    {
        os << "I am Resource";
        return os;
    }
};

void takeOwnership(unique_ptr<Resource> res)
{
    if(res)
        cout << *res << endl;
}

int main()
{
    auto ptr = make_unique<Resource>();

    // takeOwnership(ptr); no ok!
    takeOwnership(move(ptr));

    takeOwnership(make_unique<Resource>()); // ok. use the rvalue operator= of unique_ptr;

    cout << "End" << endl;

    return 0;
}
```
output:
```javascript
Resource acquired
I am Resource
Resource destoryed
Resource acquired
I am Resource
Resource destoryed
End

```
值得注意的是哪个资源的所有权被转移到了`takeOwnership`这里去了, 因此, 当走出了`takeOwnership`的函数作用域后, 这些资源就会被销毁, 而不是等到`mian`结束时才销毁.

这段代码仅仅是实验性质的, 因为根本没有必要把整个智能指针传入进去, 即使我们仅仅传入智能指针的引用也是如此, 因为这会增加代码的复杂度. <br/>
更何况,这里我们仅仅需要的是获得这个资源的使用而不是它的所有权 <br/>
并且我们其实也不打算在走出`takeOwnership`作用域时就销毁资源. <br/>
因此, 这个`takeOwnership`接受个裸指针就行了, 这样就不会立马就销毁资源.
可以改成这样:
```javascript
#include <iostream>
#include <memory>
#include <utility> // for std::move
using namespace std;

class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
    friend ostream& operator<<(ostream& os, const Resource& res)
    {
        os << "I am Resource";
        return os;
    }
};

void takeOwnership(Resource *res)  // only accept raw pointer
{
    if(res)
        cout << *res << endl;
}

int main()
{
    auto ptr = make_unique<Resource>();

    takeOwnership(ptr.get()); // get() to get a pointer to the Resource;

    cout << "End" << endl;

    return 0;
}
```
这个`get`方法返回`unique_ptr`的资源指针.

output:
```javascript
Resource acquired
I am Resource
End
Resource destoryed

```
<br/>

## 用作类成员
我们可以把`unique_ptr`当作一个类的成员. 这样我们就不必担心放进类里的资源不会被释放了. 

当对象过期或者被`delete`时, 就会调用析构函数删除成员, <br/>
而在`unique_ptr`被删除时, `unique_ptr`作为类也会自动调用自己的析构函数把正在管理的资源释放掉.

但如果这个对象没有被正确完整的销毁时, 很可能里面的`unique_ptr`成员也没有被销毁, 那么`unique_ptr`所管理的资源也不会被释放.
这样还是会造成内存泄漏.
<br/>

## 总结与`unique_ptr`使用规范建议

### - **尽量不用已经存在过的指针来生成`unique_ptr`**

看看这个例子:
```javascript
Resource *res = new Resource();
unique_ptr<Resource> res1(res);

delete res; // !!warning
```
这个例子很危险! `res` 已经被释放了! 而`res1`还在管理着, 可以预见, 在`res1`走出作用域,调用自身的析构函数,尝试释放已经释放过的资源就会立马报错!

因为 原指针与`unique_ptr`指针是分离的, 元指针可以越过`unique_ptr`直接操纵资源, 而`unique_ptr`却不知道! 保不准元指针释放掉了资源, `unique_ptr`还在傻傻的守着! 后来恍然大悟,一怒之下给你报个错, 就有你好受的了.

从`unique_ptr`甚至后面的智能指针的设计理念看, 将已经存在过的指针来生成智能指针, 就背离了原来的让智能指针管理资源的理念.

资源要不就从一开始就完全用裸指针, 自己对自己负责.

要不就信任智能指针,从分配资源一开始就给智能指针管, 如果真的要裸指针,也要从智能指针那里谨慎郑重的获得裸指针, 而从智能指针得来的裸指针最好就仅仅用于读取资源,而不改动资源.要不就很可能又会重蹈覆辙!

俗话讲, 将在外,军令有所不受就是这种意思, 千万别政出多门!

### - **尽量使用生成函数`make_unique`**

为了不小心就政出多门, 那么就是在初始化智能指针时,就不给裸指针出场的机会.

而`make_unique`就很好的做到了, 直接调用资源构造函数,然后立马把资源包裹进`unique_ptr`里面.这里根本没有露出原始指针, 也就不会担心不小心通过原始指针操纵资源了.

而裸指针此时也智能通过`get()`方法得到, 安全性得到了很大的保证.

### - **坚持一个资源一个`unique_ptr`原则**
其实如果做到以上两点就可以自动符合这个原则了.

不过还是要强调一番.

看下面这个例子:
```javascript
Resource *res = new Resource();

unique_ptr<Resource> res1(res);
unique_ptr<Resource> res2(res);
```
这个例子完美的违反了上面两条建议.

一个管家就够了, 再多个管家, 这个家又会乱了套!

两个原本遵循着独占性的管家, 竟然能共同操作同一个资源, 这就离谱! 更何况后面的资源二次释放就够提一壶了. 并且还有可能有`res`背刺, 偷偷把资源释放掉, 那么这两个管家就纯纯是个小丑🤡.

### ***综上, 只要遵循我这三个原则, 保准不会在用`unque_ptr`上出错!***
