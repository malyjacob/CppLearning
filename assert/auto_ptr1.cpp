#include <string>
#include <iostream>
#include <memory>
//有问题, 忘了释放指针
void fn1(std::string & str)
{
    std::string *ps = new std::string(str);
    std::cout << *ps << std::endl;
}


//有问题, 当if通过时,直接走出函数域,没有执行底下的释放语句.
void fn2(std::string & str)
{
    std::string *ps = new std::string(str);

    if(ps->length() > 16)
        return;
    else
    std::cout << *ps << std::endl;

    delete ps;
}

// use smart pointer
void smart_fn1(std::string & str)
{
    std::unique_ptr<std::string> ps(new std::string(str)); //
    std::cout << *ps << std::endl;
}

// use smart pointer
void smart_fn2(std::string & str)
{
    std::unique_ptr<std::string> ps(new std::string(str));
    if(ps->length() > 16)
        return;
    else
    std::cout << *ps << std::endl;
}

template<class T>
class Smart_ptr
{
    T *m_ptr;
public:
    Smart_ptr(T *ptr = nullptr) : m_ptr(ptr) {}
    ~Smart_ptr() { delete m_ptr; }

    // overload the operators
    T& operator*() const { return *m_ptr; }
    T *operator->() const { return m_ptr; }
};

class Resource
{
public:
    Resource() { std::cout << "Resource aquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
};

int main()
{
    Smart_ptr<Resource> res(new Resource());
    Smart_ptr<Resource> res2(res); // Error!
    return 0;
}