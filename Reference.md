# Reference

相比于C, Cpp有一种新的类型----引用变量.

引用变量本质上就是已经定义的变量的别名.

引用变量主要用于函数的形参, 通过将引用变量用作参数, 函数将直接访问实参的内容, 而不是其副本.

这样就可以摆脱之前, 在C里,只能按值传递的困境. 为那些之前没学好指针的人一条崭新的道路.😤

这篇主要是将引用类型的实质, 说明它是如何工作的, 而具体的实践指南则在下面两章:

- **[As Parameter](./ref_as_param.md)**
- **[As Returned Type](./ref_as_return.md)**
<br/>

## 创建引用变量
如何创建引用变量呢? 看下面:
```javascript
int rats;
int& rodents = rats;
```
这个`rodents`就是`rats`的引用变量. 而那个`&`现在的意思并不是取地址运算符, 而是用来声明引用变量来用的.

来看看具体怎么样:
```javascript
#include <iostream>

using namespace std;

int main()
{
    int rats = 101;
    int& rodents = rats; // rodents is a reference to rats.

    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    rodents++; // !!!!
    cout << "Attention!!!!" << endl;

    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    cout << "look the adress" << endl;
    cout << "&rats = " << &rats << endl;
    cout << "&rodents = " << &rodents << endl;

    return 0;
}
```
output:
```javascript
rats = 101
rodents = 101
Attention!!!!
rats = 102
rodents = 102
look the adress
&rats = 0x61ff08
&rodents = 0x61ff08
```
上面我们已经说过, 引用本质上就是已定义的变量的一个别名.

因此, 我们可以看到`rats` 和 `rodents`的值与地址都是一样的, 并且当`rodents`自增后, 发现`rats`也加1了, 这个例子就可以证明引用就是变量的别名.
<br/>

## 引用的特性

我们可以发现, 引用的活动表现跟指针有点相像.
例如:
```javascript
int rats = 12;
int& rodents = rats;
int *prats = &rats;
```

这样的表达式`rodents`和`*prats`都可以与`rats`互换, 引用看起来就像是伪装的指针.

不过, 与指针可以先声明再赋值不同, 引用必须在声明的同时被初始化. 下面的行为是被禁止的:
```javascript
int rat;
int& rodent;
rodent = rat; // Not allowed!
```

引用一旦与某个变量关联起来, 就将在其生命周期内效忠于这个变量. 引用变量更接近带有`const`的指针.
也就是说: <br/>
`int& rodent = rat;`<br/>
实际上是下面代码的伪装: <br/>
`int * const ptr = &rat;` <br/>
其中, 引用`rodent`扮演的角色与表达式`*ptr`相同.

来看下面的例子:
```javascript
#include <iostream>

using namespace std;

int main()
{
    int rats = 12;
    int &rodents = rats;

    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    cout << "&rats = " << &rats << endl;
    cout << "&rodents = " << &rodents << endl;

    int bunnies = 13;
    rodents = bunnies; // !!!!!!!
    cout << "bunnies = " << bunnies << endl;
    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    cout << "&bunnies = " << &bunnies << endl;
    cout << "&rats = " << &rats << endl;
    cout << "&rodents = " << &rodents << endl;

    return 0;
}   
```
output:
```javascript
rats = 12
rodents = 12
&rats = 0x61ff08
&rodents = 0x61ff08
bunnies = 13
rats = 13
rodents = 13
&bunnies = 0x61ff04
&rats = 0x61ff08
&rodents = 0x61ff08
```
最初, `rodents`是指向`rats`的, 而后我们尝试`rodents = bunnies`.

乍一看, 好像`rodents`的指向转到了`bunnies`那里, 因为`rodents`的值确实与`bunnies`相等了.

但是, 等等! 我们注意到, `rats`的值也是`13`, 并且我们还发现`rodents`的地址依然与原来的`rats`一样, <br/>
但是却不与`bunnies`一样!

这其实就说明了, `rodents`并没有指向`bunnies`, 还依旧指向`rats`.

至于说, 为什么`rodents`的值变成跟`bunnies`一样的`13`, 那是因为, `rodents = bunnies`将`bunnies`的值复制到了`rodents`, 
也就是说, `bunnies`的值也复制到了`rats`那里, 因此, 才使得`rats`和`rodents`的值与`bunnies`一样.

## 上面只是一种体操, 并没有实际的应用价值, 引用真正大放异彩的地方是在这里:

### **[As Parameter](./ref_as_param.md)**

### **[As Returned Type](./ref_as_return.md)** 

## 当然还有右值引用

## ***[Right Reference](./right_ref.md)***

