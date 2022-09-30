#include <iostream>
#include <cstring>

using ::std::cout;
using ::std::endl;

class StringBad
{
private:
    char * str; // pointer to string
    int len;  // the length of string
    static int num_strings;
public:
    StringBad(const char *s)
    {
        len = strlen(s);
        str = new char[len + 1];
        strcpy(str, s);
        num_strings++;
        cout << num_strings << ": \"" << str << "\" object created" << endl;
    }

    StringBad()
    {
        len = 3;
        str = new char[4];
        strcpy(str, "Cpp");
        num_strings++;
        cout << num_strings << ": \"" << str << "\" default object created" << endl;
    }

    StringBad(const StringBad & sb)
    {
        len = strlen(sb.str);
        str = new char[len + 1];
        strcpy(str, sb.str);
        num_strings++;
    }

    StringBad & operator= (const StringBad sb)
    {
        len = strlen(sb.str);
        delete[] str;  // don't forget it!!!!!
        str = new char[len + 1];
        strcpy(str, sb.str);
        num_strings++;
        return *this;  // don't forget it!!!!!
    }

    ~StringBad()
    {
        cout << "\"" << str << "\" object deleted, ";
        num_strings--;
        cout << num_strings << " left" << endl;
        delete [] str;
    }

    friend std::ostream & operator<< (std::ostream & os, const StringBad & sb)
    {
        os << sb.str;
        return os;
    }
};

int StringBad::num_strings = 0;

int main()
{
    StringBad sb1; // use defalut constructor
    StringBad sb2("Rust");
    StringBad sb3("Csharp");

    cout << sb1 << endl;
    cout << sb2 << endl;
    cout << sb3 << endl;

    cout << "assign sb2 to sb1" << endl;
    sb1 = sb2;
    cout << "sb1: " << sb1 << endl;

    cout << "create new StringBad object sb4 using sb3" << endl;
    StringBad sb4 = sb3;
    cout << "sb4: " << sb4 << endl;

    return 0;
}