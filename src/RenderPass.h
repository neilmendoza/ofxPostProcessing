/*
 *  RenderPass.h
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

//#define _ITG_TWEAKABLE

#include "ofFbo.h"
#include "ofVec3f.h"
#include "ofShader.h"
#ifdef _ITG_TWEAKABLE
    #include "Tweakable.h"
#endif

#define STRINGIFY(A) #A

namespace itg
{
    using namespace tr1;
    
    class RenderPass
#ifdef _ITG_TWEAKABLE
        : public Tweakable
#endif
    {
    public:
        typedef shared_ptr<RenderPass> Ptr;
        
        RenderPass(const ofVec2f& aspect, const string& name);
        
        virtual void render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depth);
        virtual void render(ofFbo& readFbo, ofFbo& writeFbo) {}
        
        void setEnabled(bool enabled) { this->enabled = enabled; }
        bool getEnabled() const { return enabled; }
        
        void enable() { enabled = true; }
        void disable() { enabled = false; }
        
        // for GUI
        bool& getEnabledRef() { return enabled; }
        
        void setAspect(const ofVec2f& aspect){ this->aspect = aspect; }

#ifndef _ITG_TWEAKABLE
        string getName() const { return name; }
#endif

    protected:
        void texturedQuad(float x, float y, float width, float height, float s = 1.0, float t = 1.0);
        
        ofVec2f aspect;
    
    private:
#ifndef _ITG_TWEAKABLE
        string name;
#endif
        bool enabled;
    };
}
