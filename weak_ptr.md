# weak_ptr

**前情:**
> [shared_ptr](./shared_ptr.md)
<br/>

再前面的那一节, 我们看到`shared_ptr`能够让我们多个智能指针共享一个资源.

但这个可能会遇到一些问题, 单纯用`shared_ptr`是无法解决的.

让我们看看下面这个例子:
```javascript
#include <iostream>
#include <memory>
#include <cstring>

using namespace std;

class Father;
class Son;

class Father
{
public:
    char name[16] { 0 };
    shared_ptr<Son> son {};
    Father(const char *nm)
    {
        strncpy(name, nm, 16);
        cout << "created Father" << endl;
    }
    ~Father() { cout << "destoryed Father" << endl; }
};

class Son
{
public:
    char name[16] { 0 };
    shared_ptr<Father> father {};
    Son(const char *nm)
    {
        strncpy(name, nm, 16);
        cout << "created Son" << endl;
    }
    ~Son() { cout << "destoryed Son" << endl; }
};

int main()
{
    auto f = make_shared<Father>("father");
    auto s = make_shared<Son>("son");

    cout << "f's ref count: " << f.use_count() << endl;
    cout << "s's ref count: " << s.use_count() << endl;

    cout << "linked..." << endl;
    f->son = s;
    s->father = f;

    cout << "f's ref count: " << f.use_count() << endl;
    cout << "s's ref count: " << s.use_count() << endl;
    
    return 0;
}  
```
output:
```javascript
created Father
created Son
f's ref count: 1
s's ref count: 1
linked...
f's ref count: 2
s's ref count: 2

```
单单看代码, 好像没什么错误, 并且运行时也没有报错.

但这个输出结果却表明, 这两个资源对象都没有释放!

这就是内存泄漏! 有很大的风险.

那么为什么会这样呢?

让我们先分析以下:

首先创建两个对象, 此时两个对象的引用值都是1, 这部分正常.

然后我们再把这两个对象联系起来, 也就是各自的一个`shared_ptr`指向对方.

然后, 此时各自的引用量是2.

然后退出函数体时, `s`先出栈, 销毁一个`shared_ptr`, 此时的`s`所指向的资源引用量减1, 也就是1, 那么这个资源还没有被释放.

随后, `f`出栈, 销毁一个`shared_ptr`, 此时的`f`所指向的资源引用量减1, 也就是1, 那么这个资源也没有被释放.

这就麻烦了! 走出函数体后, 两个资源依旧没有被释放.

这种情况叫做引用循环, 十分的棘手! 单单用`shared_ptr`是无法解决这个问题的.

于是为了解决这个循环引用的问题, 就引入了`weak_ptr`来辅助`shared_ptr`走出循环引用困境!

## `weak_ptr`
`weak_ptr`也是个智能指针, 但是它没有真正拥有所指向的资源的所有权.

并且,`weak_ptr`只能由`shared_ptr`或者`weak_ptr`产生, 就是把`shared_ptr`或`weak_ptr`值赋值给`weak_ptr`.

`weak_ptr`就是个观察者, 它的产生或这销毁不会让`shared_ptr`的引用量增加或减少.

由于`weak_ptr`只是个观察者, 所以不能通过它直接访问资源, 而是要将它转换为`shared_ptr`才能访问.

而打破循环引用的关键就在于`weak_ptr`不会增加或减少`shared_ptr`的引用量.

来看看改进版:
```javascript
#include <iostream>
#include <memory>
#include <cstring>

using namespace std;

class Father;
class Son;

class Father
{
public:
    char name[16] { 0 };
    weak_ptr<Son> son {}; // replace the shared one with weak one
    Father(const char *nm)
    {
        strncpy(name, nm, 16);
        cout << "created Father" << endl;
    }
    ~Father() { cout << "destoryed Father" << endl; }
};

class Son
{
public:
    char name[16] { 0 };
    weak_ptr<Father> father {}; // replace the shared one with weak one
    Son(const char *nm)
    {
        strncpy(name, nm, 16);
        cout << "created Son" << endl;
    }
    ~Son() { cout << "destoryed Son" << endl; }
};

int main()
{
    auto f = make_shared<Father>("father");
    auto s = make_shared<Son>("son");

    cout << "f's ref count: " << f.use_count() << endl;
    cout << "s's ref count: " << s.use_count() << endl;

    cout << "linked..." << endl;

    // assign shared one to weak one
    f->son = s;
    s->father = f;

    cout << "f's ref count: " << f.use_count() << endl;
    cout << "s's ref count: " << s.use_count() << endl;
    
    return 0;
}  
```
output:
```javascript
created Father
created Son
f's ref count: 1
s's ref count: 1
linked...
f's ref count: 1
s's ref count: 1
destoryed Son
destoryed Father

```
这下, 我们看到, 这`f` 跟 `s`所指向的资源都得以释放了.

## `weak_ptr` 细节

`weak_ptr`跟`shared_ptr`一样也有`use_count`方法, 就是计算一共有多少`shared_ptr`指向这同一个资源.

那么如果`weak_ptr`所指的资源被`shared_ptr`释放了, `weak_ptr`指向什么?

要回答这个问题, 我们要知道`weak_ptr`还有个独有的方法`expired`, 这个方法是用来检测`weak_ptr`所指向的资源还到底在不在.

因此,我们根本不用明确知道过后`weak_ptr`指向什么, 只需要使用`expired()`检测就行, 如果返回`true`, 就不要继续用它.

而这个`expired`跟`weak_ptr`转`shared_ptr`有很大的联系.

上面已经说了, 不能直接用`weak_ptr`访问资源, 一定要将`weak_ptr`转化为`shared_ptr`才能访问.

而这个转换成员函数就是`lock()`.确切的说这个函数是这样的` expired() ? shared_ptr<R>() : shared_ptr(*this)`.

也就是说, 如果这个`weak_ptr`所指向的资源不在, 那么要转`shared_ptr`的话, 就转成一个空的`shared_ptr`. <br/>
如果在, 则正式转正,成为指向资源的`shared_ptr`.

走个例子就明白了:
```javascript
#include <memory>
#include <iostream>

using namespace std;

void observe(weak_ptr<int> weak)
{
    if(auto obs = weak.lock())
        cout << "observe() able to lock weak_ptr, value = " << *obs << endl;
    else
        cout << "observe() unable to lock weak_ptr" << endl;
}

int main()
{
    weak_ptr<int> weak;
    cout << "weak_ptr not yet init" << endl;
    observe(weak);

    {
        auto shared = make_shared<int>(12);
        weak = shared;
        cout << "weak_ptr inited with shared_ptr" << endl;
        observe(weak);
    }

    cout << "shared_ptr has been destructed due to out of scope" << endl;
    observe(weak);
}
```
output:
```javascript
weak_ptr not yet init
observe() unable to lock weak_ptr
weak_ptr inited with shared_ptr
observe() able to lock weak_ptr, value = 12
shared_ptr has been destructed due to out of scope
observe() unable to lock weak_ptr

```

## weak_ptr规范建议
- 非必要不用`weak_ptr`, 最好只有当出现循环引用才用它.

- 非必要不尝试通过`weak_ptr`直接或间接访问资源.

- 当真的必须要通过`weak_ptr`间接访问资源时, 必须要检测转换后的`shared_ptr`是否为空.