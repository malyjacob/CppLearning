#include <string>
#include <iostream>
#include <cstring>

class Person
{
    std::string m_name {};
    int m_age {};

public:
    Person(const char *name = "", int age = 0)
        : m_name(name), m_age(age) {}

    const std::string & getName() const { return m_name; }
    int getAge() const { return m_age; }
};


class BaseballPlayer : public Person
{
    double m_battingAverage {};
    int m_homeRuns {};

public:
    BaseballPlayer(double batingAverage = 0.0, int homeRuns = 0)
        : m_battingAverage(batingAverage), m_homeRuns(homeRuns) {}
};


class Base
{
    int m_id {};
public:
    Base(int id = 0) : m_id(id) { std::cout << "Base" << std::endl; }
    int getId() const { return m_id; }
    ~Base() { std::cout << "delete Base...." << std::endl; }
};


class Derived: public Base
{
    double m_cost {};
public:
    Derived(double cost = 0.0, int id = 0) : Base(id), m_cost(cost) { std::cout << "Derived" << std::endl; }
    double getCost() const {  return m_cost; }
    ~Derived() { std::cout << "delete Derived...." << std::endl; }
};

class BaseDMA
{
private:
    char *label;
    int rating;
public:
    BaseDMA(const char *lb = "null", int rt = 0) : rating(rt)
    {
        int len = strlen(lb);
        label = new char[len + 1];
        strcpy(label, lb); 
    }
    BaseDMA(const BaseDMA & base) : rating(base.rating)
    {
        int len = strlen(base.label);
        label = new char[len + 1];
        strcpy(label, base.label);
    }
    BaseDMA & operator= (const BaseDMA & base)
    {
        rating = base.rating;
        delete[] label;
        int len = strlen(base.label);
        label = new char[len + 1];
        strcpy(label, base.label);
        return *this;
    }
    ~BaseDMA() { delete[] label; }
};

class LackDMA : public BaseDMA
{
    char color[40];
public:
    //.....
};

class HasDMA : public BaseDMA
{
    char *style;
public:
    HasDMA(const char *sty) // implicitily use BaseDMA's defalut construcor
    {
        int len = strlen(sty);
        style= new char[len + 1];
        strcpy(style, sty); 
    }
    ~HasDMA() { delete[] style; } // have to specify the destructor
    HasDMA(const HasDMA & has) : BaseDMA(has) // `has` upcasting..
    {
        int len = strlen(has.style);
        style= new char[len + 1];
        strcpy(style, has.style); 
    }
    HasDMA & operator= (const HasDMA & has)
    {
        if(this == &has)
            return *this;
        BaseDMA::operator=(has); // copy base portion
        delete[] style;
        style = new char[strlen(has.style) + 1];
        strcpy(style, has.style);
        return *this;
    }
};

int main()
{
    Derived derived(1.3, 5);
    std::cout << "Id: " << derived.getId() << std::endl;
    std::cout << "Cost: " << derived.getCost() << std::endl;

    return 0;
}