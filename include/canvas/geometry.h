#ifndef canvas_geometry_h
#define canvas_geometry_h

//2D pixel geometry!
#include <iostream> //temporariliy

using namespace std;

class point
{
private:
    float px, py;

public:
    point();
    point( float x, float y );
    ~point(){}

    point operator+( point const& p )       //defined as member fctn. If not --> two parameters!
    {
        return point(  px + p.px, py + p.py);
    }

    point operator-( point const& p )       //defined as member fctn. If not --> two parameters!
    {
        return point(  px - p.px, py - p.py);
    }

    point operator*( float const& scale )       //defined as member fctn. If not --> two parameters!
    {
        return point(  px*scale, py*scale);
    }

    friend ostream& operator<<( ostream& os, const point& pt )      //& --> pass a reference (like a pointer but different)
    {
        os << "(" << pt.px << ", " << pt.py << ")";
        return os;
    }
};


class rect
{
public:
    rect( float tlx, float tly, float brx, float bry );
    ~rect(){}



    friend ostream& operator<<( ostream& os, const rect& pt )      //& --> pass a reference (like a pointer but different)
    {
        //os << "(" << pt.px << ", " << pt.py << ")";
        return os;
    }

private:
    point topleft;
    point bottomright;
};












#endif
