#include <iostream>
#include <memory>

using namespace std;

class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
};

unique_ptr<Resource> createResource()
{
    return make_unique<Resource>();
}

int main()
{
    auto ptr = createResource();
    // do whatever
    if(ptr)
        cout << "no null" << endl;

    return 0;
}