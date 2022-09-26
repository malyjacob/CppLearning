#include <iostream>

using namespace std;

template <class T, int n>
class Add
{
public:
    T *operator()(const T a[n], const T b[n])
    {
        T *result = new T[n];
        for (int i = 0; i < n; i++)
            result[i] = a[i] + b[i];
        return result;
    }
};

template <class T, template <class U> class F, int n>
class Traverse
{
public:
    void operator()(T t[n])
    {
        for(int i = 0; i < n; i++)
            F<T>()(t[i]);
    }
};

template <class T>
class Fn
{
public:
    void operator()(const T &t)
    {
        cout << t << endl;
    }
};

int main()
{
    int a[5] = {1, 2, 3, 4};
    int b[5]{5, 4, 5, 6, 1};

    int *result = Add<int, 5>()(a, b);

    Traverse<int, Fn, 5>()(result);
}