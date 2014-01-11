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
#pragma once

#include "RenderPass.h"
#include "ofShader.h"

namespace itg
{
    /*
     * @see http://machinesdontcare.wordpress.com/2008/10/29/subsurface-scatter-shader/
     */
    class FakeSSSPass : public RenderPass
    {
    public:

        typedef shared_ptr<FakeSSSPass> Ptr;

        FakeSSSPass(const ofVec2f& aspect,
                    const ofPoint& lightPosition = ofPoint(0,0,0),
                    const ofVec4f& extinctionCoefficient = ofVec4f(0.8,0.3,0.1,1.0),
                    const ofVec4f& lightColor = ofVec4f(1.0, 1.0, 1.0, 1.0),
                    const ofVec4f& specularColor = ofVec4f(1.0, 1.0, 1.0, 1.0),
                    float specular = 1.0, float rimScale = 1.0,
                    float attenuationOffset = 3.0, float materialThickness = 0.6);

        void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth);

        void setLightPosition(const ofPoint& pt) { lightPosition.set(pt); }
        const ofPoint getLightPosition() { return lightPosition; }

        void setExtinctionCoefficient(const ofVec4f& pt) { extinctionCoefficient.set(pt); }
        const ofVec4f getExtinctionCoefficient() { return extinctionCoefficient; }

        void setLightColor(const ofVec4f& pt) { lightColor.set(pt); }
        const ofVec4f getLightColor() { return lightColor; }

        void setBaseColor(const ofVec4f& pt) { baseColor.set(pt); }
        const ofVec4f getBaseColor() { return baseColor; }

        void setSpecularColor(const ofVec4f& pt) { specularColor.set(pt); }
        const ofVec4f getSpecularColor() { return specularColor; }

        void setMaterialThickness(float val) { materialThickness = val; }
        float getMaterialThickness() { return materialThickness; }

        void setSpecular(float val) { specular = val; }
        float getSpecular() { return specular; }

        void setRimScale(float val) { rimScale = val; }
        float getRimScale() { return rimScale; }

        void setAttenuationOffset(float val) { attenuationOffset = val; }
        float getAttenuationOffset() { return attenuationOffset; }
    private:

        ofShader shader;

        ofPoint lightPosition;
        ofVec4f extinctionCoefficient;
        ofVec4f lightColor;
        ofVec4f baseColor;
        ofVec4f specularColor;
        float materialThickness;
        float specular;
        float rimScale;
        float attenuationOffset;
    };
}
