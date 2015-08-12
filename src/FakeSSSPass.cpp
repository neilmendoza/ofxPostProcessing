/*
 *  FakeSSSPass.h
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
#include "FakeSSSPass.h"
#include "ofMain.h"

namespace itg
{
    FakeSSSPass::FakeSSSPass(const ofVec2f& aspect,
                             bool arb,
                             const ofPoint& lightPosition,
                             const ofVec4f& extinctionCoefficient,
                             const ofVec4f& lightColor,
                             const ofVec4f& specularColor,
                             float specular, float rimScale,
                             float attenuationOffset,
                             float materialThickness) : lightPosition(lightPosition),
    extinctionCoefficient(extinctionCoefficient), lightColor(lightColor),
    specularColor(specularColor), specular(specular), rimScale(rimScale),
    attenuationOffset(attenuationOffset), materialThickness(materialThickness),
    RenderPass(aspect, arb, "SSS")
    {
        baseColor.set(1.0, 1.0, 1.0, 1.0);
        
        string vertShaderSrc = 
        "uniform vec3 LightPosition;"
        "varying vec3 worldNormal, eyeVec, lightVec, vertPos, lightPos;"

        "void subScatterVS(vec4 ecVert){"
            "lightVec = LightPosition - ecVert.xyz;"
            "eyeVec = -ecVert.xyz;"
            "vertPos = ecVert.xyz;"
            "lightPos = LightPosition;"
        "}"


        "void main(){"
            "worldNormal = gl_NormalMatrix * gl_Normal;"
            "vec4 ecPos = gl_ModelViewProjectionMatrix * gl_Vertex;"
            "subScatterVS(ecPos);"
            "gl_Position = ecPos;"
            "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
        "}";
        
        string fragShaderSrc =
        "uniform float MaterialThickness;"
        "uniform vec3 ExtinctionCoefficient;"
        "uniform vec4 LightColor;"
        "uniform vec4 BaseColor;"
        "uniform vec4 SpecColor;"
        "uniform float SpecPower;"
        "uniform float RimScalar;"
        "uniform float AttenuationOffset;"
        "uniform sampler2D Texture;"

        "varying vec3 worldNormal, eyeVec, lightVec, vertPos, lightPos;"

        "float halfLambert(vec3 vect1, vec3 vect2){"
            "float product = dot(vect1,vect2);"
            "return product * 0.5 + 0.5;"
        "}"

        "float blinnPhongSpecular(vec3 normalVec, vec3 lightVec, float specPower){"
            "vec3 halfAngle = normalize(normalVec + lightVec);"
            "return pow(clamp(0.0,1.0,dot(normalVec,halfAngle)),specPower);"
        "}"

        "vec4 subScatterFS(){"
            "vec2 vUv = gl_TexCoord[0].st;"
            "float attenuation = 10.0 * (1.0 / distance(lightPos,vertPos)) + AttenuationOffset;"
            "vec3 eVec = normalize(eyeVec);"
            "vec3 lVec = normalize(lightVec);"
            "vec3 wNorm = normalize(worldNormal);"

            "vec4 dotLN = vec4(halfLambert(lVec,wNorm) * attenuation);"
            "dotLN *= texture2D(Texture, gl_TexCoord[0].xy);"
            "dotLN *= BaseColor;"

            "vec3 indirectLightComponent = vec3(MaterialThickness * max(0.0,dot(-wNorm,lVec)));"
            "indirectLightComponent += MaterialThickness * halfLambert(-eVec,lVec);"
            "indirectLightComponent *= attenuation;"
            "indirectLightComponent.r *= ExtinctionCoefficient.r;"
            "indirectLightComponent.g *= ExtinctionCoefficient.g;"
            "indirectLightComponent.b *= ExtinctionCoefficient.b;"
            "indirectLightComponent.rgb *= texture2D(Texture, gl_TexCoord[0].xy).rgb;"
            "vec3 rim = vec3(1.0 - max(0.0,dot(wNorm,eVec)));"
            "rim *= rim;"
            "rim *= max(0.0,dot(wNorm,lVec)) * SpecColor.rgb;"
        
            "vec4 finalCol = dotLN + vec4(indirectLightComponent,1.0);"
            "finalCol.rgb += (rim * RimScalar * attenuation * finalCol.a);"
            "finalCol.rgb += vec3(blinnPhongSpecular(wNorm,lVec,SpecPower) * attenuation * SpecColor * finalCol.a * 0.05);"
            "finalCol.rgb *= LightColor.rgb;"

            "return finalCol;"
        "}"


        "void main(){"
            "gl_FragColor = subScatterFS();"
        "}";
        
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();
        
    }
    

    void FakeSSSPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        writeFbo.begin();
        
        shader.begin();
        
        shader.setUniformTexture("Texture", readFbo.getTexture(), 0);
        shader.setUniform3f("LightPosition", lightPosition.x, lightPosition.y, lightPosition.z);
        shader.setUniform1f("MaterialThickness", materialThickness);
        shader.setUniform3f("ExtinctionCoefficient", extinctionCoefficient.x, extinctionCoefficient.y, extinctionCoefficient.z);
        shader.setUniform4f("LightColor", lightColor.x, lightColor.y, lightColor.z, 1.0);
        shader.setUniform4f("BaseColor", baseColor.x, baseColor.y, baseColor.z, 1.0);
        shader.setUniform4f("SpecColor", specularColor.y, specularColor.y, specularColor.z, 1.0 );
        shader.setUniform1f("SpecPower", specular);
        shader.setUniform1f("RimScalar", rimScale);
        shader.setUniform1f("AttenuationOffset", attenuationOffset);
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}