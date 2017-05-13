/*
 *  SaturatePass.cpp
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
#include "SaturatePass.h"
#include "ofMain.h"

namespace itg
{
    SaturatePass::SaturatePass(const ofVec2f& aspect, bool arb) :
        RenderPass(aspect, arb, "saturate"), threshold(.5f), colour(1.f, 1.f, 1.f, 1.f)
    {
        
        string fragShaderSrc = STRINGIFY(
             uniform sampler2D tex;
             uniform float threshold;
             uniform vec3 colour;
             uniform float darken;
             
             void main()
             {
                 vec4 texColour = texture2D(tex, gl_TexCoord[0].st);
                 float brightness = (texColour.r + texColour.g + texColour.b) / 3.0;
                 gl_FragColor = step(threshold, brightness) * vec4(colour, 1.0) + (1.0 - step(threshold, brightness)) * vec4(darken * texColour.rgb, 1.0);
             }
        );
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();
#ifdef _ITG_TWEAKABLE
        addParameter("threshold", this->threshold, "min=0 max=1");
        addParameter("darken", this->threshold, "min=0 max=1");
#endif
    }
    
    void SaturatePass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();
        ofClear(255, 255, 255, 255);
        
        shader.begin();
        shader.setUniformTexture("tex", readFbo.getTexture(), 0);
        shader.setUniform1f("threshold", threshold);
        shader.setUniform1f("darken", darken);
        shader.setUniform3f("colour", colour.r, colour.g, colour.b);
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        shader.end();
        
        writeFbo.end();
    }
}
