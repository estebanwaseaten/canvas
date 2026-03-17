// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include "canvasutils.h"
#include "debug.h"


void utilPrintCardInfo( drmModeResPtr cardInfo )
{
    if (cardInfo == NULL)
    {
        DEBUG_MSG( "printCardInfo Error: cardInfo is NULL");
        return;
    }
    DEBUG_MSG( "CardInfo:" );
    DEBUG_MSG( "\tcount_fbs: " << cardInfo->count_fbs );
    DEBUG_MSG( "\tcount_crtcs: " << cardInfo->count_crtcs );
    DEBUG_MSG( "\tcount_connectors: " << cardInfo->count_connectors );
    DEBUG_MSG( "\tcount_encoders: " << cardInfo->count_encoders );
    DEBUG_MSG( "\twidth (min/max): " << cardInfo->min_width << "/" << cardInfo->max_width );
    DEBUG_MSG( "\theight (min/max): " << cardInfo->min_height << "/" << cardInfo->max_height );
}

void utilPrintModeInfo( drmModeModeInfoPtr  mode )
{
    if ( mode == nullptr )
    {
        DEBUG_MSG( "utilPrintModeInfo(): mode is NULL" );
        return;
    }

    DEBUG_MSG( "Mode Info: ");
    DEBUG_MSG( "\tdisplay h/v: " << mode->hdisplay << "/" << mode->vdisplay );
    DEBUG_MSG( "\tvrefresh: " << mode->vrefresh );
    DEBUG_MSG( "\ttype: " << mode->type );
    DEBUG_MSG( "\tname: " << mode->name );
}

void utilPrintConnectorInfo( drmModeConnector *connector, bool verbose )
{
    if ( connector == NULL )
    {
        DEBUG_MSG( "utilPrintConInfo(): connector is NULL" );
        return;
    }

    DEBUG_MSG( "Connected Connector Info: ");
    DEBUG_MSG( "\tconnector_id: " << connector->connector_id );
    DEBUG_MSG( "\tencoder_id: " << connector->encoder_id );
    DEBUG_MSG( "\tconnector_type: " << connector->connector_type );
    DEBUG_MSG( "\tconnector_type_id: " << connector->connector_type_id );
    switch (connector->connection )
    {
        case DRM_MODE_CONNECTED:
            DEBUG_MSG( "\tconnection: DRM_MODE_CONNECTED" );
            break;
        case DRM_MODE_DISCONNECTED:
            DEBUG_MSG( "\tconnection: DRM_MODE_DISCONNECTED" );
            break;
        case DRM_MODE_UNKNOWNCONNECTION:
        default:
            DEBUG_MSG( "\tconnection: DRM_MODE_UNKNOWNCONNECTION" );
    }

    DEBUG_MSG( "\tdimensions (mm): " << connector->mmWidth << "x" << connector->mmHeight );

    switch (connector->subpixel)
    {
        case DRM_MODE_SUBPIXEL_UNKNOWN:
            DEBUG_MSG( "\tsubpixel: DRM_MODE_SUBPIXEL_UNKNOWN" );
            break;
        case DRM_MODE_SUBPIXEL_HORIZONTAL_RGB:
            DEBUG_MSG( "\tsubpixel: DRM_MODE_SUBPIXEL_HORIZONTAL_RGB" );
            break;
        case DRM_MODE_SUBPIXEL_HORIZONTAL_BGR:
            DEBUG_MSG( "\tsubpixel: DRM_MODE_SUBPIXEL_HORIZONTAL_BGR" );
            break;
        case DRM_MODE_SUBPIXEL_VERTICAL_RGB:
            DEBUG_MSG( "\tsubpixel: DRM_MODE_SUBPIXEL_VERTICAL_RGB" );
            break;
        case DRM_MODE_SUBPIXEL_VERTICAL_BGR:
            DEBUG_MSG( "\tsubpixel: DRM_MODE_SUBPIXEL_VERTICAL_BGR" );
            break;
        case DRM_MODE_SUBPIXEL_NONE:
            DEBUG_MSG( "\tsubpixel: DRM_MODE_SUBPIXEL_NONE" );
            break;
    }

    DEBUG_MSG( "\tcount_modes: " << connector->count_modes );
    if( verbose )
    {
        for( int i = 0; i < connector->count_modes; i++ )
        {
            DEBUG_MSG( "\t\tmode " << i << " name:" << connector->modes[i].name << " type: " << connector->modes[i].type);
        //    DEBUG_MSG( "mode " << i << " type:" << connector->modes[i].type );
        //    DEBUG_MSG( "mode " << i << " clock:" << connector->modes[i].clock );
            DEBUG_MSG( "\t\t "  << connector->modes[i].hdisplay << "x" << connector->modes[i].vdisplay );
        }
    }
    DEBUG_MSG( "\tcount_props: " << connector->count_props );
    if( verbose )
    {
        for( int i = 0; i < connector->count_props; i++ )
        {
            DEBUG_MSG( "\t\tprop " << i << " id: " << connector->props[i] << " value: " << connector->prop_values[i] );
        }
    }
    DEBUG_MSG( "\tcount_encoders: " << connector->count_encoders );
    if ( verbose )
    {
        for( int i = 0; i < connector->count_encoders; i++ )
        {
            DEBUG_MSG( "\t\tencoder " << i << " id:" << connector->encoders[i] );
        }
    }
}

void utilPrintEncoderInfo( drmModeEncoderPtr encoder )
{
    if( encoder == NULL )
    {
        DEBUG_MSG( "utilPrintEncoderInfo(): encoder is NULL");
        return;
    }

    DEBUG_MSG( "Encoder Info (id/type: " << encoder->encoder_id << "/" << encoder->encoder_type << ")" );
    DEBUG_MSG( "\tcrtc_id: " << encoder->crtc_id );
    DEBUG_MSG( "\tpossible crtcs/clones: " << encoder->possible_crtcs << "/" << encoder->possible_clones );

}

void utilPrintCrtcInfo( drmModeCrtcPtr crtc )
{
    if( crtc == NULL )
    {
        DEBUG_MSG( "utilPrintCrtcInfo(): crtc is NULL");
        return;
    }

    DEBUG_MSG( "CRTC Info (crtc_id: " << crtc->crtc_id << ")");
    DEBUG_MSG( "\tbuffer_id: " << crtc->buffer_id );
    DEBUG_MSG( "\tframebuffer pos x/y: " << crtc->x << "/" << crtc->y );
    DEBUG_MSG( "\twidth/height: " << crtc->width << "/" << crtc->height );
    DEBUG_MSG( "\tmode name: " << crtc->mode.name);

}
