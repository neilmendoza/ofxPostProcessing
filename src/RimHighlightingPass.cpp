/*
 *  RimHighlightingPass.cpp
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
#include "RimHighlightingPass.h"

namespace itg
{
    RimHighlightingPass::RimHighlightingPass(const ofVec2f& aspect) :
        RenderPass(aspect, "rimhighlighting")
    {
        string vertShaderSrc = STRINGIFY(
                                         varying vec3 normal;
                                         varying vec3 sides;
                                         varying vec2 v_texCoord;
                                         varying vec4 v_color;
                                         
                                         void main()
        {
            normal = gl_NormalMatrix * gl_Normal;
            gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
            sides = gl_Position.xyz;
            sides.x = sides.x - 128.0;
            
            v_texCoord = vec2(gl_MultiTexCoord0);
            v_color = gl_Color;
        }
                                         );
        
        string fragShaderSrc = STRINGIFY(
                                         varying vec3 normal;
                                         varying vec3 sides;
                                         varying vec2 v_texCoord;
                                         uniform sampler2D myTexture;
                                         varying vec4 v_color;
                                         
                                         void main()
        {
            float intensity;
            vec3 n = normalize(normal);
            vec4 color;
            intensity = dot(sides,n);
            
            gl_FragColor = texture2D(myTexture, v_texCoord);
            if (intensity >= 64.0)
            {
                gl_FragColor.b = gl_FragColor.b / 1.5;
                gl_FragColor.r = gl_FragColor.r * 1.5;
                gl_FragColor.g = gl_FragColor.g * 1.25;
            }
            else
            {
                gl_FragColor = gl_FragColor * v_color;
            }
        }
        );
        
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();

    }
    
    void RimHighlightingPass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();
        
        shader.begin();
        shader.setUniformTexture("myTexture", readFbo.getTextureReference(), 0);
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}
