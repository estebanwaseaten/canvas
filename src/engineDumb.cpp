// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include <fcntl.h>      //open() and O_RDWR
#include <sys/ioctl.h>  //ioctl()
#include <sys/mman.h>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>

#include <xf86drm.h>        //drmModeGetResources()
#include <xf86drmMode.h>    //drmModeGetResources()
#include <drm_fourcc.h>     //DRM_FORMAT_XRGB8888

#include "engineDumb.h"
#include "canvasutils.h"
#include "debug.h"

#include <iomanip>
#include <cmath>
#include <algorithm>    //clamp and min and max


using namespace std;

engineDumb::engineDumb()
{
    DEBUG_MSG( "engineDumb::engineDumb()" );

    uint64_t    has_dumb;
    bool        hasDumb;

    // open device
    card = open( "/dev/dri/card0", O_RDWR | O_CLOEXEC );
    cardInfo = drmModeGetResources( card );
    hasDumb = !( (drmGetCap(card, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0) || has_dumb == 0);

    if ( cardInfo != NULL && hasDumb == true )
    {
        DEBUG_MSG( "card0 has resources, dumb buffer and is selected: " << card );
        valid = true;
    }
    else
    {
        DEBUG_MSG( "card0: has no resources or dumb buffer, try card1");
        card = open( "/dev/dri/card1", O_RDWR | O_CLOEXEC );
        cardInfo = drmModeGetResources( card );
        hasDumb = !( (drmGetCap(card, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0) || has_dumb == 0);
        if ( cardInfo != NULL && hasDumb == true)
        {
            DEBUG_MSG( "card1 has resources, dumb buffer and is selected: " << card );
            valid = true;
        }
        else
        {
            DEBUG_MSG( "card1: has no resources or dumb buffer, failure");
            valid = false;
        }
    }

    if ( valid )
    {
        initWithFirstConnectedConnector();
        if( cardConnector == NULL )
        {
            DEBUG_MSG("Could not find connected Connector.");
            valid = false;
        }
        else
        {
            DEBUG_MSG("\tConnected Connector found.");
            utilPrintCardInfo( cardInfo );
            utilPrintConnectorInfo( cardConnector, false );
            DEBUG_MSG("preferred Mode:");
            utilPrintModeInfo( preferredMode );
            utilPrintEncoderInfo( cardEncoder );
            utilPrintCrtcInfo( cardCrtc );
            valid = true;
        }
    }


    //hardware buffers:
    createBuffer( xres, yres, 32, &buffers[0] );    //initial back buffer
    createBuffer( xres, yres, 32, &buffers[1] );    // initial front buffer

    //for testing only
    memset(buffers[0].map, 0, buffers[0].size);       //back buffer
    memset(buffers[1].map, 0xff, buffers[1].size);    //front buffer

    int ret;
    ret = drmModeSetCrtc(card, cardCrtc->crtc_id, buffers[indexFront].dumbBuffer, 0, 0, &cardConnector->connector_id, 1, preferredMode );
    if( ret )
    {
        DEBUG_MSG( "drmModeSetCrtc() failed! returned: " << ret);
        valid = false;
    }
    else
    {
        DEBUG_MSG( "drmModeSetCrtc(), buf_id: " << buffers[indexFront].dumbBuffer );
    }

    srand( time(nullptr) );
}

engineDumb::~engineDumb()
{
    DEBUG_MSG( "engineDumb::~engineDumb()" );

    if (cardInfo != NULL )
        drmModeFreeResources(cardInfo);

    if (cardConnector != NULL )
        drmModeFreeConnector(cardConnector);

    if (cardEncoder != NULL )
        drmModeFreeEncoder(cardEncoder);

    if (cardCrtc != NULL )
        drmModeFreeCrtc(cardCrtc);

}

void engineDumb::initWithFirstConnectedConnector()    //finds first connected connector! also Encoder and Crtc
{
    cardConnector = NULL;
    if( cardInfo == NULL )
    {
        DEBUG_MSG( "findConnector(): cardInfo is NULL" );
        return;
    }

    //loop through all connectors and find the first one that is CONNECTED
    for( int i = 0; i < cardInfo->count_connectors; i++ )
    {
        drmModeConnector *connector = drmModeGetConnector(card, cardInfo->connectors[i]);
        //DEBUG_MSG("connector: " << connector->connection << "(DRM_MODE_TYPE_PREFERRED is " << DRM_MODE_TYPE_PREFERRED << ")");
        if (connector->connection == DRM_MODE_CONNECTED )   //found connected connector
        {
            cardConnector = connector;
            //loop through all modes and find the PREFERRED one.
            for( int j = 0; j < connector->count_modes; j++)
            {
                if( (connector->modes[j].type & DRM_MODE_TYPE_PREFERRED) != 0 )
                {
                    DEBUG_MSG( "DRM_MODE_TYPE_PREFERRED at mode: " << j);
                    preferredMode = &connector->modes[j];
                    xres = preferredMode->hdisplay;
                    yres = preferredMode->vdisplay;
                }
            }

            if( preferredMode == nullptr )
            {
                //fail
                DEBUG_MSG( "failed getting preferred mode");
            }

            cardEncoder = drmModeGetEncoder(card, connector->encoder_id); //gets encoder needed for connector
            if( cardEncoder == nullptr )
            {
                //fail
                DEBUG_MSG( "failed getting card encoder");
            }

            cardCrtc = drmModeGetCrtc(card, cardEncoder->crtc_id);      //gets crtc selected within encoder
            if( cardCrtc == nullptr )
            {
                //fail
                DEBUG_MSG( "failed getting ctrc");
            }

            return;
        }
        else
        {
            drmModeFreeConnector(connector);
        }
    }
    return;
}


void engineDumb::getBackBuffer( void **bb, uint32_t *ppL )
{
    //cout << buffers[indexBack].map32 << endl;
    //cout << buffers[indexBack].map << endl;
    *bb = (void*)buffers[indexBack].map32;
    *ppL = buffers[indexBack].pitch_in_pixels;
}

int engineDumb::createBuffer( uint16_t xres, uint16_t yres, uint8_t depth, bufferPtr buffer )
{
    int ret;
    struct drm_mode_create_dumb create_dumb_request = {
        .height = yres,
		.width  = xres,
		.bpp    = depth,      //16 maybe?
        .flags  = 0,
	  };

    if( ioctl(card, DRM_IOCTL_MODE_CREATE_DUMB, &create_dumb_request) )
    {
      DEBUG_MSG( "Dumb buffer allocation failed!");
      return -1;
    }

    DEBUG_MSG( "Dumb buffer allocation successful!");
    DEBUG_MSG( "\thandle: " << create_dumb_request.handle );
    DEBUG_MSG( "\tpitch: " << create_dumb_request.pitch );
    DEBUG_MSG( "\tsize: " << create_dumb_request.size );

    buffer->handle = create_dumb_request.handle;
    buffer->pitch = create_dumb_request.pitch;
    buffer->size = create_dumb_request.size;                    ///this is the real size
    DEBUG_MSG( "REQUESTED BUFFER WITH SIZE: " << dec << xres << " x " << yres );
    DEBUG_MSG( "CREATED BUFFER WITH SIZE: " << dec << buffer->size );

    buffer->pitch_in_pixels = buffer->pitch / 4;
    buffer->size_in_pixels = buffer->size / 4;
    //buffer->maxIndex32 = buffer->size_in_pixels - 1;

    struct drm_mode_map_dumb map_dumb_request {
        .handle = create_dumb_request.handle,
        .pad = 0,
    };
    ret = drmIoctl(card, DRM_IOCTL_MODE_MAP_DUMB, &map_dumb_request);
    if( ret )
    {
        DEBUG_MSG( "DRM_IOCTL_MODE_MAP_DUMB() failed! returned: " << ret);
        return -1;
    }
    // map_dumb_request now contains .offset for mmap:
    buffer->map = (uint8_t*)mmap(0, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, card, map_dumb_request.offset);
    buffer->map32 = (uint32_t*)buffer->map;
    if( buffer == nullptr )
    {
        DEBUG_MSG( "failed to map memory!");
        return -1;
    }

    ret = drmModeAddFB(card, xres, yres, 24, 32, buffer->pitch, buffer->handle, &buffer->dumbBuffer);
    if( ret )
    {
        DEBUG_MSG( "drmModeAddFB() failed! returned: " << ret);
        return -1;
    }

    DEBUG_MSG( "drmModeAddFB(), buf_id: " << buffer->dumbBuffer );

    buffer->lineCount = buffer->size / buffer->pitch;

    verticalBlank.request.type = DRM_VBLANK_RELATIVE;
    verticalBlank.request.sequence = 1; //next

    return 0;
}


bool engineDumb::isValid()
{
  return valid;
}

void engineDumb::swapBuffer()
{
    uint8_t temp = indexFront;  //swap the indices --> swap the buffers!
    indexFront = indexBack;
    indexBack = temp;

    drmWaitVBlank( card, &verticalBlank );    //not sure if this has an effect at all
//    DEBUG_MSG("swapBuffer(): s,us: " << verticalBlank.reply.tval_sec << "," << verticalBlank.reply.tval_usec);
    int ret = drmModeSetCrtc(card, cardCrtc->crtc_id, buffers[indexFront].dumbBuffer, 0, 0, &cardConnector->connector_id, 1, preferredMode );
    if( ret )
    {
        DEBUG_MSG( "swapBuffer(): drmModeSetCrtc() failed: " << ret );
        valid = false;
    }
}

void engineDumb::clearBackBuffer()
{
    memset(buffers[indexBack].map, 0, buffers[indexBack].size);
    //DEBUG_MSG("clearBuffer()");
}


//https://en.wikipedia.org/wiki/Line_drawing_algorithm
//https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
//https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
//https://www.mat.univie.ac.at/~kriegl/Skripten/CG/node35.html
void engineDumb::drawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color )
{
    //1. special cases:
    //horizontal line
    if ( y1 == y2 )
    {
        if( x1 <= x2 )
        {
            for (size_t i = x1; i <= x2; i++)
            {
                setPixel( i, y1, color );
            }
        }
        else
        {
            for (size_t i = x2; i <= x1; i++)
            {
                setPixel( i, y1, color );
            }
        }
        return;
    }
    //vertical line
    if( x1 == x2 )
    {
        if( y1 <= y2 )
        {
            for (size_t i = y1; i <= y2; i++)
            {
                setPixel( x1, i, color );
            }
        }
        else
        {
            for (size_t i = y2; i <= y1; i++)
            {
                setPixel( x1, i, color );
            }
        }
        return;
    }


    int deltax = x2 - x1;
    int deltay = y2 - y1;
    double slope = (double)deltay / (double)deltax;
    double absSlope = abs(slope);
    //cout << "slope: " << slope << endl;
    //cout << "absSlope: " << absSlope << endl;

    if( absSlope > 1.0 )   // deltay > deltax --> we need to loop through y
    {
        //cout << "absSlope > 1.0" << endl;
        if( y1 > y2 )   //swap points
        {
            int tempy = y2;
            y2 = y1;
            y1 = tempy;
            int tempx = x2;
            x2 = x1;
            x1 = tempx;
        }
        slope = 1/slope;

        //cout << "want: (" << x1 << ", " << y1 << ") & ";
        //cout << "(" << x2 << ", " << y2 << ")" << endl;

        double current_x = (double)x1;
        //cout << "get: (" << round(current_x) << ", " << y1 << ") & ";
        for (size_t i = y1; i <= y2; i++)
        {
            //current_y = i
            setPixel( round(current_x), i, color );
            //cout << "(" << (int)round(current_x) << ", " << i << ")" << endl;
            current_x += slope;
        }
        //cout << "(" << round(current_x-slope) << ", " << y2 << ")" << endl;
        return;
    }

    if( absSlope <= 1.0 )   //deltay <= deltax --> we need to loop through x
    {
        //cout << "absSlope <= 1.0";
        //cout << " slope: " << slope << endl;

        if( x1 > x2 )   //swap points
        {
            int tempy = y2;
            y2 = y1;
            y1 = tempy;
            int tempx = x2;
            x2 = x1;
            x1 = tempx;
        }
        //cout << "want: (" << x1 << ", " << y1 << ") & ";
        //cout << "(" << x2 << ", " << y2 << ")" << endl;

        double current_y = (double)y1;
        //cout << "get: (" << x1 << ", " << round(current_y) << ") & ";
        for (size_t i = x1; i <= x2; i++)
        {
            //current_x = i
            setPixel(  i, round(current_y), color );
            //cout << "(" << i << ", " << (int)round(current_y) << ")" << endl;
            current_y += slope;
        }
        //cout << "(" << x2 << ", " << round(current_y-slope) << ")" << endl;
        return;
    }
}

void engineDumb::drawStripedLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t linepx, uint16_t nolinepx, uint32_t color )
{
    uint32_t lineCounter = 0;
    bool line = true;
    uint32_t nolineCounter = 0;

    if ( y1 == y2 )
    {
        if( x1 <= x2 )
        {
            for (size_t i = x1; i <= x2; i++)
            {
                if( line )
                {
                    setPixel( i, y1, color );
                    lineCounter++;
                    if( lineCounter > linepx )
                    {
                        lineCounter = 0;
                        line = false;
                    }
                }
                else
                {
                    nolineCounter++;
                    if( nolineCounter > nolinepx )
                    {
                        nolineCounter = 0;
                        line = true;
                    }
                }
            }
        }
        else
        {
            for (size_t i = x2; i <= x1; i++)
            {
                if( line )
                {
                    setPixel( i, y1, color );
                    lineCounter++;
                    if( lineCounter > linepx )
                    {
                        lineCounter = 0;
                        line = false;
                    }
                }
                else
                {
                    nolineCounter++;
                    if( nolineCounter > nolinepx )
                    {
                        nolineCounter = 0;
                        line = true;
                    }
                }
            }
        }
        return;
    }
    //vertical line
    if( x1 == x2 )
    {
        if( y1 <= y2 )
        {
            for (size_t i = y1; i <= y2; i++)
            {
                if( line )
                {
                    setPixel( x1, i, color );
                    lineCounter++;
                    if( lineCounter > linepx )
                    {
                        lineCounter = 0;
                        line = false;
                    }
                }
                else
                {
                    nolineCounter++;
                    if( nolineCounter > nolinepx )
                    {
                        nolineCounter = 0;
                        line = true;
                    }
                }
            }
        }
        else
        {
            for (size_t i = y2; i <= y1; i++)
            {
                if( line )
                {
                    setPixel( x1, i, color );
                    lineCounter++;
                    if( lineCounter > linepx )
                    {
                        lineCounter = 0;
                        line = false;
                    }
                }
                else
                {
                    nolineCounter++;
                    if( nolineCounter > nolinepx )
                    {
                        nolineCounter = 0;
                        line = true;
                    }
                }
            }
        }
        return;
    }


    int deltax = x2 - x1;
    int deltay = y2 - y1;
    double slope = (double)deltay / (double)deltax;
    double absSlope = abs(slope);
    //cout << "slope: " << slope << endl;
    //cout << "absSlope: " << absSlope << endl;

    if( absSlope > 1.0 )   // deltay > deltax --> we need to loop through y
    {
        //cout << "absSlope > 1.0" << endl;
        if( y1 > y2 )   //swap points
        {
            int tempy = y2;
            y2 = y1;
            y1 = tempy;
            int tempx = x2;
            x2 = x1;
            x1 = tempx;
        }
        slope = 1/slope;

        //cout << "want: (" << x1 << ", " << y1 << ") & ";
        //cout << "(" << x2 << ", " << y2 << ")" << endl;

        double current_x = (double)x1;
        //cout << "get: (" << round(current_x) << ", " << y1 << ") & ";
        for (size_t i = y1; i <= y2; i++)
        {
            //current_y = i
            if( line )
            {
                setPixel( round(current_x), i, color );
                lineCounter++;
                if( lineCounter > linepx )
                {
                    lineCounter = 0;
                    line = false;
                }
            }
            else
            {
                nolineCounter++;
                if( nolineCounter > nolinepx )
                {
                    nolineCounter = 0;
                    line = true;
                }
            }
            //cout << "(" << (int)round(current_x) << ", " << i << ")" << endl;
            current_x += slope;
        }
        //cout << "(" << round(current_x-slope) << ", " << y2 << ")" << endl;
        return;
    }

    if( absSlope <= 1.0 )   //deltay <= deltax --> we need to loop through x
    {
        //cout << "absSlope <= 1.0";
        //cout << " slope: " << slope << endl;

        if( x1 > x2 )   //swap points
        {
            int tempy = y2;
            y2 = y1;
            y1 = tempy;
            int tempx = x2;
            x2 = x1;
            x1 = tempx;
        }
        //cout << "want: (" << x1 << ", " << y1 << ") & ";
        //cout << "(" << x2 << ", " << y2 << ")" << endl;

        double current_y = (double)y1;
        //cout << "get: (" << x1 << ", " << round(current_y) << ") & ";
        for (size_t i = x1; i <= x2; i++)
        {
            //current_x = i
            if( line )
            {
                setPixel(  i, round(current_y), color );
                lineCounter++;
                if( lineCounter > linepx )
                {
                    lineCounter = 0;
                    line = false;
                }
            }
            else
            {
                nolineCounter++;
                if( nolineCounter > nolinepx )
                {
                    nolineCounter = 0;
                    line = true;
                }
            }
            //cout << "(" << i << ", " << (int)round(current_y) << ")" << endl;
            current_y += slope;
        }
        //cout << "(" << x2 << ", " << round(current_y-slope) << ")" << endl;
        return;
    }
}

void engineDumb::drawFrame( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy, uint32_t color )
{
    drawLine( x1, y1, x1 + dx, y1, color );
    drawLine( x1, y1 + dy, x1 + dx, y1 + dy, color );

    drawLine( x1, y1, x1, y1 + dy, color);
    drawLine( x1 + dx, y1, x1 + dx, y1 + dy, color);
}

void engineDumb::drawBox( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy, uint32_t color )
{
    uint16_t x2 = x1 + dx;
    uint16_t y2 = y1 + dy;
    //check bounds:

    if( (x1 >= buffers[indexBack].pitch_in_pixels) || (x2 >= buffers[indexBack].pitch_in_pixels) || (y1 >= buffers[indexBack].lineCount) || (y2 >= buffers[indexBack].lineCount) )
    {
        //DEBUG_MSG("pixel overflow");
        return;
    }

    //memcpy copies byte by byte --> pixels -> bytes
    uint32_t first_line_offset;

    //draw line by line:
    for( size_t i = 0; i <= dy; i++)
    {
        int current_y = y1+i;

        uint32_t offset_bytes_line_start = current_y * buffers[indexBack].pitch + 4*x1; //4bytes per pixel

        if ( i == 0 )   //first line
        {
            drawLine(x1, y1, x1 + dx, y1, color );
            first_line_offset = offset_bytes_line_start;
        }
        else
        {
            //copy first line
            memcpy( (void*)&buffers[indexBack].map[offset_bytes_line_start], (void*)&buffers[indexBack].map[first_line_offset], (dx+1)*4 );
        }
    }
}

//not sure if this works
void engineDumb::insertRegion( uint32_t *region, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy)
{
    int pixelsPerLine = buffers[indexBack].pitch_in_pixels;
    int bytesPerLine = buffers[indexBack].pitch;
    uint8_t *regionInBytes = (uint8_t *)region;

    if( (x + dx >= buffers[indexBack].pitch_in_pixels) || (y + dy >= buffers[indexBack].lineCount) )
    {
        DEBUG_MSG("pixel overflow");
        return;
    }

    //memcpy line by line:  //void* memcpy( void* dest, const void* src, std::size_t count );
    for (uint32_t i = 0; i < dy; i++)
    {
        //memcpy( (void*)&buffers[indexBack].map[buffers[indexBack].pitch*i], regionInBytes, 4 * dx );
    }
}


void engineDumb::setPixel( uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b )
{
    uint32_t    color = (r << 16) | (g << 8) | b;
    setPixel( x, y, color);
}

void engineDumb::setPixel( uint32_t x, uint32_t y, uint32_t color )             //safe function (no buffer overflow possible --> overhead)
{
    uint32_t offset32 = buffers[indexBack].pitch_in_pixels * y + x;    //offset in pixels

    if ( offset32 < buffers[indexBack].size_in_pixels )    //size is in bytes
    {
          buffers[indexBack].map32[offset32] = color; //works
          //DEBUG_MSG("engineDumb::setPixel() " << x << ", " << y << ", " << color);
          //DEBUG_MSG("engineDumb::setPixel() " << buffers[indexBack].map32[offset32] );
    }
    else
    {
        //DEBUG_MSG("pixel overflow");
    }
}



void engineDumb::testfunction()
{
    //DEBUG_MSG("engineDumb::testfunction()");
    setPixel( 1, 1, 255 );    //does not work
    setPixel( 2, 2, 255 );
    setPixel( 3, 3, 255 );
    setPixel( 3, 1, 255 );
    setPixel( 1, 3, 255 );

    /*if ( swapper%2 == 0 )
    {
        for (size_t i = 0; i < xres; i++)
        {
            for (size_t j = 0; j < yres; j++)
            {
                setPixel( i, j, 255, 0, 0 );
            }
        }
    }
    else
    {
        for (size_t i = 0; i < xres; i++)
        {
            for (size_t j = 0; j < yres; j++)
            {
                setPixel( i, j, 0, 255, 0 );
            }
        }
    }
    swapper++;
    swapBuffer();*/
}

// fill one horizontal line and then copy that into all the other lines...
void engineDumb::fill( uint8_t r, uint8_t g, uint8_t b )
{
    uint32_t color = (r << 16) | (g << 8) | b;

    //first line:
    for (uint32_t i = 0; i < buffers[indexBack].pitch_in_pixels; i++)
    {
        buffers[indexBack].map32[i] = color;// red: 0x00FF0000; gren: 0x0000FF00; blue 0x000000FF
    }

    //all the other lines memcpied over:
    for (uint32_t i = 1; i < buffers[indexBack].lineCount; i++)
    {
        memcpy( (void*)&buffers[indexBack].map[i*buffers[indexBack].pitch], (void*)buffers[indexBack].map, buffers[indexBack].pitch );
    }
}

void engineDumb::fillRnd()
{
    float norm = 255.0/RAND_MAX;

    if( valid )
    {
        uint8_t r = (uint8_t)(rand()*norm);
        uint8_t g = (uint8_t)(rand()*norm);
        uint8_t b = (uint8_t)(rand()*norm);

        fill( r, g, b );
    }
}

void engineDumb::printInfo()
{

}
