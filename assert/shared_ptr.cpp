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
    Resource *res = new Resource();

    shared_ptr<Resource> ptr1(res);
    {
        shared_ptr<Resource> ptr2 = ptr1;
        cout << "Killing one shared ptr" << endl;
    }
    cout << "Killing another shared ptr" << endl;

    return 0;
}