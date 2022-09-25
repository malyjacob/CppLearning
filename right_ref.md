# Right Reference

现在来介绍右值引用.不过在介绍它之前, 先来看看常规的引用的一些用法, 并分析一下具体如何?

来看看如下代码:

```javascript
#include <iostream>
#include <cstring>

using std::cout;
using std::endl;

// 交换值
template <class T>
void swap(T &t1, T &t2)
{
    auto temp = t1;
    t1 = t2;
    t2 = temp;
}

class Resource
{
public:
    char *value;
    Resource(const char *v)
    {
        int len = strlen(v);
        cout << "create a new empty value...." << endl;
        value = new char[len + 1];
        strcpy(value, v);
        cout << "Resource created" << endl;
    }
    Resource(Resource &res)
    {
        cout << "using Resource's copy construcotr...." << endl;
        int len = strlen(res.value);
        cout << "create a new empty value...." << endl;
        value = new char[len + 1];
        strcpy(value, res.value);
        cout << "Resource created" << endl;
    }
    ~Resource()
    {
        cout << "Resource destoryed" << endl;
        if(value)
        {
            cout << "delete the value...." << endl;
            delete[] value;
        }
    }
    Resource &operator=(const Resource &res)
    {
        cout << "using Resource's operator= overload...." << endl;
        int len = strlen(res.value);
        if(value)
        {
            cout << "delete the origin value...." << endl;
            delete[] value;
        }
        cout << "create a new empty value...." << endl;
        value = new char[len + 1];
        strcpy(value, res.value);
        return *this;
    }
    operator bool() { return value ? true : false; }
};

int main()
{
    Resource res1("res1");
    Resource res2("res2");
    cout << "res1 value = " << res1.value << endl;
    cout << "res2 value = " << res2.value << endl;
    cout << endl;

    cout << "using swapping...." << endl;
    swap<Resource>(res1, res2);
    cout << endl;
    cout << "res1 value = " << res1.value << endl;
    cout << "res2 value = " << res2.value << endl;
    cout << endl;
}
```

output:

```javascript
create a new empty value....
Resource created
create a new empty value....
Resource created
res1 value = res1
res2 value = res2

using swapping....
using Resource's copy construcotr....
create a new empty value....
Resource created
using Resource's operator= overload....
delete the origin value....
create a new empty value....
using Resource's operator= overload....
delete the origin value....
create a new empty value....
Resource destoryed
delete the value....

res1 value = res2
res2 value = res1

Resource destoryed
delete the value....
Resource destoryed
delete the value....
```

我们可以看到, 在交换时, 有三次堆内存的创建和销毁.

- 第一次是, 创建临时变量`temp`
- 第二次是使用复制构造函数将`t2`的值赋值给`t1`.
- 第三次是使用复制构造函数将`temp`的值赋值给`t2`.

但如果仔细想想, 这里其实可以不用那么频繁的销毁和创建, 为什么不可以把这两个对象的各自的资源指针相互交换一下呢? 这样的话, 就根本没有资源的创建和销毁, 提升性能.

好的, 那么现在就去实现一个针对`Resource`的特化版`swap`.

```javascript
template <>
void swap<Resource>(Resource &r1, Resource &r2)
{
    char *temp_value = r1.value; // temporary value to be passed
    r1.value = r2.value;
    r2.value = temp_value;
}
```

output:

```javascript
create a new empty value....
Resource created
create a new empty value....
Resource created
res1 value = res1
res2 value = res2

using swapping....

res1 value = res2
res2 value = res1

Resource destoryed
delete the value....
Resource destoryed
delete the value....
```

看看这个, 压根就没有创建和销毁堆内存就实现了这两个对象的交换.不过这个方法在没有`右值引用`时,其实并不能应用于所有的对象.

给个另一种对象, 就又要重新写一套对应的特化版本, 而不能像那个非特化的`swap`版本, 只要这个对象实现了复制构造函数和赋值运算符重载, 就可以使用这个模板函数.

继续, 假如我想要搞个函数`show`来展示`Resource`的内容. <br/>
先写一个比较差的常规版本:

```javascript
void show(Resource & res);
{
  (cout << res.value) << endl;
}
```

这个`show`只能接受可修改引用和可修改左值对象作为实参, 如果输入的是右值, 比如这样用`show(Resource("res"))`, 那就会报错.

要解决这个问题很简单, 就是把形参改为常量引用:

```javascript
void show(const Resource &res)
{
    cout << res.value << endl;
}
```

这样这个函数就可以接受右值参数了, 当然也可以接受可修改引用和可修改左值对象作为实参.

不过, 虽然这个新的`show`可以接受右值了, 但这样就会有一个代价, 那就是这个函数会创建一个临时变量,然后调用复制构造函数, 把这个右值 copy 过来. 这个临时变量的体内会开辟一个新的内存去存取刚刚从实参复制过来的`value`的内容.

并且此时`res`所指向的是那个临时变量, 而不是刚刚传进来的右值, 这个右值, 连同其`value`早在复制构造函数执行完毕时就已经被销毁了(在编译器没有做优化的情况下).

而按照我们之前的那个特化版`swap`的思路, 这个其实有点浪费, 为什么我们不可以在创建临时变量时,直接拿走右值的`value`给这个临时变量呢?

更进一步, 我们为什么要这么早就把这个右值销毁呢? 把它留存下来, 在函数体内直接使用这个右值, 等到走出函数作用域再来销毁不也很好? 这个更进一步的方法比前一个通用性更强, 并且压根就不用另外创建一个临时对象.

这个关键性的把右值`留存下来`, 更确切地来说`延长生命周期`这个思路, 在没有`右值引用`时, 是做不到的.

## 创建右值引用

前面, 我们可以看到, 在没有`右值引用`时后的一些尴尬, 现在终于可以正式来了解`右值引用`了.

先来看看`右值引用`怎么回事.

之前,说过左值就是可以取地址的变量, 那么反过来, 右值就是不能取地址的值, 或者说是没有名字的值. 最常见的右值就是`字面量`, `构造函数返回值`.

```javascript
int a = 5; // a是左值
int *ptr = &a;
int &ref_a = a; // use lvalue ref.

"maly"; // 这个就是右值
int res = Resource("sss"); // 等式右边的表达式是右值
a + 12; // 这个表达式是右值.

int &ref_b = 5; // error!!! lvalue ref cannot be assigned with rvalue
```

之前, 右值往往出现在赋值语句或者初始化语句的等号右边.在语句执行完毕后, 这个右值一般都会被销毁. 假如右值没有被立即使用, 那么就会即刻被销毁.

有了`右值引用`后, 我们就可以变相的延长右值的生命周期:

```javascript
int &&a = 13; // use rvalue ref
Resource &&res = Resource("sss"); // use rvalue ref
int b = a + 12; //=> 25
show(res); // use void show(cosnt Resource &res);
```

`右值引用`使用`&&`来标识.

> 注意! 当使用`右值引用`时, 它并没有另外再创建一个对象再来复制, 而是这个右值引用变量名直接指向右值内容, 如同`左值引用`那样指向被引用变量的内容.

右值引用本质上就是给右值安上一个名字, 然后这个右值暂时没有被立即销毁, 而是留存下来, 通过`右值引用`来被访问.

有了这个`右值引用`, 我们就可以先改进一下我们的`show`了, 我们可以为`show`增加一个重载, 专门用于接受右值参数.

```javascript
void show(const Resource &&s)
{
    cout << s.value << endl;
}
```

同样, 我们的`Resource`也可以专门为右值搞个对应的右值复制构造函数, 和右值复制运算符重载.

```javascript
Resource(Resource && res);
{
  cout << "using Resource's rvalue copy construcotr...." << endl;
  cout << "move the ownership of the value" << endl;
  value = res.value;
  res.value = nullptr;
  cout << "Resource created" << endl;
}

Resource &operator=(Resource &&res)
{
    cout << "using Resource's rvalue operator= overload...." << endl;
    cout << "move the ownership of the value" << endl;
    if (value)
    {
        cout << "delete the origin value...." << endl;
        delete[] value;
    }
    value = res.value;
    res.value = nullptr;
    return *this;
}
```
这函数体内的有些多余的语句是为了更明显的看到效果的.

好的! 让我们来用一下:
```javascript
int main()
{
    Resource res1("res1");
    Resource res2("res2");
    cout << "res1 value = " << res1.value << endl;
    cout << "res2 value = " << res2.value << endl;
    cout << endl;

    Resource res3(Resource(res3));  // use rvalue ref construcotr
    cout << endl;

    res1 = Resource("res4"); // use rvalue ref operator= overload
    cout <<  endl;
}
```
output:
```javascript
create a new empty value....
Resource created
create a new empty value....
Resource created
res1 value = res1
res2 value = res2


create a new empty value....
Resource created
using Resource's rvalue operator= overload....
move the ownership of the value
delete the origin value....
Resource destoryed

Resource destoryed
delete the value....
Resource destoryed
delete the value....
```
需要注意的是, 编译器会自动优化, 如果开了优化, 那么输出结果就看不到`Resource res3(Resource(res3));`与上面输出对应的结果, 建议在编译时, 后面加上`-fno-elide-constructors`指令.

## 强制右值转换
上面, 我们已经写出了右值复制构造函数和右值赋值运算符重载, 并且用右值尝试了一番.

但是, 假如在上面的应用代码中, 我就是看那个`res2`不爽, 想要把它用在这两个函数怎么办?

由于`res2`是左值, 如果真的要把它用在这两个函数上, 就必须把`res2`转换成右值, 再传入这两个函数即可.

Cpp里有个模板函数`std::move`正好符合我们的需求, 它位于头文件`<utility>`, 接受任意对象, 并把这个对象转换为右值并返回.

那么就可以这样用: (PS: 记得头文件加上`<utility>`)
```javascript
int main()
{
    Resource res1("res1");
    Resource res2("res2");
    cout << "res1 value = " << res1.value << endl;
    cout << "res2 value = " << res2.value << endl;
    cout << endl;

    auto res3(std::move(res2));
    if (!res2)
        cout << "res2 is null" << endl;
    else
        cout << "res2 value = " << res2.value << endl;
    cout << "res3 value = " << res3.value << endl;
    cout << endl;

    res1 = std::move(res3);
    if (!res3)
        cout << "res3 is null" << endl;
    else
        cout << "res3 value = " << res3.value << endl;
    cout << "res1 value = " << res1.value << endl;
} 
```
output:
```javascript
create a new empty value....
Resource created
create a new empty value....
Resource created
res1 value = res1
res2 value = res2

using Resource's rvalue copy construcotr....
move the ownership of the value
Resource created
res2 is null
res3 value = res2

using Resource's rvalue operator= overload....
move the ownership of the value
delete the origin value....
res3 is null
res1 value = res2
Resource destoryed
Resource destoryed
Resource destoryed
delete the value....
```
可以看到, 使用了`std::move`后, 都成功的调用了指责两个函数.

并且我们还可以看到, 第一段使用了右值复制构造函数, 然后`res2`的`value`值转移到了`res3`这里;

第二段使用了右值赋值运算符重载, 然后`res3`的`value`值转移到了`res1`这里;

有了这个`std::move`, 我们就可以改进`swap`了, 让这个`swap`模板函数能够在不操作堆内存的情况下,实现高性能的交换值, 并且不再像上面那样, 要专门写特定对象的特化版本.
```javascript
class Resource;

template <class T>
void better_swap(T &t1, T &t2)
{
    auto temp = std::move(t1);
    t1 = std::move(t2);
    t2 = std::move(temp);
}
```
闲话少说, 上代码溜溜:
```javascript
int main()
{
    Resource res1("res1");
    Resource res2("res2");
    cout << "res1 value = " << res1.value << endl;
    cout << "res2 value = " << res2.value << endl;
    cout << endl;

    cout << "using better swapping...." << endl;
    better_swap<Resource>(res1, res2);
    cout << endl;
    cout << "res1 value = " << res1.value << endl;
    cout << "res2 value = " << res2.value << endl;
    cout << endl;
}
```
output:
```javascript
create a new empty value....
Resource created
create a new empty value....
Resource created
res1 value = res1
res2 value = res2

using better swapping....
using Resource's rvalue copy construcotr....
move the ownership of the value
Resource created
using Resource's rvalue operator= overload....
move the ownership of the value
using Resource's rvalue operator= overload....
move the ownership of the value
Resource destoryed

res1 value = res2
res2 value = res1

Resource destoryed
delete the value....
Resource destoryed
delete the value....
```
可以看出, 这里完全没有操作堆内存, 就是交换了`value`的所有权!

并且只要所接受的参数类型实现了右值复制构造函数和右值赋值运算符重载, 就都可以用这个函数模板!

## 右值引用非右值
需要特别提醒的是, 不要想当然认为既然都`右值引用`了, 那么`右值引用`就是右值.

是否是右值, 在引入`右值引用`后变得有些许微妙.

判定的依据是如下: <br/>

***右值引用类型既可以被当作左值也可以被当作右值，判断的标准是，如果它有名字，那就是左值，否则就是右值。***

假如有这样一个代码:
```javascript
void foo(X&& x)
{
    X anotherX = x;
    /....
}
```
在这个例子中，因为右值引用x是有名字的，所以x被当作左值来处理.

再来看看下面的:
```javascript
X&& go(); // function prototype
X x = go();
```
这个时候, `go`的返回值就被当作右值处理.

## 相关建议
......

## 附加相关代码
```javascript
#include <iostream>
#include <string>
#include <utility>
using namespace std;

template <class T>
void swap_E(T &e1, T &e2)
{
    T temp(move(e1));
    e1 = move(e2);
    e2 = move(temp);
}


class Element
{
    string name;
    int id;

public:
    Element(const char *nm, int i = 0) : name(nm), id(i)
    {
        cout << "Element created...." << endl;
    }
    Element(Element &&ele)
    {
        id = ele.id;
        name = move(ele.name);
        cout << "Element using rvalue constructor..." << endl;
    }
    Element &operator=(Element &&ele)
    {
        id = ele.id;
        name = move(ele.name);
        cout << "Element using rvalue operator= overload...." << endl;
        return *this;
    }
    ~Element() { cout << "Element destoryed..." << endl; }
    const string &getName() { return name; }
};

class Container
{
    Element element;
    int id;

public:
    Container(const char *nm, int i = 0) : element(nm, i)
    {
        id = i;
        cout << "Container created...." << endl;
    }
    Container(Container &&ctr) : element(move(ctr.element))
    {
        id = ctr.id;
        cout << "Container using rvalue constructor..." << endl;
    }
    Container &operator=(Container &&ctr)
    {
        id = ctr.id;
        element = move(ctr.element);
        cout << "Container using rvalue operator= overload..." << endl;
        return *this;
    }
    const string &getName() { return element.getName(); }
    ~Container() { cout << "Container destoryed..." << endl; }
};



int main()
{
    Container c1("c1", 1);
    cout << endl;

    auto c2(move(c1));
    cout << endl;

    c2 = Container("c3", 3);
    cout << endl;

    cout << "c1: " << c1.getName() << endl;
    cout << "c2: " << c2.getName() << endl;
    cout << endl;

    Container c4("c4", 4);
    cout << endl;

    cout << "swap c2 and c4...." << endl;
    swap_E<Container>(c2, c4);
    cout << "c2: " << c2.getName() << endl;
    cout << "c4: " << c4.getName() << endl;
    cout << endl;

    Element e1("e1");
    Element e2("e2", 1);
    cout << "e1: " << e1.getName() << endl;
    cout << "e2: " << e2.getName() << endl;
    cout << "swap....." << endl;
    swap_E<Element>(e1, e2);
    cout << "e1: " << e1.getName() << endl;
    cout << "e2: " << e2.getName() << endl;
    cout << endl;
}
```
output:
```javascript
Element created....
Container created....

Element using rvalue constructor...
Container using rvalue constructor...

Element created....
Container created....
Element using rvalue operator= overload....
Container using rvalue operator= overload...
Container destoryed...
Element destoryed...

c1:
c2: c3

Element created....
Container created....

swap c2 and c4....
Element using rvalue constructor...
Container using rvalue constructor...
Element using rvalue operator= overload....
Container using rvalue operator= overload...
Element using rvalue operator= overload....
Container using rvalue operator= overload...
Container destoryed...
Element destoryed...
c2: c4
c4: c3

Element created....
Element created....
e1: e1
e2: e2
swap.....
Element using rvalue constructor...
Element using rvalue operator= overload....
Element using rvalue operator= overload....
Element destoryed...
e1: e2
e2: e1

Element destoryed...
Element destoryed...
Container destoryed...
Element destoryed...
Container destoryed...
Element destoryed...
Container destoryed...
Element destoryed...
```

