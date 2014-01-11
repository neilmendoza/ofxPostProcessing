/*
 *  PostProcessing.cpp
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
#include "PostProcessing.h"
#include "ofMain.h"

namespace itg
{
    void PostProcessing::init(unsigned width, unsigned height)
    {
        this->width = width;
        this->height = height;

        ofFbo::Settings s;
        s.width = ofNextPow2(width);
        s.height = ofNextPow2(height);
        s.textureTarget = GL_TEXTURE_2D;

        // no need to use depth for ping pongs
        for (int i = 0; i < 2; ++i)
        {
            pingPong[i].allocate(s);
        }

        s.useDepth = true;
        s.depthStencilInternalFormat = GL_DEPTH_COMPONENT24;
        s.depthStencilAsTexture = true;
        raw.allocate(s);

        numProcessedPasses = 0;
        currentReadFbo = 0;
        flip = true;
    }

    void PostProcessing::begin()
    {
        raw.begin(false);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glViewport(0, 0, raw.getWidth(), raw.getHeight());

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        ofPushStyle();
        glPushAttrib(GL_ENABLE_BIT);
    }

    void PostProcessing::begin(ofCamera& cam)
    {
        // update camera matrices
        cam.begin();
        cam.end();

        raw.begin(false);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(cam.getProjectionMatrix(ofRectangle(0, 0, width, height)).getPtr());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(cam.getModelViewMatrix().getPtr());

        glViewport(0, 0, raw.getWidth(), raw.getHeight());

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        ofPushStyle();
        glPushAttrib(GL_ENABLE_BIT);
    }

    void PostProcessing::end(bool autoDraw)
    {
        glPopAttrib();
        ofPopStyle();

        glViewport(0, 0, ofGetWidth(), ofGetHeight());

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        raw.end();

        ofPushStyle();
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        ofSetColor(255, 255, 255);
        process();
        if (autoDraw) draw();
        glPopAttrib();
        ofPopStyle();
    }

    void PostProcessing::debugDraw()
    {
        raw.getTextureReference().draw(10, 10, 300, 300);
        raw.getDepthTexture().draw(320, 10, 300, 300);
        pingPong[currentReadFbo].draw(630, 10, 300, 300);
    }

    void PostProcessing::draw(float x, float y)
    {
        draw(x, y, width, height);
    }

    void PostProcessing::draw(float x, float y, float w, float h)
    {
        if (flip)
        {
            glPushMatrix();
            glTranslatef(x, y + h, 0);
            glScalef(1, -1, 1);
        }
        else glTranslatef(x, y, 0);
        if (numProcessedPasses == 0) raw.draw(0, 0, w, h);
        else pingPong[currentReadFbo].draw(0, 0, w, h);
        if (flip) glPopMatrix();
    }

    ofTexture& PostProcessing::getProcessedTextureReference()
    {
        if (numProcessedPasses) return pingPong[currentReadFbo].getTextureReference();
        else return raw.getTextureReference();
    }

    // need to have depth enabled for some fx
    void PostProcessing::process(ofFbo& raw)
    {
        numProcessedPasses = 0;
        for (std::size_t i = 0; i < passes.size(); ++i)
        {
            if (passes[i]->getEnabled())
            {
                if (numProcessedPasses == 0) passes[i]->render(raw, pingPong[1 - currentReadFbo], raw.getDepthTexture());
                else passes[i]->render(pingPong[currentReadFbo], pingPong[1 - currentReadFbo], raw.getDepthTexture());
                currentReadFbo = 1 - currentReadFbo;
                ++numProcessedPasses;
            }
        }
    }

    void PostProcessing::process()
    {
        process(raw);
    }
}
