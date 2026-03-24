// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include <canvas/canvas.h>

#include "debug.h"
#include "canvasutils.h"

#include <engineDumb.h>

canvas::canvas()
{
    myEngine = new engineDumb();
    if( !myEngine->isValid() )
    {
        
    }
}


canvas::~canvas()
{
    if( myEngine != nullptr )
    {
        delete myEngine;
    }
}

void canvas::draw()
{
    if ( isDrawing )
    {
        //DEBUG_MSG( "already drawing!!");
        return;
    }

    clearBuffer();

    isDrawing = true;
    drawElements();
    isDrawing = false;

    swapBuffer();

}


void canvas::drawElements()         //private to avoid infinite loops
{
    for( auto iterator : baseElements )     //1. loop through all available base elements and draw them (recursively with their child elements)
    {
        iterator->drawRecursive();
    }
}

void canvas::addElement( element *elem )
{
    elem->setCanvas(this);      //should also set this for all other elements
    baseElements.push_back( elem );
}

void canvas::setLineColor( uint32_t rgb )
{
    lineColor = rgb;
}
void canvas::setLineColor( uint8_t r, uint8_t g, uint8_t b )
{
    lineColor = (r << 16) | (g << 8) | b;
}

void canvas::setFillColor( uint32_t rgb )
{
    fillColor = rgb;
}
void canvas::setFillColor( uint8_t r, uint8_t g, uint8_t b )
{
    fillColor = (r << 16) | (g << 8) | b;
}

void canvas::drawTestbild()
{
    myEngine->drawLine(  100, 0, 100, 200, 0xffffff );
    myEngine->drawLine(  0, 100, 200, 100, 0xffffff  );

    myEngine->drawLine(  100, 100, 30, 0, 0xff55ff  );
    myEngine->drawLine(  100, 100, 170, 0, 0xff55ff  );
    myEngine->drawLine(  100, 100, 30, 200, 0xff55ff  );
    myEngine->drawLine(  100, 100, 170, 200, 0xff55ff  );

    myEngine->drawLine(  100, 100, 0, 30, 0xffff55  );
    myEngine->drawLine(  100, 100, 0, 170, 0xffff55  );
    myEngine->drawLine(  100, 100, 200, 30, 0xffff55  );
    myEngine->drawLine(  100, 100, 200, 170, 0xffff55  );

    myEngine->drawLine(  100, 100, 0, 0, 0x55ffff  );
    myEngine->drawLine(  100, 100, 0, 200, 0x55ffff  );
    myEngine->drawLine(  100, 100, 200, 0, 0x55ffff  );
    myEngine->drawLine(  100, 100, 200, 200, 0x55ffff  );
}

void canvas::drawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 )
{
    myEngine->drawLine( x1, y1, x2, y2, lineColor );
}

void canvas::drawStripedLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t linepx, uint16_t nolinepx )
{
    myEngine->drawStripedLine( x1, y1, x2, y2, linepx, nolinepx, lineColor );
}

void canvas::drawBox( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy )
{
    myEngine->drawBox( x1, y1, dx, dy, fillColor );
}

void canvas::drawFrame( uint16_t x1, uint16_t y1, uint16_t dx, uint16_t dy )
{
    myEngine->drawFrame( x1, y1, dx, dy, lineColor );
}


void canvas::drawPoint( uint16_t x, uint16_t y )
{
    myEngine->setPixel(x, y, lineColor);
}

void canvas::insertRegion( uint32_t *region, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy)
{
    myEngine->insertRegion( region, x, y, dx, dy);
}


void canvas::printInfo()
{
    myEngine->printInfo();
}

void canvas::swapBuffer()
{
    myEngine->swapBuffer();
}

void canvas::fill( uint8_t r, uint8_t g, uint8_t b )
{
    myEngine->fill(r,g,b);
}

void canvas::fillRnd()
{
  if ( myEngine != nullptr )
  {
      myEngine->fillRnd();
  }
}

void canvas::clearBuffer()
{
    if ( myEngine != nullptr )
    {
        myEngine->clearBackBuffer();
    }
}

void canvas::clearScreen()
{
  if ( myEngine != nullptr )
  {
      myEngine->clearBackBuffer();
      myEngine->swapBuffer();
  }
}



//for testing and console drawing
void canvas::printPixelBackBuffer( uint32_t width, uint32_t height )
{
    DEBUG_MSG("canvas::printPixelBackBuffer()");
    if ( myEngine == nullptr )
      return;

    uint32_t *buffer;
    uint32_t line_width;
    myEngine->getBackBuffer( (void**)&buffer, &line_width);

    uint32_t offset;

    for (size_t i = 0; i <= height; i++)       //y
    {
        offset = i*line_width;
        for (size_t j = 0; j <= width; j++)  //x
        {
            //cout << buffer[offset + j];
            if ( buffer[offset + j] == 0 )
              cout << " .";
            else
              cout << " #";
        }
        cout << endl;
    }

}


void canvas::testfunction()
{
    //DEBUG_MSG("canvas::testfunction()");
    //myEngine->testfunction();
    //                  x1 y1 x2 y2 c
  //  myEngine->drawLine( 1, 1, 6, 3, 1 );
  //  myEngine->drawLine( 2, 12, 4, 5, 1 );

   myEngine->drawLine( 5, 5, 9, 5, 1 );
   myEngine->drawLine( 5, 5, 5, 9, 1 );

   myEngine->drawLine( 5, 5, 9, 4, 1 );
   myEngine->drawLine( 5, 5, 9, 6, 1 );
   myEngine->drawLine( 5, 5, 1, 4, 1 );
   myEngine->drawLine( 5, 5, 1, 6, 1 );
}
