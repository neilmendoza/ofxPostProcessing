/*
 *  BloomAltPass.cpp
 *
 *  Copyright (c) 2020, Neil Mendoza, http://www.neilmendoza.com
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

#include "BloomAltPass.h"

namespace nm
{
    BloomAltPass::BloomAltPass(const ofVec2f& aspect, bool arb, float downSampleFactor) :
        downSampleFactor(downSampleFactor),
        RenderPass(aspect, arb, "bloomalt")
    {
        string luminanceFragShaderSrc = R"(
            VERSION
            
            uniform sampler2D tex;
            uniform float luminanceThreshold;
            uniform vec2 resolution;

            INOUT
            
            void main()
            {
                vec3 texel = TEXTURE_FUNCTION(tex, TEX_COORD).rgb;
                float luminance = dot(texel, vec3(0.2125, 0.7154, 0.0721));
                float thresholded = step(luminanceThreshold, luminance);

                FRAG_COLOR = vec4(thresholded, thresholded, thresholded, 1.0);
            }
        )";
        RenderPass::replacePlaceHolders(luminanceFragShaderSrc);
        RenderPass::setupShaderFromFragmentSource(luminanceFragShaderSrc, luminanceShader);
        
        string blurFragShaderSrc = R"(
            VERSION
            
            const int KERNEL_SIZE = 5;
            const float KERNEL[KERNEL_SIZE] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
            
            uniform sampler2D tex;
            uniform vec2 direction;
            
            INOUT
            
            void main()
            {
                vec2 texOffset = 1.0 / textureSize(tex, 0);
                vec3 blurred = texture(tex, texCoordVarying).rgb * KERNEL[0];
                for (int i = 1; i < KERNEL_SIZE; ++i)
                {
                    blurred += TEXTURE_FUNCTION(tex, TEX_COORD + (i * texOffset * direction)).rgb * KERNEL[i];
                    blurred += TEXTURE_FUNCTION(tex, TEX_COORD - (i * texOffset * direction)).rgb * KERNEL[i];
                }
                FRAG_COLOR = vec4(blurred, 1.0);
            }
        )";
        RenderPass::replacePlaceHolders(blurFragShaderSrc);
        RenderPass::setupShaderFromFragmentSource(blurFragShaderSrc, blurShader);
        
        string combineFragShaderSrc = R"(
            VERSION
            
            uniform sampler2D original;
            uniform sampler2D glow;
            
            INOUT
            
            void main()
            {
                vec3 originalTexel = TEXTURE_FUNCTION(original, TEX_COORD).rgb;
                vec3 glowTexel = TEXTURE_FUNCTION(glow, TEX_COORD).rgb;
                FRAG_COLOR = vec4(originalTexel + glowTexel, 1.0);
            }
        )";
        RenderPass::replacePlaceHolders(combineFragShaderSrc);
        RenderPass::setupShaderFromFragmentSource(combineFragShaderSrc, combineShader);
        
        ofFboSettings s;
        if (arb)
        {
            s.width = downSampleFactor * aspect.x;
            s.height = downSampleFactor * aspect.y;
            s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
        }
        else
        {
            int size = min(ofNextPow2(downSampleFactor * aspect.x), ofNextPow2(downSampleFactor * aspect.y));
            s.width = size;
            s.height = size;
            s.textureTarget = GL_TEXTURE_2D;
        }
        
        for (int i = 0; i < 2; ++i) downSampledFbos[i].allocate(s);
    }
    
    void BloomAltPass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        downSampledFbos[0].begin();
        ofClear(0, 0);
        luminanceShader.begin();
        luminanceShader.setUniformTexture("tex", readFbo.getTexture(), 0);
        luminanceShader.setUniform1f("luminanceThreshold", luminanceThreshold);
        quadMesh.draw();
        luminanceShader.end();
        downSampledFbos[0].end();
        
        for (unsigned i = 0; i < 2; ++i)
        {
            downSampledFbos[1].begin();
            ofClear(0, 0);
            blurShader.begin();
            blurShader.setUniformTexture("tex", downSampledFbos[0].getTexture(), 0);
            blurShader.setUniform2f("direction", 1.f, 0.f);
            quadMesh.draw();
            blurShader.end();
            downSampledFbos[1].end();
            
            downSampledFbos[0].begin();
            ofClear(0, 0);
            blurShader.begin();
            blurShader.setUniformTexture("tex", downSampledFbos[1].getTexture(), 0);
            blurShader.setUniform2f("direction", 0.f, 1.f);
            quadMesh.draw();
            blurShader.end();
            downSampledFbos[0].end();
        }
        
        writeFbo.begin();
        combineShader.begin();
        combineShader.setUniformTexture("original", readFbo.getTexture(), 0);
        combineShader.setUniformTexture("glow", downSampledFbos[0].getTexture(), 1);
        quadMesh.draw();
        combineShader.end();
        writeFbo.end();
    }
}
