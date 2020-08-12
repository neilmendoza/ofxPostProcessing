/*
 *  PostProcessing.h
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
#include "ofCamera.h"

namespace itg
{
    class PostProcessing : public ofBaseDraws
    {
    public:
        typedef shared_ptr<PostProcessing> Ptr;
        
        void init(unsigned width = ofGetWidth(), unsigned height = ofGetHeight(), bool arb = false);
        void begin();
        void begin(ofCamera& cam);
        void end(bool autoDraw = true);
        
        // float rather than int and not const to override ofBaseDraws
        void draw(float x = 0.f, float y = 0.f) ;
        void draw(float x, float y, float w, float h) ;
        float getWidth() const { return width; }
        float getHeight() const { return height; }
        
        void debugDraw();
        
        template<class T>
        shared_ptr<T> createPass()
        {
            shared_ptr<T> pass = shared_ptr<T>(new T(ofVec2f(width, height), arb));
            passes.push_back(pass);
            return pass;
        }
        
        ofTexture& getProcessedTextureReference();
        
        // advanced
        void process(ofFbo& raw, bool hasDepthAsTexture = true);
        
        /**
         * Set flip.
         * Turn on if using ofEasyCam to fix flipping bug.
         */
        void setFlip(bool flip) { this->flip = flip; }
        
        unsigned size() const { return passes.size(); }
        RenderPass::Ptr operator[](unsigned i) const { return passes[i]; }
        vector<RenderPass::Ptr>& getPasses() { return passes; }
        unsigned getNumProcessedPasses() const { return numProcessedPasses; }
        
        ofFbo& getRawRef() { return raw; }
        
         float getHeight(){ return 0 ; };
         float getWidth(){ return 0 ; };

        
    private:
        void process();
        
        unsigned currentReadFbo;
        unsigned numProcessedPasses;
        unsigned width, height;
        bool flip;
        bool arb;
        
        ofFbo raw;
        ofFbo pingPong[2];
        vector<RenderPass::Ptr> passes;
    };
}
