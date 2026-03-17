// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #ifndef debug_h
#define debug_h



//#undefine DEBUG
#define DEBUG
//#define QUIET

#ifdef DEBUG

#include <iostream>
  #ifdef QUIET
    #include <fstream>
    #define DEBUG_MSG(str) { std::ofstream debugfile;\
                                debugfile.open("debug", std::ios::app);\
                                debugfile << str << std::endl;\
                                debugfile.close(); }
  #else
    #define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
  #endif
#else
  #define DEBUG_MSG(str) do { } while ( false )
#endif

#endif
