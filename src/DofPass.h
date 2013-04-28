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
    using namespace tr1;
    
    /**
     * @see https://github.com/mrdoob/three.js/blob/master/examples/js/shaders/BokehShader.js
     */
    class DofPass : public RenderPass
    {
    public:
        typedef shared_ptr<DofPass> Ptr;
        
        DofPass(const ofVec2f& aspect, float focus = 0.985, float aperture = 0.8, float maxBlur = 0.6);
        
        void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex);
        
        float getFocus() const { return focus; }
        void setFocus(float focus) { this->focus = focus; }
        
        float getAperture() const { return aperture; }
        void setAperture(float aperture) { this->aperture = aperture; }
        
        float getMaxBlur() const { return maxBlur; }
        void setMaxBlur(float maxBlur) { this->maxBlur = maxBlur; }
        
        float& getFocusRef() { return focus; }
        float& getApertureRef() { return aperture; }
        float& getMaxBlurRef() { return maxBlur; }
        
    private:
        ofShader shader;
        
        float focus;
        float aperture;
        float maxBlur;
    };
}