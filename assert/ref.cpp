#include <iostream>

using namespace std;

int main()
{
    int rats = 101;
    int& rodents = rats; // rodents is a reference to rats.

    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    rodents++; // !!!!
    cout << "Attention!!!!" << endl;

    cout << "rats = " << rats << endl;
    cout << "rodents = " << rodents << endl;

    cout << "look the adress" << endl;
    cout << "&rats = " << &rats << endl;
    cout << "&rodents = " << &rodents << endl;

    return 0;
}