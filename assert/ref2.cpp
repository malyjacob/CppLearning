#include <iostream>

using namespace std;

int main()
{
    int rats = 12;
    int &rodents = rats;

    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    cout << "&rats = " << &rats << endl;
    cout << "&rodents = " << &rodents << endl;

    int bunnies = 13;
    rodents = bunnies; // !!!!!!!
    cout << "bunnies = " << bunnies << endl;
    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    cout << "&bunnies = " << &bunnies << endl;
    cout << "&rats = " << &rats << endl;
    cout << "&rodents = " << &rodents << endl;

    return 0;
}