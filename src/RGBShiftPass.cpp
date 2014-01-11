/*
 *  RGBShiftPass.h
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
#include "RGBShiftPass.h"
#include "ofMain.h"

namespace itg
{
    RGBShiftPass::RGBShiftPass(const ofVec2f& aspect, float amount, float angle) :
        amount(amount), angle(angle), RenderPass(aspect, "RGBShift")
    {

        string fragShaderSrc = STRINGIFY(
                                         uniform sampler2D tDiffuse;
                                         uniform float amount;
                                         uniform float angle;

                                         void main() {
                                         vec2 vUv = gl_TexCoord[0].st;
                                         vec2 offset = amount * vec2( cos(angle), sin(angle));
                                         vec4 cr = texture2D(tDiffuse, vUv + offset);
                                         vec4 cga = texture2D(tDiffuse, vUv);
                                         vec4 cb = texture2D(tDiffuse, vUv - offset);
                                         gl_FragColor = vec4(cr.r, cga.g, cb.b, cga.a);

                                         }
        );


        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();

    }


    void RGBShiftPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        writeFbo.begin();


        shader.begin();

        shader.setUniformTexture("tDiffuse", readFbo.getTextureReference(), 0);
        shader.setUniform1f("amount", amount);
        shader.setUniform1f("angle", angle);

        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());

        shader.end();
        writeFbo.end();
    }
}
