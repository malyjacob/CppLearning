#include "class.hpp"

int main()
{
    cout << "begin" << endl;
    Circle c1, c2;

    c1.show();
    c2.show();

    cout << "c1's rand: " << c1.get_rand() << endl;
    cout << "c2's rand: " << c2.get_rand() << endl;

    cout << "scalar...." << endl;
    c1.scalar(1.5);
    c2.scalar(3);

    cout << "c1's rand: " << c1.get_rand() << endl;
    cout << "c2's rand: " << c2.get_rand() << endl;

    cout << "c1's square: " << c1.square() << endl;
    cout << "c2's square: " << c2.square() << endl;

    return 0;
}