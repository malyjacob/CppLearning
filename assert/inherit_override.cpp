#include <iostream>

class Base
{
protected:
    int m_value {};

public:
    Base(int value) : m_value(value) {}

    void identity() const { std::cout << "I am a Base" << std::endl; }

    friend std::ostream & operator<< (std::ostream & os, const Base & b)
    {
        os << "In Base" << std::endl;
        os << b.m_value << std::endl;
        return os;
    }
};

class Derived : public Base
{
public:
    Derived(int value) : Base(value) {}
    void identity() const { Base::identity(); std::cout << "I am a Derived" << std::endl; }
    friend std::ostream & operator<< (std::ostream & os, const Derived & d)
    {
        os << "In Derived" << std::endl;
        os << static_cast<const Base&>(d);
        return os;
    }
};

int main()
{
    Derived derived(7);

    std::cout << derived << std::endl;
    
    return 0;
}