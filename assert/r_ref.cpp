#include <iostream>
#include <string>

using namespace std;

template <class T>
void swap_E(T &e1, T &e2)
{
    T temp(move(e1));
    e1 = move(e2);
    e2 = move(temp);
}


class Element
{
    string name;
    int id;

public:
    Element(const char *nm, int i = 0) : name(nm), id(i)
    {
        cout << "Element created...." << endl;
    }
    Element(Element &&ele)
    {
        id = ele.id;
        name = move(ele.name);
        cout << "Element using rvalue constructor..." << endl;
    }
    Element &operator=(Element &&ele)
    {
        id = ele.id;
        name = move(ele.name);
        cout << "Element using rvalue operator= overload...." << endl;
        return *this;
    }
    ~Element() { cout << "Element destoryed..." << endl; }
    const string &getName() { return name; }
};

class Container
{
    Element element;
    int id;

public:
    Container(const char *nm, int i = 0) : element(nm, i)
    {
        id = i;
        cout << "Container created...." << endl;
    }
    Container(Container &&ctr) : element(move(ctr.element))
    {
        id = ctr.id;
        cout << "Container using rvalue constructor..." << endl;
    }
    Container &operator=(Container &&ctr)
    {
        id = ctr.id;
        element = move(ctr.element);
        cout << "Container using rvalue operator= overload..." << endl;
        return *this;
    }
    const string &getName() { return element.getName(); }
    ~Container() { cout << "Container destoryed..." << endl; }
};



int main()
{
    Container c1("c1", 1);
    cout << endl;

    auto c2(move(c1));
    cout << endl;

    c2 = Container("c3", 3);
    cout << endl;

    cout << "c1: " << c1.getName() << endl;
    cout << "c2: " << c2.getName() << endl;
    cout << endl;

    Container c4("c4", 4);
    cout << endl;

    cout << "swap c2 and c4...." << endl;
    swap_E<Container>(c2, c4);
    cout << "c2: " << c2.getName() << endl;
    cout << "c4: " << c4.getName() << endl;
    cout << endl;

    Element e1("e1");
    Element e2("e2", 1);
    cout << "e1: " << e1.getName() << endl;
    cout << "e2: " << e2.getName() << endl;
    cout << "swap....." << endl;
    swap_E<Element>(e1, e2);
    cout << "e1: " << e1.getName() << endl;
    cout << "e2: " << e2.getName() << endl;
    cout << endl;
}