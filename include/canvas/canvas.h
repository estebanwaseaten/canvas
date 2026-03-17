#ifndef canvas_h
#define canvas_h

#include <string>
#include <linux/fb.h>   //fb_var_screeninfo and fb_fix_screeninfo

//The Direct Rendering Manager (DRM) is a framework to manage Graphics Processing Units (GPUs).
#include <xf86drm.h>        //drmModeGetResources()
#include <xf86drmMode.h>    //drmModeGetResources()

//#include <EGL/egl.h>
//#include <GLES2/gl2.h>    //OpenGL ES 2.0 - 2 is max version on raspberry pi
#include <canvas/element.h>
#include <canvas/geometry.h>
//class element;

//should not include private headers!
class engineDumb;


//CANVAS is the real interface! ENGINE is shared between the engines...

using namespace std;

class canvas
{
public:
        canvas();
        canvas( bool accel );
        ~canvas();

    void setLineColor( uint32_t rgb );
    void setLineColor( uint8_t r, uint8_t g, uint8_t b );
    void setFillColor( uint32_t rgb );
    void setFillColor( uint8_t r, uint8_t g, uint8_t b );

    void drawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 );
    void drawStripedLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t linepx, uint16_t nolinepx );
    void drawBox( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy );
    void drawFrame( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy );
    void drawPoint( uint16_t x, uint16_t y );

    void insertRegion( uint32_t *region, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy);

    void drawTestbild();

    void clearBuffer();
    void fill( uint8_t r, uint8_t g, uint8_t b );
    void fillRnd();

    void swapBuffer();
    void clearScreen();

    void printInfo();
    void testfunction();
    void printScreenInfo();

    void printPixelBackBuffer( uint32_t width, uint32_t height ); //for testing

    void addElement( element *elem );
    void draw();


    //make private:
    void drawElements();                    //shall not be called from others
private:

    //maybe this needs to go to the elements that are drawn.
    uint32_t    lineColor = 0xffffff;
    uint32_t    fillColor = 0x000000;
    uint32_t    lineWidth = 1;

    //list of elements (what container structure?)
    /*
    array
    vector: auto growing
    list: not continuous in memory
    (derived from those are stack and queue)
    *///
    vector<element*> baseElements;          //vector of base elements
    bool             isDrawing = false;     //avoid infinite loops

    engineDumb      *myEngine = nullptr;      //engine first tries to implement OpenGl, otherwise goes to fb0 mode... MAYBE DROP accelerated support for now...





};





#endif
