#include <memory>
#include <iostream>

using namespace std;

void observe(weak_ptr<int> weak)
{
    if(auto obs = weak.lock())
        cout << "observe() able to lock weak_ptr, value = " << *obs << endl;
    else
        cout << "observe() unable to lock weak_ptr" << endl;
}

int main()
{
    weak_ptr<int> weak;
    cout << "weak_ptr not yet init" << endl;
    observe(weak);

    {
        auto shared = make_shared<int>(12);
        weak = shared;
        cout << "weak_ptr inited with shared_ptr" << endl;
        observe(weak);
    }

    cout << "shared_ptr has been destructed due to out of scope" << endl;
    observe(weak);
}