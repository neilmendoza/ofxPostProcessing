/*
 *  ToonPass.h
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
    class ToonPass : public RenderPass
    {
    public:
        //https://github.com/davertron/ToonCabinet
        typedef std::shared_ptr<ToonPass> Ptr;

        ToonPass(const ofVec2f& aspect, float edgeThreshold = 0.2, float level = 1.0,
                 const ofVec4f& ambientColor = ofVec4f(0.1,0.1,0.1,1.0),
                 const ofVec4f& diffuseColor = ofVec4f(0.9,0.9,0.9,1.0),
                 const ofVec4f& specularColor = ofVec4f(1,1,1,1),
                 bool isSpecular = false, float shinyness = 100);

        void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth);

        void setEdgeThreshold(float val) { edgeThreshold = val; }
        float getEdgeThreshold() { return edgeThreshold; }

        void setLevel(float val ) { level = val; }
        float getLevel() { return level; }

        void setAmbientColor(const ofVec4f& val) { ambientColor = val; }
        const ofVec4f getAmbientColor() { return ambientColor; }

        void setDiffuseColor(const ofVec4f& val) { diffuseColor = val; }
        const ofVec4f getDiffuseColor() { return diffuseColor; }

        void setSpecularColor(const ofVec4f& val) { specularColor = val; }
        const ofVec4f getSpecularColor() { return specularColor; }

        void setEnableSpecular(bool b) { isSpecular = b; }
        bool getEnableSpecular() { return isSpecular; }

        void setShinyness(float val ) { shinyness = val; }
        float getShinyness() { return shinyness; }

    private:

        ofShader shader;

        float edgeThreshold;
        float level;
        ofVec4f ambientColor;
        ofVec4f diffuseColor;
        ofVec4f specularColor;
        bool isSpecular;
        float shinyness;


    };
}
