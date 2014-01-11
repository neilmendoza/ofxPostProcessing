/*
 *  ToonPass.cpp
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
#include "ToonPass.h"
#include "ofMain.h"

namespace itg
{
    ToonPass::ToonPass(const ofVec2f& aspect, float edgeThreshold, float level,
                       const ofVec4f& ambientColor,
                       const ofVec4f& diffuseColor,
                       const ofVec4f& specularColor,
                       bool isSpecular, float shinyness) :
        edgeThreshold(edgeThreshold), level(level), ambientColor(ambientColor), diffuseColor(diffuseColor), specularColor(specularColor), isSpecular(isSpecular), shinyness(shinyness), RenderPass(aspect, "toon")
    {
        string vertShaderSrc = STRINGIFY(
            varying vec3 v;
            varying vec3 N;

            void main()
            {
                v = vec3(gl_ModelViewMatrix * gl_Vertex);
                N = normalize(gl_NormalMatrix * gl_Normal);

                gl_TexCoord[0] = gl_MultiTexCoord0;
                gl_Position = ftransform();
            }
        );
        
        string fragShaderSrc = STRINGIFY(
            uniform sampler2D normalImage;
            uniform float textureSizeX;
            uniform float textureSizeY;
            uniform float normalEdgeThreshold;
            uniform float qLevel;
            uniform int bSpecular;
            uniform vec4 ambient;
            uniform vec4 diffuse;
            uniform vec4 specular;
            uniform float shinyness;
                                         
            varying vec3 v;
            varying vec3 N;

            vec3 getNormal(vec2 st){
                vec2 texcoord = clamp(st, 0.001, 0.999);
                return texture2D(normalImage, texcoord).rgb;
            }

            void main(void){
                float dxtex = 1.0 / textureSizeX;
                float dytex = 1.0 / textureSizeY;

                vec2 st = gl_TexCoord[0].st;
                // access center pixel and 4 surrounded pixel
                vec3 center = getNormal(st).rgb;
                vec3 left = getNormal(st + vec2(dxtex, 0.0)).rgb;
                vec3 right = getNormal(st + vec2(-dxtex, 0.0)).rgb;
                vec3 up = getNormal(st + vec2(0.0, -dytex)).rgb;
                vec3 down = getNormal(st + vec2(0.0, dytex)).rgb;

                // discrete Laplace operator
                vec3 laplace = abs(-4.0*center + left + right + up + down);
                // if one rgb-component of convolution result is over threshold => edge
                vec4 line = texture2D(normalImage, st);
                if(laplace.r > normalEdgeThreshold
                || laplace.g > normalEdgeThreshold
                || laplace.b > normalEdgeThreshold){
                    line = vec4(0.0, 0.0, 0.0, 1.0); // => color the pixel green
                } else {
                    line = vec4(1.0, 1.0, 1.0, 1.0); // black
                }

                //end Line;

                //start Phong

                //vec3 lightPosition = vec3(100.0, 100.0, 50.0);
                vec3 lightPosition = gl_LightSource[0].position.xyz;

                vec3 L = normalize(lightPosition - v);
                vec3 E = normalize(-v);
                vec3 R = normalize(-reflect(L,N));

                // ambient term
                vec4 Iamb = ambient;

                // diffuse term
                vec4 Idiff = texture2D( normalImage, gl_TexCoord[0].st) * diffuse;
                //vec4 Idiff = vec4(1.0, 1.0, 1.0, 1.0) * diffuse;
                Idiff *= max(dot(N,L), 0.0);
                Idiff = clamp(Idiff, 0.0, 1.0);

                // specular term
                vec4 Ispec = specular;
                Ispec *= pow(max(dot(R,E),0.0), shinyness);
                Ispec = clamp(Ispec, 0.0, 1.0); 
                
                vec4 color = Iamb + Idiff;
                if ( bSpecular == 1 ) color += Ispec;
                // store previous alpha value
                float alpha = color.a;
                // quantize process: multiply by factor, round and divde by factor
                color = floor(0.5 + (qLevel * color)) / qLevel;
                // set fragment/pixel color
                color.a = alpha;

                gl_FragColor = color * line;
            }
                                         

        );
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();
        
    }
    
    void ToonPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        
        writeFbo.begin();
        
        shader.begin();
        
        shader.setUniformTexture("normalImage", readFbo.getTextureReference(), 0);
        shader.setUniform1f("textureSizeX", writeFbo.getWidth());
        shader.setUniform1f("textureSizeY", writeFbo.getHeight());
        shader.setUniform1f("normalEdgeThreshold", 0.2);
        shader.setUniform1f("qLevel", 3.0);
        shader.setUniform1i("bSpecular", isSpecular ? 1 : 0);
        shader.setUniform4f("ambient", ambientColor.x, ambientColor.y, ambientColor.z, ambientColor.w);
        shader.setUniform4f("diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w);
        shader.setUniform4f("specular", specularColor.x, specularColor.y, specularColor.z, specularColor.w);
        shader.setUniform1f("shinyness", shinyness);
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}