/*
 *  BleachBypassPass.cpp
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
#include "BleachBypassPass.h"
#include "ofMain.h"

namespace itg
{
    BleachBypassPass::BleachBypassPass(const ofVec2f& aspect, float opacity) :
        opacity(opacity), RenderPass(aspect, "bleachbypass")
    {
        
        string fragShaderSrc = STRINGIFY(
                                         uniform float opacity;
                                         uniform sampler2D tDiffuse;
                                         void main() {
                                             vec2 vUv = gl_TexCoord[0].st;
                                             vec4 base = texture2D( tDiffuse, vUv );
                                             
                                             vec3 lumCoeff = vec3( 0.25, 0.65, 0.1 );
                                             float lum = dot( lumCoeff, base.rgb );
                                             vec3 blend = vec3( lum );
                                             
                                             float L = min( 1.0, max( 0.0, 10.0 * ( lum - 0.45 ) ) );
                                             
                                             vec3 result1 = 2.0 * base.rgb * blend;
                                             vec3 result2 = 1.0 - 2.0 * ( 1.0 - blend ) * ( 1.0 - base.rgb );
                                             
                                             vec3 newColor = mix( result1, result2, L );
                                             
                                             float A2 = opacity * base.a;
                                             vec3 mixRGB = A2 * newColor.rgb;
                                             mixRGB += ( ( 1.0 - A2 ) * base.rgb );
                                             
                                             gl_FragColor = vec4( mixRGB, base.a );
                                             
                                         }
        );
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();
        
    }
    
    void BleachBypassPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        writeFbo.begin();
        
        
        shader.begin();
        
        shader.setUniformTexture("tDiffuse", readFbo.getTextureReference(), 0);
        shader.setUniform1f("opacity", opacity);
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}