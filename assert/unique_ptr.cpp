#include <iostream>
#include <memory>

using namespace std;

class Fraction
{
    int m_numberator = 0;
    int m_denominator = 1;
public:
    Fraction(int num = 0, int deno = 1)
        : m_numberator(num), m_denominator(deno) {}
    friend ostream& operator<<(ostream& os, const Fraction& f)
    {
        os << f.m_denominator << '/' << f.m_denominator;
        return os;
    }
};

int main()
{
    auto f1 = make_unique<Fraction>(3, 5);
    cout << *f1 << endl;

    auto f2 = make_unique<Fraction[]>(4);
    cout << f2[2] << endl;

    return 0;
}