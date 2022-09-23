# shared_ptr

**前情:**
> [unique_ptr](./unique_ptr.md)
<br/>

不同于`unnique_ptr`的独占性, `shared_ptr`是专门用来多个智能指针指向同一个资源的.

在`shared_ptr`内部, `shared_ptr`持续追踪有多少个`shared_ptr`指向它所指向的资源.

在多个`shared_ptr`指向同一资源时, 单个`shared_ptr`的销毁是无法销毁其所指向的资源的.

一般情况下, 当且仅当指向资源的最后一个`shared_ptr`被销毁时, 这个资源才能得到释放.

走个例子:
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

int main()
{
    Resource *res = new Resource();

    shared_ptr<Resource> ptr1(res);
    {
        shared_ptr<Resource> ptr2 = ptr1;
        cout << "Killing one shared ptr" << endl;
    }
    cout << "Killing another shared ptr" << endl;

    return 0;
}
```
output:
```javascript
Resource acquired
Killing one shared ptr
Killing another shared ptr
Resource destoryed

```
我们可以看到, 当销毁第二个`shared_ptr`时, 其所指向的资源并没有被销毁. <br/>
而是等到`main`函数结束时, 也就是最后一个`shared_ptr`销毁时, 才最终被释放.

`shared_ptr`可以从`unnique_ptr`通过构造函数生成过来. <br/>
只不过, 之后,原来的`unnique_ptr`就失去资源的所有权, 所有权被转移到`shared_ptr`.

那么可以从`shared_ptr`转换为`unnique_ptr`吗? <br/>
答案是不能哈. <br/>
理由也很简单, 可能有多个`shared_ptr`指向同一个资源, 如果直接转换的话, 那么剩下的`shared_ptr`还共享不共享这个资源,
那么这个新的`unnique_ptr`还独占不独占这个资源? <br/>

## make_shared
`make_shared`跟`unnique_ptr`类似, 都是智能指针生成函数. 不过这个是生成`shared_ptr`的.

还有就是, `shared_ptr`不能用于数组, 因此`make_shared`的参数列表只跟`make_unique`的非数组版本一样.

下面是使用`make_shared`的一个例子:
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

int main()
{
    auto ptr1 = make_shared<Resource>();
    {
        auto ptr2 = ptr1; // using copy constructor
        cout << "Killing one shared ptr" << endl;
    }
    cout << "Killing another shared ptr" << endl;

    return 0;
}
```
这里的输出跟上面的例子一样.

这里, 先建议, 如果要初始化`shared_ptr`,最好直接用`make_shared`,而不是用`shared_ptr`的构造函数.

## shared_ptr 的大致原理
shared_ptr`本质上还是个包含指针的对象.里面有两个重要指针, 
- 一个指向资源对象
- 一个则指向计数器, 用来计算引用所管理的资源的`shared_ptr`的个数.

当`shared_ptr`创建时, 就包装资源对象, 并且生成一个计数器.

当使用赋值运算符或者复制构造函数时, 等号右边的`shared_ptr`的资源指针值和计数器指针值会复制到左边,同时计数器加1.

每当销毁一个`shared_ptr`这个`shared_ptr`指向的计数器就会减1.
每当一个`shared_ptr`被复制时, 指向的计数器就会加1.

如果计数器等于0, 那么这个资源就会被释放.

我们可以调用成员方法`use_count()`来获得引用数.

## shared_ptr 的尴尬之处
正如前面所说, 有个计数器会计算资源的引用量, 然后等到引用数为0时, 就自动释放资源.

但不幸的是, 有些时候,指向同一个资源的`shared_ptr`们, 自己的计数器会不一致.

来看看这个例子:
```javascript
int main()
{
    Resource *res = new Resource();

    shared_ptr<Resource> res1(res);
    {
        shared_ptr<Resource> res2(res);
        cout << "Killing one shanred ptr" << endl;
    }
    return 0;
}
```
output:
```javascript
Resource acquired
Killing one shanred ptr
Resource destoryed
Resource destoryed
free(): double free detected in tcache 2
zsh: IOT instruction (core dumped)  ./shared_ptr_error.out

```
Crash了.

这里面显示的问题是资源二次释放.

为什么呢?

虽然两个`shared_ptr`指向同一个资源, 但这两个`shared_ptr`的计数器却不同步, <br/>
因为这两个`shared_ptr`都不户知道对方的存在, 各自都维护这各自的计数器, <br/>
也就是说, 他们两所指向的计数器不是同一个计数器. <br/>

当`ptr2`创建时, 是直接根据原指针而创建的, 并不是通过把`ptr1`复制过来而创建的. <br/>
于是,此时会另外生成一个计数器, 然后`ptr2`指向它, 而不是指向`ptr1`所指向的计数器.

因此当`ptr2`离开作用域时, `ptr2`所指向的计数器由1减到0, 于是就销毁所指向的对象.
而这个对象却也是`ptr1`所指的对象, 并且`ptr1`的计数器还是1!

然后当`ptr1`离开作用域时,`ptr1`计数器为0, 它也尝试去释放所指向的对象, 但这个对象早就被`ptr2`释放了....

这就是产生错误的缘由.

## 部分规范建议
- 不直接从裸指针生成`shared_ptr`, 原因跟[unique_ptr](./unique_ptr.md)的一样!

- 尽量使用`make_shared`而不是构造函数来生成`shared_ptr`, 原因还是与[unique_ptr](./unique_ptr.md)一样!

- 当要使用多个`shared_ptr`指向同一资源时, 建议先使用`make_shared`生成第一个指向资源的`shared_ptr`.之后指向这个资源的`shared_ptr`的指针都要从第一个`shared_ptr`或者由第一个`shared_ptr`层层复制而来的`shared_ptr`复制过来.(有点绕哈).
