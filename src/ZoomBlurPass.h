/*
 *  ZoomBlurPass.h
 *
 *  Copyright (c) 2013, satcy, http://satcy.net
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

namespace nm
{
    class ZoomBlurPass : public RenderPass
    {
    public:
        
        typedef shared_ptr<ZoomBlurPass> Ptr;
        
        ZoomBlurPass(const ofVec2f& aspect, bool arb, float centerX = 0.5, float centerY = 0.5,
                       float exposure = 0.48, float decay = 0.9, float density = 0.25,
                       float weight = 0.25, float clamp = 1);
        
        void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth);
        
        void setCenterX(float v){ centerX = v; }
        float getCenterX() { return centerX; }
        
        void setCenterY(float v){ centerY = v; }
        float getCenterY() { return centerY; }
        
        void setExposure(float v){ exposure = v; }
        float getExposure() { return exposure; }
        
        void setDecay(float v){ decay = v; }
        float getDecay() { return decay; }
        
        void setDensity(float v){ density = v; }
        float getDensity() { return density; }
        
        void setWeight(float v){ weight = v; }
        float getWeight() { return weight; }
        
        void setClamp(float v){ clamp = v; }
        float getClamp() { return clamp; }
    private:
        
        ofShader shader;
        
        float centerX;
        float centerY;
        float exposure;
        float decay;
        float density;
        float weight;
        float clamp;
        
    };
}
