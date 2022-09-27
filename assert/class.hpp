#ifndef CLASS_H_
#define CLASS_H_

#include <iostream>
#include <string>

#define PI 3.14

using ::std::string;
using ::std::cout;
using ::std::endl;

class Circle
{
private:
    double rand_;
    string color_;
    double x_index_;
    double y_index_;
public:
    Circle(const string &color = "red", double rand = 1, double x_index = 0, double y_index = 0);
    ~Circle() { cout << "free the circle: " << color_ << endl; }
    void init(const string &color, double rand, double x_index, double y_index);
    void change_color(const string &color);
    void move_to(double x_index, double y_index);
    void scalar(double d);
    void show();
    double square();
    const string &get_color() { return color_; }
    double get_rand() { return rand_; }
    double get_x_index() { return x_index_; }
    double get_y_index() { return y_index_; }
};

Circle::Circle(const string &color, double rand, double x_index, double y_index)
{
    if(rand_ < 0)
        abort();
    rand_ = rand;
    color_ = color;
    x_index_ = x_index;
    y_index_ = y_index;
}

void Circle::init(const string &color, double rand, double x_index, double y_index)
{
    if(rand_ < 0)
        abort();
    rand_ = rand;
    color_ = color;
    x_index_ = x_index;
    y_index_ = y_index;
}

void Circle::change_color(const string &color)
{
    color_ = color;
}

void Circle::move_to(double x_index, double y_index)
{
    x_index_ = x_index;
    y_index_ = y_index;
}

void Circle::scalar(double scale)
{
    if(scale <= 0)
        return;
    rand_ *= scale;
}

void Circle::show()
{
    cout << "rand: " << rand_ << endl;
    cout << "color: " << color_ << endl;
    cout << "x_index: " << x_index_ << endl;
    cout << "y_index: " << y_index_ << endl;
    cout << endl;
}

double Circle::square()
{
    return PI * rand_ * rand_;
}
#endif