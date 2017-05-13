/*
 *  BloomPass.h
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
#pragma once

#include "RenderPass.h"
#include "ConvolutionPass.h"

namespace itg
{
    class BloomPass : public RenderPass
    {
    public:
        typedef shared_ptr<BloomPass> Ptr;
        
        BloomPass(const ofVec2f& aspect, bool arb, const ofVec2f& xBlur = ofVec2f(0.001953125, 0.0), const ofVec2f& yBlur = ofVec2f(0.0, 0.001953125), unsigned resolution = 256, bool aspectCorrect = true);
        
        void render(ofFbo& readFbo, ofFbo& writeFbo);
        
        void allocateSelectiveGlow(unsigned w, unsigned h);
        void beginSelectiveGlow(bool clear = true);
        void endSelectiveGlow();
        
        void debugDraw();
        
        bool hasArbShader() { return true; }
        
    private:
        ConvolutionPass::Ptr xConv;
        ConvolutionPass::Ptr yConv;
        
        ofFbo selectiveGlow;
        // small fbos for rendering stuff to glow
        ofFbo fbos[2];
        
        unsigned currentReadFbo;
        unsigned w, h;
    };
}