/*
 *  RenderPass.cpp
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
#include "RenderPass.h"

namespace itg
{
    RenderPass::RenderPass(const ofVec2f& aspect, const string& name) :
#ifdef _ITG_TWEAKABLE
        aspect(aspect), enabled(true), Tweakable(name)
    {
        addParameter("enable", enabled);
#else
        aspect(aspect), enabled(true), name(name)
    {
#endif
    }

    void RenderPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& /*depth*/)
    {
        render(readFbo, writeFbo);
    }

    void RenderPass::texturedQuad(float x, float y, float width, float height, float s, float t)
    {
        // TODO: change to triangle fan/strip
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(x, y, 0);

        glTexCoord2f(s, 0);
        glVertex3f(x + width, y, 0);

        glTexCoord2f(s, t);
        glVertex3f(x + width, y + height, 0);

        glTexCoord2f(0, t);
        glVertex3f(x, y + height, 0);
        glEnd();
    }
}
