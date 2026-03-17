#ifndef canvas_element_h
#define canvas_element_h

#include <vector>
#include <iostream> //temporariliy

//base class for elements that are drawn on a canvas.
class canvas;

class element           //elements are drawable entitites of the canvas. can be subclassed to be anything. need to provide a draw function
{
public:

    element();
    virtual ~element();      //needs to be virtual so c++ does not complain. is still called!

    virtual void update(){}                                     //is this really a part of a canvas element?
    virtual void update( uint32_t increment ){}                 //is this really a part of a canvas element?

    virtual void drawRecursive() final;       //draws
    virtual void addChild( element *child ) final;
    virtual void setCanvas( canvas *newCanvas) final;

protected:
    virtual void draw(){};            //needs to be overriden, otherwise does nothing!    is protected such that only drawRecursive can call it
    canvas *myCanvas = nullptr;

private:
    std::vector<element*> childElements;


};


#endif
