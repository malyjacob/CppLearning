#include <iostream>

using namespace std;

class Base
{
public:
    int m_public {};
protected:
    int m_protected {};
private:
    int m_private {};
};

class Derived : public Base
{
public:
    Derived()
    {
        m_public = 1; // allowed
        m_protected = 2; // allowed
        m_private = 3; // not allowed
    }
};

int main()
{
    Base base;
    base.m_public = 1;
    base.m_protected = 2; // not allowed
    base.m_private = 2; // not allowed

    return 0;
}

