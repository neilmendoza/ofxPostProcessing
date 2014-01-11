/*
 *  ConvolutionPass.cpp
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
#include "ConvolutionPass.h"
#include "ofMain.h"

namespace itg
{
    ConvolutionPass::ConvolutionPass(const ofVec2f& aspect, const ofVec2f& imageIncrement, float sigma, unsigned kernelSize) :
        RenderPass(aspect, "convolution"), imageIncrement(imageIncrement)
    {
        // set up shader
        ostringstream oss;
        oss << "#version 120\n#define KERNEL_SIZE " << kernelSize << ".0\n";

        string vertShaderSrc =
        oss.str() +
        "uniform vec2 imageIncrement;"

        "varying vec2 vUv;"

        "void main()"
        "{"
            "gl_TexCoord[0] = gl_MultiTexCoord0;"
            "vUv = gl_TexCoord[0].st - ( ( KERNEL_SIZE - 1.0 ) / 2.0 ) * imageIncrement;"
            "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
        "}";

        oss.str("");
        oss << "#version 120\n#define KERNEL_SIZE " << kernelSize << "\n";
        string fragShaderSrc =
        oss.str() +
        "\n "
        "uniform float kernel[KERNEL_SIZE];"
        "uniform sampler2D readTex;"
        "uniform vec2 imageIncrement;"

        "varying vec2 vUv;"

        "void main()"
        "{"

            "vec2 imageCoord = vUv;"
            "vec4 sum = vec4( 0.0, 0.0, 0.0, 0.0 );"

            "for( int i = 0; i < KERNEL_SIZE; i++ )"
            "{"
                "sum += texture2D( readTex, imageCoord ) * kernel[ i ];"
                "imageCoord += imageIncrement;"
            "}"

            "gl_FragColor = sum;"
        "}";

        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();

        // build kernel
        buildKernel(sigma);
    }

    // We lop off the sqrt(2 * pi) * sigma term, since we're going to normalize anyway.
    float ConvolutionPass::gauss(float x, float sigma)
    {
        return expf( -( x * x ) / ( 2.0 * sigma * sigma ) );
    }

    void ConvolutionPass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();

        ofClear(0, 0, 0, 255);

        shader.begin();

        shader.setUniformTexture("readTex", readFbo, 0);
        shader.setUniform2f("imageIncrement", imageIncrement.x, imageIncrement.y);
        shader.setUniform1fv("kernel", kernel.data(), kernel.size());

        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());

        shader.end();
        writeFbo.end();
    }

    void ConvolutionPass::buildKernel(float sigma)
    {
        unsigned kernelSize = 2 * ceil( sigma * 3.0 ) + 1;

        if (kernelSize > MAX_KERNEL_SIZE) kernelSize = MAX_KERNEL_SIZE;

        kernel.clear();
        kernel.reserve(kernelSize);

        float halfWidth = ( kernelSize - 1 ) * 0.5;

        float sum = 0.0;
        for (unsigned i = 0; i < kernelSize; ++i )
        {
            kernel.push_back(gauss(i - halfWidth, sigma));
            sum += kernel.back();
        }

        // normalize the kernel
        for (unsigned i = 0; i < kernelSize; ++i )
        {
            kernel[ i ] /= sum;
        }
    }
}
