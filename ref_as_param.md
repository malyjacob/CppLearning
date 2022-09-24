# Reference as Parameter

**前情:**
> [Reference](./Reference.md)
<br/>

引用经常用于函数的形参, 使得函数中的变量名成为调用程序的变量的别名, 这种传递参数的方式叫做`按引用传递`, 与传统的C的`按值传递`相对.

现在我通过常见的操作---交换两个变量的值, 来说明`按引用传递`的用法.

```javascript
#include <iostream>

// use ref param
void swapr(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

//use pointer
void swapp(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// use value ------- Wrong!!!
void swapv(int a, int b)
{
    int temp = a;
    a = b;
    b = temp;
}

int main()
{
    using namespace std;
    int value_1 = 300;
    int value_2 = 500;
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    cout << "using ref to swap...." << endl;
    swapr(value_1, value_2);
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    cout << "using ptr to swap....." << endl;
    swapp(&value_1, &value_2);
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    cout << "using value to swap" << endl;
    swapv(value_1, value_2);
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    return 0;
}
```
output:
```javascript
value_1 = 300
value_2 = 500

using ref to swap....
value_1 = 500
value_2 = 300

using ptr to swap.....
value_1 = 300
value_2 = 500

using value to swap
value_1 = 300
value_2 = 500

```
`按引用传递`和`按值传递`看起来调用方法相同, 都是`swap(v1, v2)`, 但很显然后者交换失败, 而前者成功.

从观感上来说, `按引用传递`的`swap`比`传递指针`的`swap`好看些哈.
<br/>

## 引用参数的特别之处
在使用引用参数时, 我们需要注意它的一些特性, 下面的例子虽然写的比较奇怪, 但至少能让我们明显的感受到其中的特别之处.

```javascript
#include <iostream>

double cube(double a)
{
    a *= a * a;
    return a;
}

double refcube(double &a)
{
    a *= a * a;
    return a;
}

int main()
{
    using namespace std;
    double x = 3.0;

    cout << cube(x);
    cout << " = cube of " << x << endl;
    
    cout << refcube(x);
    cout << " = cube of " << x << endl;

    return 0;
}
```
output:
```javascript
27 = cube of 3
27 = cube of 27
```
为什么会出现这种情况?

这是因为在`refcube`里面会改变传进来的引用值, 也就是说会改变传进来的实参的值.

如果我们想要让函数使用传递给它的信息, 而不对这些信息进行修改, 同时又想要用引用, 那么最好的办法就是使用常量引用:
`double refcube(const double &a)` <br/>
这样, 当编译器发现在这个函数体内试图修改常量引用, 就会生成错误信息.

一般来说, 如果函数所要处理的对象体积比较大, 那么比较好的办法就是将这个对象用作引用值, 而不是直接`按值传递`, 至于为什么, 后面会同一谈一下.

## 临时变量, 引用参数和`const`
如果函数使用`按引用传递`, 并且实参与引用参数不匹配, 那么Cpp的编译器会这样做:<br/>
- 如果使用`const`引用, 则会生成临时变量
- 否则, 立马报错.

如果引用参数使用了`const`, 临时变量将在如下情形中生成:
- 实参类型正确, 但不是左值
- 实参类型不正确, 但可以转换为正确的类型

左值是什么? 就是可以被引用的数据对象, 通俗点就是可以被取地址, 可以通过它们的地址去访问它们.

不过常规变量属于可修改左值, 而`const`变量则属于不可修改左值.

回到前面的例子, 假设`refcube`是这样子的:
```javascript
double refcube(const double &ra)
{
    return ra * ra * ra;
}
```
现在让我们看看如下代码:
```javascript
double side = 3.0;
double *pd = &side;
double &rd = side;
long edge = 5L;

double lens[4] = { 2.0, 5.0, 10.0, 12.0 };

double c1 = refcube(side); // ra is side
double c2 = refcube(lens[2]); // ra is lens[2]
double c3 = refcube(rd); //ra is rd is side
double c4 = refcube(*pd); // ra is *pd is side

double c5 = refcube(edge); // ra is temporary variable
double c6 = refcube(7.0); // ra is temporary variable
double c7 = refcube(side + 10.0); // ra is temporary variable
```

那么为什么常量引用可以使用临时变量, 而引用就会报错呢?

答案是: 如果接受引用参数的函数意图就是修改作为参数传递的变量, 则创建临时变量将阻止这种意图的实现.

解决办法就是, 禁用创建临时变量.

对于`const`引用来说, 如果实参不匹配, 由于`const`不试图修改引用变量, 只是函数想要用传进来的参数的信息, 因此这样不会影响这个函数的功能,. 这样就相当于`按值传递`.

## 使用引用参数的建议
- 首先要明确这个函数的意图是什么? 如果是为了修改引用参数的值, 那么就直接用, 如果只是为了使用这里面的信息,不修改, 那么最好加上`const`.

- 对于大型数据类型, 使用引用参数是明智的, 除非是数组.

- 对于数组, 引用参数是不合适的, 建议使用指针.

- 一般情况下, 除了要修改值外, 其他一律用`const`引用.理由如下:

1. 使用`const`可以避免五一修改数据的错误.
2. 使用`const`能够处理`const`和fei`const`实参, 否则只能接受非`const`数据.
3 使用`const`引用使得函数能够正确生成并且使用临时变量.



