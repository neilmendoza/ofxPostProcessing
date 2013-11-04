/*
 *  FxaaPass.cpp
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
#include "FxaaPass.h"

namespace itg
{
    FxaaPass::FxaaPass(const ofVec2f& aspect, bool arb) : RenderPass(aspect, arb, "fxaa")
    {
        string fragShaderSrc = STRINGIFY(
             uniform SAMPLER_TYPE tDiffuse;
             uniform vec2 resolution;
             
             varying vec2 vUv;
             
             const float FXAA_REDUCE_MIN = 1.0/128.0;
             const float FXAA_REDUCE_MUL = 1.0/8.0;
             const float FXAA_SPAN_MAX = 8.0;
             
             void main() {
                 
                 vec3 rgbNW = TEXTURE_FN( tDiffuse, ( gl_FragCoord.xy + vec2( -1.0, -1.0 ) ) * resolution ).xyz;
                 vec3 rgbNE = TEXTURE_FN( tDiffuse, ( gl_FragCoord.xy + vec2( 1.0, -1.0 ) ) * resolution ).xyz;
                 vec3 rgbSW = TEXTURE_FN( tDiffuse, ( gl_FragCoord.xy + vec2( -1.0, 1.0 ) ) * resolution ).xyz;
                 vec3 rgbSE = TEXTURE_FN( tDiffuse, ( gl_FragCoord.xy + vec2( 1.0, 1.0 ) ) * resolution ).xyz;
                 vec4 rgbaM  = TEXTURE_FN( tDiffuse,  gl_FragCoord.xy  * resolution );
                 vec3 rgbM  = rgbaM.xyz;
                 float opacity  = rgbaM.w;
                 
                 vec3 luma = vec3( 0.299, 0.587, 0.114 );
                 
                 float lumaNW = dot( rgbNW, luma );
                 float lumaNE = dot( rgbNE, luma );
                 float lumaSW = dot( rgbSW, luma );
                 float lumaSE = dot( rgbSE, luma );
                 float lumaM  = dot( rgbM,  luma );
                 float lumaMin = min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) );
                 float lumaMax = max( lumaM, max( max( lumaNW, lumaNE) , max( lumaSW, lumaSE ) ) );
                 
                 vec2 dir;
                 dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
                 dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
                 
                 float dirReduce = max( ( lumaNW + lumaNE + lumaSW + lumaSE ) * ( 0.25 * FXAA_REDUCE_MUL ), FXAA_REDUCE_MIN );
                 
                 float rcpDirMin = 1.0 / ( min( abs( dir.x ), abs( dir.y ) ) + dirReduce );
                 dir = min( vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
                           max( vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
                               dir * rcpDirMin)) * resolution;
                 
                 vec3 rgbA = 0.5 * (
                                    TEXTURE_FN( tDiffuse, gl_FragCoord.xy  * resolution + dir * ( 1.0 / 3.0 - 0.5 ) ).xyz +
                                    TEXTURE_FN( tDiffuse, gl_FragCoord.xy  * resolution + dir * ( 2.0 / 3.0 - 0.5 ) ).xyz );
                 
                 vec3 rgbB = rgbA * 0.5 + 0.25 * (
                                                  TEXTURE_FN( tDiffuse, gl_FragCoord.xy  * resolution + dir * -0.5 ).xyz +
                                                  TEXTURE_FN( tDiffuse, gl_FragCoord.xy  * resolution + dir * 0.5 ).xyz );
                 
                 float lumaB = dot( rgbB, luma );
                 
                 if ( ( lumaB < lumaMin ) || ( lumaB > lumaMax ) ) {
                     
                     gl_FragColor = vec4( rgbA, opacity );
                     
                 } else {
                     
                     gl_FragColor = vec4( rgbB, opacity );
                     
                 }
                 
             }
        );
        ostringstream oss;
        oss << "#version 120" << endl;
        if (arb)
        {
            oss << "#define SAMPLER_TYPE sampler2DRect" << endl;
            oss << "#define TEXTURE_FN texture2DRect" << endl;
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
    }
    
    void FxaaPass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();
        
        shader.begin();
        
        shader.setUniformTexture("tDiffuse", readFbo.getTextureReference(), 0);
        if (arb) shader.setUniform2f("resolution", 1.f, 1.f);
        else shader.setUniform2f("resolution", 1.f / writeFbo.getWidth(), 1.f / writeFbo.getHeight());
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}