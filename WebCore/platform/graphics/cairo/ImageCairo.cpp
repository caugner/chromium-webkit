/*
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "BitmapImage.h"

#if PLATFORM(CAIRO)

#include "AffineTransform.h"
#include "FloatRect.h"
#include "GraphicsContext.h"
#include <cairo.h>
#include <math.h>

namespace WebCore {

void FrameData::clear()
{
    if (m_frame) {
        cairo_surface_destroy(m_frame);
        m_frame = 0;
        m_duration = 0.;
        m_hasAlpha = true;
    }
}

// ================================================
// Image Class
// ================================================

// Drawing Routines

void BitmapImage::draw(GraphicsContext* ctxt, const FloatRect& dst, const FloatRect& src, CompositeOperator op)
{
    cairo_t* context = ctxt->platformContext();

    if (!m_source.initialized())
        return;
    
    cairo_surface_t* image = frameAtIndex(m_currentFrame);
    if (!image) // If it's too early we won't have an image yet.
        return;

    IntSize selfSize = size();                       
    FloatRect srcRect(src);
    FloatRect dstRect(dst);

    cairo_save(context);

    // Set the compositing operation.
    if (op == CompositeSourceOver && !frameHasAlphaAtIndex(m_currentFrame))
        ctxt->setCompositeOperation(CompositeCopy);
    else
        ctxt->setCompositeOperation(op);

    // If we're drawing a sub portion of the image or scaling then create
    // a pattern transformation on the image and draw the transformed pattern.
    // Test using example site at http://www.meyerweb.com/eric/css/edge/complexspiral/demo.html
    cairo_pattern_t* pattern = cairo_pattern_create_for_surface(image);
    float scaleX = srcRect.width() / dstRect.width();
    float scaleY = srcRect.height() / dstRect.height();
    cairo_matrix_t mat = { scaleX,  0, 0 , scaleY, srcRect.x(), srcRect.y() };
    cairo_pattern_set_matrix(pattern, &mat);

    // Draw the image.
    cairo_translate(context, dstRect.x(), dstRect.y());
    cairo_set_source(context, pattern);
    cairo_rectangle(context, 0, 0, dstRect.width(), dstRect.height());
    cairo_fill(context);

    cairo_pattern_destroy(pattern);
    cairo_restore(context);

    startAnimation();

}

void Image::drawPattern(GraphicsContext* ctxt, const FloatRect& tileRect, const AffineTransform& patternTransform,
                        const FloatPoint& phase, CompositeOperator op, const FloatRect& destRect)
{
    cairo_surface_t* image = nativeImageForCurrentFrame();
    if (!image) // If it's too early we won't have an image yet.
        return;

    cairo_t* context = ctxt->platformContext();
    ctxt->save();

    // TODO: Make use of tileRect.

    cairo_pattern_t* pattern = cairo_pattern_create_for_surface(image);
    cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

    cairo_matrix_t pattern_matrix = cairo_matrix_t(patternTransform);
    cairo_matrix_t phase_matrix = {1, 0, 0, 1, phase.x(), phase.y()};
    cairo_matrix_t combined;
    cairo_matrix_multiply(&combined, &pattern_matrix, &phase_matrix);
    cairo_matrix_invert(&combined);
    cairo_pattern_set_matrix(pattern, &combined);

    ctxt->setCompositeOperation(op);
    cairo_set_source(context, pattern);
    cairo_rectangle(context, destRect.x(), destRect.y(), destRect.width(), destRect.height());
    cairo_fill(context);

    cairo_pattern_destroy(pattern);
    ctxt->restore();
}

void BitmapImage::checkForSolidColor()
{
    // FIXME: It's easy to implement this optimization. Just need to check the RGBA32 buffer to see if it is 1x1.
    m_isSolidColor = false;
}

}

#endif // PLATFORM(CAIRO)
