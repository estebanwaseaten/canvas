// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 
#ifndef canvasutils_h
#define canvasutils_h


#include <xf86drm.h>        //drmModeGetResources()
#include <xf86drmMode.h>    //drmModeGetResources()
#include <linux/fb.h>   //fb_var_screeninfo and fb_fix_screeninfo


void utilPrintCardInfo( drmModeResPtr cardInfo );
void utilPrintConnectorInfo( drmModeConnectorPtr connector, bool verbose );
void utilPrintEncoderInfo( drmModeEncoderPtr encoder );
void utilPrintCrtcInfo( drmModeCrtcPtr crtc );
void utilPrintModeInfo( drmModeModeInfoPtr  mode );

void utilPrintScreenInfo( fb_fix_screeninfo *fix_screen_info, fb_var_screeninfo *var_screen_info );









#endif
