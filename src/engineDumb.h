// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

#ifndef engineDumb_h
#define engineDumb_h

#include <linux/fb.h>   //fb_var_screeninfo and fb_fix_screeninfo

typedef struct buffer {
    uint32_t    handle;
    uint32_t    pitch;                      //one line bytes offset
    uint32_t    lineCount;                  //lines
    uint64_t    size;                       //in bytes

    uint32_t    pitch_in_pixels;            //one line pixel count offset
    uint32_t    size_in_pixels;             //total pixel count
    //uint32_t    maxIndex32;

    uint8_t     *map = nullptr;             //bytes
    uint32_t    *map32 = nullptr;           //pixels

    uint32_t    dumbBuffer;                 //id of the dumb buffer (linux)
} buffer, *bufferPtr;

class engineDumb
{
public:
      engineDumb();
      ~engineDumb();

      bool isValid() ;

      void setPixel( uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b ) ;
      void setPixel( uint32_t x, uint32_t y, uint32_t color ) ;
      void fillRnd() ;

      void clearBackBuffer() ;      //directly without swap
      void fill( uint8_t r, uint8_t g, uint8_t b ) ;

      void drawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color );
      void drawStripedLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t linepx, uint16_t nolinepx, uint32_t color );
      void drawBox( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy, uint32_t color );
      void drawFrame( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy, uint32_t color );

      void insertRegion( uint32_t *region, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy);


      void swapBuffer() ;
      void printInfo() ;



      void testfunction() ;
      void getBackBuffer( void **bb, uint32_t *ppL ) ;

private:

    int         createBuffer( uint16_t xres, uint16_t yres, uint8_t depth, bufferPtr buffer );
    void        initWithFirstConnectedConnector();

    drmVBlank   verticalBlank;

    buffer      buffers[2];
    uint8_t     indexFront  = 1;        // frontBuffer is displayed.
    uint8_t     indexBack  = 0;         // all actual drawing occurs into back buffer ()


    uint32_t    swapper = 0;

    bool                valid = false;
    int                 card = -1;                // file descriptor (often called fd)
//    int                 rendererInUse = -1;       // set to the used renderer...

    drmModeResPtr       cardInfo = nullptr;
    drmModeConnectorPtr cardConnector = nullptr;            //first connected connector
    drmModeModeInfoPtr  preferredMode = nullptr;            //preferred mode
    uint16_t            xres = 0;
    uint16_t            yres = 0;                         //from inside preferredMode

    //uint32_t            maxIndex32 = 0;
    //uint32_t            maxIndex8 = 0;


    drmModeEncoderPtr   cardEncoder = nullptr;
    drmModeCrtcPtr      cardCrtc = nullptr;




};





#endif
