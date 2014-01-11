/*
 *  DofPass.h
 *
 *  Copyright (c) 2012, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
    /*
     * @see http://blenderartists.org/forum/showthread.php?237488-GLSL-depth-of-field-with-bokeh-v2-4-(update)
     */
    class DofAltPass : public RenderPass
    {
    public:
        DofAltPass(const ofVec2f& aspect, float focalDepth = 1.f, float focalLength = 500.f, float fStop = 3.f, bool showFocus = false);

        void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth);

        float getFocalDepth() const { return focalDepth; }
        void setFocalDepth(float focalDepth) { this->focalDepth = focalDepth; }

        float getFocalLength() const { return focalLength; }
        void setFocalLength(float focalLength) { this->focalLength = focalLength; }

        float getFStop() const { return fStop; }
        void setFStop(float fStop) { this->fStop = fStop; }

        bool getShowFocus() const { return showFocus; }
        void setShowFocus(bool showFocus) { this->showFocus = showFocus; }

    private:
        ofShader shader;
        float focalDepth; //focal distance value in meters, but you may use autofocus option below
        float focalLength; //focal length in mm
        float fStop; //f-stop value
        bool showFocus; //show debug focus point and focal range (red = focal point, green = focal range
    };
}
