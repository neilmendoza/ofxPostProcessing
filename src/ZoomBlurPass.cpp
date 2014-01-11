/*
 *  ZoomBlurPass.cpp
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
#include "ZoomBlurPass.h"
#include "ofMain.h"

namespace itg
{
    ZoomBlurPass::ZoomBlurPass(const ofVec2f& aspect, float centerX, float centerY,
                                   float exposure, float decay, float density,
                                   float weight, float clamp) :
        centerX(centerX), centerY(centerY), exposure(exposure), decay(decay), density(density), weight(weight), clamp(clamp), RenderPass(aspect, "zoomblur")
    {
        
        string fragShaderSrc = STRINGIFY(
            uniform sampler2D tDiffuse;

            uniform float fX;
            uniform float fY;
            uniform float fExposure;
            uniform float fDecay;
            uniform float fDensity;
            uniform float fWeight;
            uniform float fClamp;

            const int iSamples = 20;

            void main()
            {
                vec2 vUv = gl_TexCoord[0].st;
                vec2 deltaTextCoord = vec2(vUv - vec2(fX,fY));
                deltaTextCoord *= 1.0 /  float(iSamples) * fDensity;
                vec2 coord = vUv;
                float illuminationDecay = 1.0;
                vec4 FragColor = vec4(0.0);

                for(int i=0; i < iSamples ; i++)
                {
                    coord -= deltaTextCoord;
                    vec4 texel = texture2D(tDiffuse, coord);
                    texel *= illuminationDecay * fWeight;

                    FragColor += texel;

                    illuminationDecay *= fDecay;
                }
                FragColor *= fExposure;
                FragColor = clamp(FragColor, 0.0, fClamp);
                gl_FragColor = FragColor;
            }
        );
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();
        
    }
    
    void ZoomBlurPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        writeFbo.begin();
        
        
        shader.begin();
        
        shader.setUniformTexture("tDiffuse", readFbo.getTextureReference(), 0);
        shader.setUniform1f("fX", centerX);
        shader.setUniform1f("fY", centerY);
        shader.setUniform1f("fExposure", exposure);
        shader.setUniform1f("fDecay", decay);
        shader.setUniform1f("fDensity", density);
        shader.setUniform1f("fWeight", weight);
        shader.setUniform1f("fClamp", clamp);
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}