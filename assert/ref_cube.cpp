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