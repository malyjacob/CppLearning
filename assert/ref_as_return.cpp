#include <iostream>

using std::cout;
using std::endl;

struct Time
{
public:
    int hour = 0;
    int min = 0;
    int second = 0;
    Time(int h = 0, int m = 0, int s = 0)
        : hour(h), min(m), second(s) {}
    friend void display(const Time &t)
    {
        cout << t.hour << "h : ";
        cout << t.min << "m : ";
        cout << t.second << "s;" << endl;
    }
    friend Time &plus(Time &des, const Time &src)
    {
        int ih = 0;
        int im = 0;

        int s = des.second + src.second;
        if (s < 60)
        {
            des.second = s;
        }
        else
        {
            des.second = s % 60;
            im = 1;
        }

        int m = des.min + src.min + im;
        if (m < 60)
        {
            des.min = m;
        }
        else
        {
            des.min = m % 60;
            ih = 1;
        }

        des.hour += ih + src.hour;
        return des;
    }
};

int main()
{
    Time t1(1, 23, 54);
    Time t2(2, 53, 12);

    display(t1);
    display(t2);

    /*  equal to:
     *  plus(t1, t2);
     *  display(t1);
     */
    display(plus(t1, t2));

    plus(t1, t2);
    display(t1);
}