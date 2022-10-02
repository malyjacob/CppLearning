#include <iostream>

class Base
{
private:
    int m_value {};
public:
    Base(int value) : m_value(value) {}
protected:
    void printValue() const { std::cout << m_value << std::endl; }
};

class Derived : public Base
{
public:
    Derived(int value) : Base(value) {}
    
    // Base::printValue was inherited as protected, so the public has no access
    // But we're changing it to public via using declaration
    using Base::printValue;
};

int main()
{
    Derived derived(7);

    // it will work! printValue is public in Derived
    derived.printValue();

    return 0;
}