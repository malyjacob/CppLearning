#include <iostream>
#include <memory>

class Resource
{
public:
    Resource() { std::cout << "Resource aquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
};

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
    operator bool() const { return m_ptr == nullptr ? false : true; } // test if the smart pointer is null

    // a copy constructor that implements move semantics
    Smart_ptr(Smart_ptr& a) // note: no const
    {
        m_ptr = a.m_ptr;
        a.m_ptr = nullptr;
    }

    Smart_ptr& operator=(Smart_ptr& a)
    {
        if(&a == this)
            return *this;

        delete m_ptr;  // don't forget!
        m_ptr = a.m_ptr;
        a.m_ptr = nullptr;
        return *this;
    }
};


int main()
{
    Smart_ptr<Resource> res1(new Resource());
    Smart_ptr<Resource> res2; // start with null

    std::cout << "res1 is " << (res1 ? "not null\n" : "null\n");
    std::cout << "res2 is " << (res2 ? "not null\n" : "null\n");

    res2 = res1;

    std::cout << "ownership has been transferred\n";

    std::cout << "res1 is " << (res1 ? "not null\n" : "null\n");
    std::cout << "res2 is " << (res2 ? "not null\n" : "null\n");

    return 0;
}
