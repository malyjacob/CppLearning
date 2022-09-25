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

class Resource;

template <class T>
void better_swap(T &t1, T &t2)
{
    auto temp = std::move(t1);
    t1 = std::move(t2);
    t2 = std::move(temp);
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
    Resource(Resource &&res)
    {
        cout << "using Resource's rvalue copy construcotr...." << endl;
        cout << "move the ownership of the value" << endl;
        value = res.value;
        res.value = nullptr;
        cout << "Resource created" << endl;
    }
    ~Resource()
    {
        cout << "Resource destoryed" << endl;
        if (value)
        {
            cout << "delete the value...." << endl;
            delete[] value;
        }
    }
    Resource &operator=(const Resource &res)
    {
        cout << "using Resource's operator= overload...." << endl;
        int len = strlen(res.value);
        if (value)
        {
            cout << "delete the origin value...." << endl;
            delete[] value;
        }
        cout << "create a new empty value...." << endl;
        value = new char[len + 1];
        strcpy(value, res.value);
        return *this;
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
    operator bool() { return value ? true : false; }
};

template <>
void swap<Resource>(Resource &r1, Resource &r2)
{
    char *temp_value = r1.value; // temporary value to be passed
    r1.value = r2.value;
    r2.value = temp_value;
}

void show_v(Resource s)
{
    cout << "using by value..." << endl;
    cout << "the value is: " << s.value << endl;
}

void show_l(const Resource &s)
{
    cout << "using lvalue...  ";
    cout << "the value is: " << s.value << endl;
}

void show_r(const Resource &&s)
{
    cout << "using rvalue...  ";
    cout << "the value is: " << s.value << endl;
}

void consume(Resource &s)
{
    cout << "consumming..." << endl;
    Resource res = std::move(s);
    if (!s)
        cout << "the s's value hase been consumed..." << endl;
}

int main()
{
    Resource res1("res1");
    Resource res2("res2");
    cout << "res1 value = " << res1.value << endl;
    cout << "res2 value = " << res2.value << endl;
    cout << endl;

        /* cout << "using swapping...." << endl;
        swap<Resource>(res1, res2);
        cout << endl;
        cout << "res1 value = " << res1.value << endl;
        cout << "res2 value = " << res2.value << endl;
        cout << endl; */

        cout << "using better swapping...." << endl;
        better_swap<Resource>(res1, res2);
        cout << endl;
        cout << "res1 value = " << res1.value << endl;
        cout << "res2 value = " << res2.value << endl;
        cout << endl;

}
