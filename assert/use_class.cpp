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