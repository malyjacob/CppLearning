#include <iostream>
#include <memory>

using namespace std;

class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
};

int main()
{
    auto ptr1 = make_shared<Resource>();
    {
        auto ptr2 = ptr1; // using copy constructor
        cout << "Killing one shared ptr" << endl;
    }
    cout << "Killing another shared ptr" << endl;

    return 0;
}