/*
 *  HsbShiftPass.cpp
 *
 *  Copyright (c) 2013, Neil Mendoza, http://www.neilmendoza.com
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
#include "HsbShiftPass.h"

namespace nm
{
    HsbShiftPass::HsbShiftPass(const ofVec2f& aspect, bool arb, float hueShift, float saturationShift, float brightnessShift) :
        hueShift(hueShift), saturationShift(saturationShift), brightnessShift(brightnessShift), RenderPass(aspect, arb, "hsbshift")
    {
        string fragShaderSrc = STRINGIFY(
            uniform sampler2D tex;
            uniform float hueShift;
            uniform float saturationShift;
            uniform float brightnessShift;
                                         
            // https://love2d.org/wiki/HSV_color
            vec3 hsbToRgb(vec3 c) { return mix(vec3(1.),clamp((abs(fract(c.x+vec3(3.,2.,1.)/3.)*6.-3.)-1.),0.,1.),c.y)*c.z; }
            // http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
            vec3 rgbToHsb(vec3 c)
            {
                vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
                vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
                vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
                
                float d = q.x - min(q.w, q.y);
                float e = 1.0e-10;
                return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
            }
                                         
            void main()
            {
                vec3 hsb = rgbToHsb(texture2D(tex, gl_TexCoord[0].st).rgb);
                vec3 rgb = hsbToRgb(vec3(hsb.x + hueShift, hsb.y + saturationShift, hsb.z + brightnessShift));
                gl_FragColor = vec4(rgb, 1.0);
            }
        );
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();
    }
    
    void HsbShiftPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth)
    {
        writeFbo.begin();
        shader.begin();
        shader.setUniformTexture("tex", readFbo.getTexture(), 0);
        shader.setUniform1f("hueShift", hueShift);
        shader.setUniform1f("saturationShift", saturationShift);
        shader.setUniform1f("brightnessShift", brightnessShift);
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}
