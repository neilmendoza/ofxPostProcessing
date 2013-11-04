/*
 *  GodRaysPass.h
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

namespace itg
{
    class GodRaysPass : public RenderPass
    {
    public:
        //http://code.google.com/p/natureal/source/browse/trunk/PGR2project/shaders/godrays/godrays_fs.glsl?r=18
        typedef shared_ptr<GodRaysPass> Ptr;
        
        GodRaysPass(const ofVec2f& aspect, bool arb, const ofVec3f & lightPositionOnScreen = ofVec3f(0.5,0.5,0.5), float lightDirDOTviewDir = 0.3 );
        
        void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth);
        
        void setLightPositionOnScreen(const ofVec3f & val) { lightPositionOnScreen = val; }
        const ofVec3f getlightPositionOnScreen() { return lightPositionOnScreen; }
        
        void setLightDirDOTviewDir(float val) { lightDirDOTviewDir = val; }
        float getLightDirDOTviewDir() { return lightDirDOTviewDir; }
    private:
        
        ofShader shader;
        
        ofVec3f lightPositionOnScreen;
        float lightDirDOTviewDir;
    };
}