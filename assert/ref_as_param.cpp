#include <iostream>

// use ref param
void swapr(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

//use pointer
void swapp(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// use value ------- Wrong!!!
void swapv(int a, int b)
{
    int temp = a;
    a = b;
    b = temp;
}

int main()
{
    using namespace std;
    int value_1 = 300;
    int value_2 = 500;
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    cout << "using ref to swap...." << endl;
    swapr(value_1, value_2);
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    cout << "using ptr to swap....." << endl;
    swapp(&value_1, &value_2);
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    cout << "using value to swap" << endl;
    swapv(value_1, value_2);
    cout << "value_1 = " << value_1 << endl;
    cout << "value_2 = " << value_2 << endl;
    cout << endl;

    return 0;
}