#include <iostream>

class Base
{
protected:
    int m_value {};
public:
    Base(int value = 0) : m_value(value) {}
    virtual void showType()
    {
        std::cout << "I am Base, and my value is: " << m_value << std::endl;
    }
    virtual ~Base() { std::cout << "destoryed Base..." << std::endl; }
};

class Derived final : public Base
{
public:
    Derived(int value = 0) : Base(value) {}
    virtual void showType() override
    {
        std::cout << "I am Derived, and my value is: " << m_value << std::endl;
    }
    virtual ~Derived() override { std::cout << "destoryed Derived..." << std::endl; }
};

int main()
{
    Base * pb_derived = new Derived(5);
    pb_derived->Base::showType();

    delete pb_derived;
    return 0;
}