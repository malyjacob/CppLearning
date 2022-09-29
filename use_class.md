# Use Class

这个部分主要继续前一章的内容, 讲讲类里面常用的`运算符重载`, `友元函数`和`类的转换`.

## 运算符重载

下面就来介绍运算符重载, 有了运算符重载, 我们对对象的操作就更加的优雅. 而与函数重载一样, 运算符重载也算作是Cpp的一种多态的实现.

Cpp允许我们将运算符重载扩展到用户自定义的类型.

例如, 之前两个数组的相加, 我们会写成这样:
```javascript
for(int i = 0; i < 10; i++)
    a[i] = b[i] + c[i];
```

而如果我们使用运算符重载, 我们带概率会写成这样:
```javascript
a = b + c;
```
而这种写法是由这个类似的伪代码函数支持的:
```javascript
operator+(args);
```
这里的加法被重载了, 不是数字意义上的加法, 并且这里面隐藏了很多的实现细节, 使得我们能够更加简洁优雅的去使用对象.

闲话少说, 我们就来看看怎么使用, 我们打算构造一个二维向量, 这个二维向量实现了加法, 乘法, 取lambda的乘法:
```javascript
#include <iostream>

using std::cout;    using std::endl;

class Vector
{
    double x_index;
    double y_index;
public:
    Vector(double x = 0, double y = 0) : x_index(x), y_index(y) {}
    Vector operator+ (const Vector & vec)
    {
        Vector result;
        result.x_index = this->x_index + vec.x_index;
        result.y_index = this->y_index + vec.y_index;
        return result;
    }
    double operator* (const Vector & vec)
    {
        return this->x_index * vec.y_index - this->y_index * vec.x_index;
    }
    Vector operator* (double lambda)
    {
        Vector result;
        result.x_index = this->x_index * lambda;
        result.y_index = this->y_index * lambda;
        return result;
    }
    void show()
    {
        cout << "x_index is: " << x_index << endl;
        cout << "y_index is: " << y_index << endl;
        cout << endl;
    }
};  
```
我们可以看到运算符重载的格式是这样子的:
`<Return_Type> operator<op\> (<arg_list\>)`

并且运算符重载也可以像函数重载一样, 只要特征标不同就可以指定任意多个同一个运算符的重载.

用用这个类:
```javascript
int main()
{
    Vector a(3, 5);
    Vector b(0, 1);

    a.show();
    b.show();

    cout << "use operator +" << endl;
    Vector c = a + b;
    c.show();

    cout << "use operator * between Vector" << endl;
    auto d = a * b;
    cout << "the result of the multiply between a and b is: " << d << endl;

    cout << "use operator * by lambda" << endl;
    auto e = c * 1.2;
    e.show();
}
```
output:
```javascript
x_index is: 3
y_index is: 5

x_index is: 0
y_index is: 1

use operator +
x_index is: 3
y_index is: 6

use operator * between Vector
the result of the multiply between a and b is: 3
use operator * by lambda
x_index is: 3.6
y_index is: 7.2

```

## 运算符重载限制

通过运算符重载, 我们能够更加简洁自由的操作对象, 但运算符重载本事并不是完全自由的, 有部分运算符不能被重载!

不能被重载的运算符有:

    .   (成员访问或点操作符)
    ?:  (三元操作符或条件操作符)
    ::  (域操作符)
    .* (指向成员的指针操作符)
    sizeof(取对象大小操作符)
    typeid(对象类型操作符)

而有部分运算符只能通过成员函数重载:

    = 赋值运算符
    () 函数调用运算符
    [] 下标运算符
    -> 通过指针访问类成员的运算符

另外需要注意的是, 即使在可以被重载的运算符, 也是有一定的规则的.

1. 重载后的运算符必须至少有一个操作数是用户自定义类型

2. 使用运算符不能违反运算符自身的句法规则

3. 不能修改运算符原来的优先级

## 友元函数

一般来说, 公有类方法是访问类内部的私有成员的唯一途径. 但有时候, 这个规则未免太过严格.

因此Cpp提供了另外一种访问类私有成员的新形势: `友元`.

一般来说, 友元分为三种:

- 友元函数
- 友元类
- 友元成员函数.

通过让函数, 或者另外一个类, 或者另外一个类的成员函数为目标类的友元, 这个友元就有了合法访问目标类的私有成员的权限.

在这里先介绍友元函数, 后面那两位, 等之后再来讲.

回来我们这个`Vector`类, 仔细观察一下那个支持lambda的乘法运算符重载. 我们会发现, 这个类不支持这种形式: `A = 1.2 * B;`.

但是这种互换位置的乘法也是十分常见的. 那么该怎么办才能解决这个问题呢? 这就要请出友元函数版本的的运算符重载了!

我们先来看看如何创建友元函数.

创建友元函数就是将其原型放进类声明中, 并且在原型声明前加上`friend`关键词, 针对上面这个, 我们可以这样来声明`Vector`的lambda乘法的友元版本的重载:
```javascript
firend Vector operator* (double lambda);
```
> 注意: 虽然这个函数定义在类里面, 但是它并不是类成员函数, 因此如果在类外定义这个函数, 是不用作用域解析符的. 还有就是定义函数时, 不用加上`firend`关键字.

它的类外实现是这样子的:
```javascript
Vector operator*(double lambda, const Vector &vec)
{
    Vector result;
    result.x_index = vec.x_index * lambda;
    result.y_index = vec.y_index * lambda;
    return result;
}
```
这样我们就可以如上面这种形式使用乘法了.

好的! 我们的也行更大些! 这个类的`show()`太难看了, 我们能不能像`string`类一样, 可以如`cout << string("ss");`
那样使用啊?

答案是可以通过友元版本的运算符`<<`重载做到!

声明:
```javascript
friend std::ostream &operator<<(std::ostream &os, const Vector &vec);
```

实现:
```javascript
std::ostream &operator<<(std::ostream &os, const Vector &vec)
{
    os << "x_index is: " << vec.x_index << endl;
    os << "y_index is: " << vec.y_index << endl;
    return os;
}
```

那么就可以这样使用了:
```javascript
int main()
{
    Vector a(3, 5);
    Vector b(0, 1);

    cout << a << endl; // use friend
    cout << b << endl;
    cout << "use operator +" << endl;
    Vector c = a + b;
    cout << c << endl;

    cout << "use operator * between Vector" << endl;
    auto d = a * b;
    cout << "the result of the multiply between a and b is: " << d << endl;

    cout << "use operator * by lambda" << endl;
    auto e = 1.2 * c; // use friend
    cout << e << endl;
}
```
output:
```javascript
x_index is: 3
y_index is: 5

x_index is: 0
y_index is: 1

use operator +
x_index is: 3
y_index is: 6

use operator * between Vector
the result of the multiply between a and b is: 3
use operator * by lambda
x_index is: 3.6
y_index is: 7.2

```

## 类之间的转换

我们知道, Cpp内置的类型转换是十分常见的, 例如各种不同精度类型的数字之间的类型转换. 

而类型转换分为自动转换, 和强制转换.

而我们的自定义类也可以实现类似这种类型转换!

我们先来看看其它类型转换为自定义类型是怎么样的.

其实, 这种转换是依靠构造函数来实现的.

假如我要将`int`类型转换为`Vector`, 那么我就重载一个只接受`int`类型并且只有一个参数的构造函数:
```javascript
Vector(int d) :  x_index(d), y_index(0) {}
```
这样我们就可以这样
```javascript
Vector v = 1; // auto convert
Vector v2 = (Vector)2; // forced convert
```
当然其实我那么原来的构造函数其实也支持类型转换, 是`double`值转`Vector`. 因为它有默认值,使得可以只把一个参数传入构造函数.
```javascript
Vector v3 = 1.3; // auto convert
Vector v4 = (Vector)2.1; // forced convert
```

有些时候, 这种自动转换是十分令人头疼的, 因为我们写着写着, 就很有可能神不知鬼不觉地使用了类型转换.强制类型转换反倒更加清晰.

而cpp提供了`explicit`关键字, 当我们把它放到构造函数原型的开头就可以禁用自动转换, 只能使用强制类型转化.
好的, 我在原构造函数前加了个`explicit`. 
```javascript
    explicit Vector(double x = 0, double y = 0) : x_index(x), y_index(y) {}
```

好的! 现在我们反过来, 如何把`Vector`类转化为`double`. 这个是十分有用的. 例如, 我打算用这种转换来直接求得这个向量的`模长`.

Cpp 给出的方案是这样子的:
```javascript
operator double();
```
当然, 这个转换函数也可以加上`explicit`来禁用自动转换.

> 注意! 这个函数是没有写返回值的, 但是它函数体内确实需要`return`! 另外, 我们不能指定这个函数任何参数!

那么, 我的求模长的转换函数是这样子的:
```javascript
    operator double()
    {
        return sqrt(x_index * x_index + y_index * y_index); // should include header file <cmath>
    }
```
这样就可以这样子了:
```javascript
Vector v(3, 4);

double len = v; //=> 5;
```

## 附加

完整代码实现如下:
```javascript
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

class Vector
{
    double x_index;
    double y_index;

public:
    explicit Vector(double x = 0, double y = 0) : x_index(x), y_index(y) {}
    Vector(int d) : x_index(d), y_index(0) {}
    Vector operator+(const Vector &vec)
    {
        Vector result;
        result.x_index = this->x_index + vec.x_index;
        result.y_index = this->y_index + vec.y_index;
        return result;
    }
    double operator*(const Vector &vec)
    {
        return this->x_index * vec.y_index - this->y_index * vec.x_index;
    }
    Vector operator*(double lambda)
    {
        Vector result;
        result.x_index = this->x_index * lambda;
        result.y_index = this->y_index * lambda;
        return result;
    }
    void show()
    {
        cout << "x_index is: " << x_index << endl;
        cout << "y_index is: " << y_index << endl;
        cout << endl;
    }

    friend Vector operator*(double lambda, const Vector &vec);
    friend std::ostream &operator<<(std::ostream &os, const Vector &vec);

    operator double()
    {
        return sqrt(x_index * x_index + y_index * y_index);
    }
};

Vector operator*(double lambda, const Vector &vec)
{
    Vector result;
    result.x_index = vec.x_index * lambda;
    result.y_index = vec.y_index * lambda;
    return result;
}

std::ostream &operator<<(std::ostream &os, const Vector &vec)
{
    os << "x_index is: " << vec.x_index << endl;
    os << "y_index is: " << vec.y_index << endl;
    return os;
}

int main()
{
    Vector a(3, 4);
    Vector b(0, 1);

    cout << a << endl;
    cout << b << endl;
    cout << "use operator +" << endl;
    Vector c = a + b;
    cout << c << endl;

    cout << "use operator * between Vector" << endl;
    auto d = a * b;
    cout << "the result of the multiply between a and b is: " << d << endl;

    cout << "use operator * by lambda" << endl;
    auto e = 1.2 * c;
    cout << e << endl;

    cout << "use double converting to Vector..." << endl;
    Vector v = 1;          // auto convert
    Vector v2 = (Vector)2; // forced convert

    cout << v << endl;
    cout << v2 << endl;

    cout << "the lenght of a is: " << (double)a << endl;
}
```

