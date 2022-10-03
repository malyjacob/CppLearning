#include <iostream>

class Base
{
private:
    int m_value {};
public:
    Base(int value) : m_value(value) {}
    virtual ~Base() = default;
    Base(const Base & base)
    {
        std::cout << "using copy constructor..." << std::endl;
        m_value = base.m_value;
    }
    Base & operator= (const Base & base)
    {
        std::cout << "using operator = ..." << std::endl;
        m_value = base.m_value;
        return *this;
    }
};

class Derived final : public Base
{
public:
    Derived(int value) : Base(value) {}
    virtual ~Derived() override = default; 
    Derived(const Derived &) = delete;
    Derived & operator=(const Derived &) = delete;
};

int main()
{
    // using copy constructor
    Base base1 = Derived(5);

    Derived derived(7);

    // using operator =
    base1 = derived;
}