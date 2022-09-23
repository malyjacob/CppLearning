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

    shared_ptr<Resource> res1(res);
    {
        shared_ptr<Resource> res2(res);
        cout << "Killing one shanred ptr" << endl;
    }
    return 0;
}