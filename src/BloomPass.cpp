/*
 *  BloomPass.cpp
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
#include "BloomPass.h"
#include "ofMain.h"

namespace itg
{
    namespace gl
    {
        BloomPass::BloomPass(const ofVec2f& aspect, const ofVec2f& xBlur, const ofVec2f& yBlur, unsigned resolution) : RenderPass(aspect, "bloom")
        {
            currentReadFbo = 0;
            if (resolution != ofNextPow2(resolution)) ofLogWarning() << "Resolution " << resolution << " is not a power of two, using " << ofNextPow2(resolution);
            
            xConv = ConvolutionPass::Ptr(new ConvolutionPass(aspect, xBlur));
            yConv = ConvolutionPass::Ptr(new ConvolutionPass(aspect, yBlur));
            
            ofFbo::Settings s;
            s.width = ofNextPow2(resolution);
            s.height = ofNextPow2(resolution);
            s.textureTarget = GL_TEXTURE_2D;
            s.useDepth = true;
            
            for (int i = 0; i < 2; ++i) fbos[i].allocate(s);
        }
        
        void BloomPass::allocateSelectiveGlow(unsigned w, unsigned h)
        {
            this->w = w;
            this->h = h;
            
            ofFbo::Settings s;
            s.textureTarget = GL_TEXTURE_2D;
            s.width = ofNextPow2(w);
            s.height = ofNextPow2(h);
            s.useDepth = true;
            selectiveGlow.allocate(s);
            selectiveGlow.begin();
            ofClear(0,0,0,255);
            selectiveGlow.end();
        }
        
        void BloomPass::beginSelectiveGlow(bool clear)
        {
            selectiveGlow.begin();
            glPushMatrix();
            glScalef(1, -1, 1);
            glTranslatef(0, -ofNextPow2(h), 0);
            if (clear) ofClear(0,0,0, 255);
        }
        
        void BloomPass::endSelectiveGlow()
        {
            glPopMatrix();
            selectiveGlow.end();
        }
        
        void BloomPass::debugDraw()
        {
            glPushMatrix();
            glScalef(1, -1, 1);
            selectiveGlow.draw(0, -selectiveGlow.getHeight());
            glPopMatrix();
        }
        
        void BloomPass::render(ofFbo& readFbo, ofFbo& writeFbo)
        {
            if (selectiveGlow.isAllocated()) xConv->render(selectiveGlow, fbos[0]);
            else xConv->render(readFbo, fbos[0]);
            yConv->render(fbos[0], fbos[1]);
            
            writeFbo.begin();
            ofClear(0, 0, 0, 255);
            ofSetColor(255, 255, 255);
            readFbo.draw(0, 0);
            ofEnableAlphaBlending();
            glBlendFunc(GL_ONE, GL_ONE);
            fbos[1].draw(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            ofDisableAlphaBlending();
            writeFbo.end();
        }
    }
}