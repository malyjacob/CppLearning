#include <iostream>
#include <memory>
#include <cstring>

using namespace std;

class Father;
class Son;

class Father
{
public:
    char name[16] { 0 };
    shared_ptr<Son> son {};
    Father(const char *nm)
    {
        strncpy(name, nm, 16);
        cout << "created Father" << endl;
    }
    ~Father() { cout << "destoryed Father" << endl; }
};

class Son
{
public:
    char name[16] { 0 };
    shared_ptr<Father> father {};
    Son(const char *nm)
    {
        strncpy(name, nm, 16);
        cout << "created Son" << endl;
    }
    ~Son() { cout << "destoryed Son" << endl; }
};

int main()
{
    auto f = make_shared<Father>("father");
    auto s = make_shared<Son>("son");

    cout << "f's ref count: " << f.use_count() << endl;
    cout << "s's ref count: " << s.use_count() << endl;

    cout << "linked..." << endl;
    f->son = s;
    s->father = f;

    cout << "f's ref count: " << f.use_count() << endl;
    cout << "s's ref count: " << s.use_count() << endl;
    
    return 0;
}