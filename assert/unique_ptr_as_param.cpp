#include <iostream>
#include <memory>
#include <utility> // for std::move
using namespace std;

class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destoryed\n"; }
    friend ostream& operator<<(ostream& os, const Resource& res)
    {
        os << "I am Resource";
        return os;
    }
};

void takeOwnership(unique_ptr<Resource> res)
{
    if(res)
        cout << *res << endl;
}

int main()
{
    auto ptr = make_unique<Resource>();

    // takeOwnership(ptr); no ok!
    takeOwnership(move(ptr));

    takeOwnership(make_unique<Resource>()); // ok. use the rvalue operator= of unique_ptr;

    cout << "End" << endl;

    return 0;
}