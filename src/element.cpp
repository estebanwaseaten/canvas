// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include <canvas/canvas.h>
#include <canvas/element.h>

#include "debug.h"

element::element()
{

}

element::~element()
{
    //delete all children
}

void element::drawRecursive()
{
    if ( myCanvas == nullptr )      //drawing methods do not *need* to use myCanvas, but they can rely on myCanvas being valid
    {
        DEBUG_MSG( "element::drawRecursive() myCanvas is invalid");
        return;
    }

    //PARENTS FIRST drawing:
    this->draw();               //1. draw itself --> derived class
    for( auto iterator : childElements)     //2. loop through children and draw recursively:
    {
        iterator->drawRecursive();
    }
}

void element::addChild( element *child )
{
    //if this element already has a canvas set, inherit also to the child
    if( myCanvas != nullptr )
    {
        child->setCanvas( myCanvas );
    }
    childElements.push_back( child );
}

void element::setCanvas( canvas *newCanvas)
{
    //also set canvas for all child elemens
    myCanvas = newCanvas;
    for( auto iterator : childElements)     //2. loop through children and draw recursively:
    {
        iterator->setCanvas( newCanvas );
    }
}
