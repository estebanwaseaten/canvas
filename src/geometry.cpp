// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */


 #include <canvas/canvas.h>
#include <canvas/geometry.h>

#include "debug.h"

point::point()
{
    px = 0;
    py = 0;
}

point::point( float x, float y )
{
    px = x;
    py = y;
}
