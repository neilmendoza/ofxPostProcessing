/*
 *  EdgePass.cpp
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
#include "EdgePass.h"

namespace nm
{
    EdgePass::EdgePass(const ofVec2f& aspect, bool arb) :
        RenderPass(aspect, arb, "edge"), hue(0.5f), saturation(0.f)
    {
        string fragShaderSrc = STRINGIFY(
            uniform SAMPLER_TYPE tex;
            uniform vec2 aspect;
            uniform float hue;
            uniform float saturation;
                                         
            vec2 texel = vec2(1.0 / aspect.x, 1.0 / aspect.y);

            // hard coded matrix values
            mat3 G[9] = mat3[]( mat3( 0.3535533845424652, 0, -0.3535533845424652, 0.5, 0, -0.5, 0.3535533845424652, 0, -0.3535533845424652 ),
                                mat3( 0.3535533845424652, 0.5, 0.3535533845424652, 0, 0, 0, -0.3535533845424652, -0.5, -0.3535533845424652 ),
                                mat3( 0, 0.3535533845424652, -0.5, -0.3535533845424652, 0, 0.3535533845424652, 0.5, -0.3535533845424652, 0 ),
                                mat3( 0.5, -0.3535533845424652, 0, -0.3535533845424652, 0, 0.3535533845424652, 0, 0.3535533845424652, -0.5 ),
                                mat3( 0, -0.5, 0, 0.5, 0, 0.5, 0, -0.5, 0 ),
                                mat3( -0.5, 0, 0.5, 0, 0, 0, 0.5, 0, -0.5 ),
                                mat3( 0.1666666716337204, -0.3333333432674408, 0.1666666716337204, -0.3333333432674408, 0.6666666865348816, -0.3333333432674408, 0.1666666716337204, -0.3333333432674408, 0.1666666716337204 ),
                                mat3( -0.3333333432674408, 0.1666666716337204, -0.3333333432674408, 0.1666666716337204, 0.6666666865348816, 0.1666666716337204, -0.3333333432674408, 0.1666666716337204, -0.3333333432674408 ),
                                mat3( 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408 ));
            
            vec3 hsv(float h,float s,float v) { return mix(vec3(1.),clamp((abs(fract(h+vec3(3.,2.,1.)/3.)*6.-3.)-1.),0.,1.),s)*v; }
                                         
            void main(void)
            {
                mat3 I;
                float cnv[9];
                vec3 sample;
            
                /* fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value */
                for (int i=0; i<3; i++)
                {
                    for (int j=0; j<3; j++)
                    {
                        sample = TEXTURE_FN(tex, gl_TexCoord[0].st + texel * vec2(i-1.0,j-1.0)).rgb;
                        I[i][j] = length(sample); 
                    }
                }

                /* calculate the convolution values for all the masks */
                for (int i=0; i<9; i++)
                {
                    float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
                    cnv[i] = dp3 * dp3; 
                }

                float M = (cnv[0] + cnv[1]) + (cnv[2] + cnv[3]);
                float S = (cnv[4] + cnv[5]) + (cnv[6] + cnv[7]) + (cnv[8] + M); 

                gl_FragColor = vec4(hsv(hue, saturation, sqrt(M/S)), 1.0);
            }
        );
        
        ostringstream oss;
        oss << "#version 120" << endl;
        if (arb)
        {
            oss << "#define SAMPLER_TYPE sampler2DRect" << endl;
            oss << "#define TEXTURE_FN texture2DRect" << endl;
            oss << "#extension GL_ARB_texture_rectangle : enable" << endl;
            oss << fragShaderSrc;
        }
        else
        {
            oss << "#define SAMPLER_TYPE sampler2D" << endl;
            oss << "#define TEXTURE_FN texture2D" << endl;
            oss << fragShaderSrc;
        }
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, oss.str());
        shader.linkProgram();
#ifdef _ITG_TWEAKABLE
        addParameter("hue", this->hue, "min=0 max=1");
        addParameter("saturation", this->saturation, "min=0 max=1");
#endif
    }
    
    void EdgePass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();
        
        shader.begin();
        shader.setUniformTexture("tex", readFbo.getTexture(), 0);
        if (arb) shader.setUniform2f("aspect", 1.f, 1.f);
        else shader.setUniform2f("aspect", aspect.x, aspect.y);
        shader.setUniform1f("hue", hue);
        shader.setUniform1f("saturation", saturation);
        
        if (arb) texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight(), readFbo.getWidth(), readFbo.getHeight());
        else texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        
        writeFbo.end();
    }
}
