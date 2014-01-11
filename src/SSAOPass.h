/*
 *  SSAOPass.h
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
     * @see https://github.com/mrdoob/three.js/blob/master/examples/js/shaders/SSAOShader.js
     */
    class SSAOPass : public RenderPass
    {
    public:

        typedef std::shared_ptr<SSAOPass> Ptr;

        SSAOPass(const ofVec2f& aspect, float cameraNear = 1, float cameraFar = 1000, float fogNear = 1, float fogFar = 1000, bool fogEnabled = false, bool onlyAO = false, float aoClamp = 0.5, float lumInfluence = 0.9);

        void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth);

        void setCameraNear(float v){ cameraNear = v; }
        void setCameraFar(float v){ cameraFar = v; }
        void setFogNear(float v){ fogNear = v; }
        void setFogFar(float v){ fogFar = v; }
        void setFogEnabled(bool v){ fogEnabled = v; }
        void setOnlyAO(bool v){ onlyAO = v; }
        void setAoClamp(float v){ aoClamp = v; }
        void setLumInfluence(float v){ lumInfluence = v; }
    private:

        ofShader shader;

        float cameraNear;
        float cameraFar;
        float fogNear;
        float fogFar;
        bool fogEnabled;
        bool onlyAO;
        float aoClamp;
        float lumInfluence;

    };
}
