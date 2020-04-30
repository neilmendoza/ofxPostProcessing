/*
 *  LUTPass.cpp
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
#include "LUTPass.h"

namespace nm
{

    LUTPass::LUTPass(const ofVec2f& aspect, bool arb) : RenderPass(aspect, arb, "lut"), lut_tex(0)
    {
    }

    LUTPass::~LUTPass()
    {
        dispose();
    }

    struct RGB
    {
        float r, g, b;
    };

    LUTPass* LUTPass::loadLUT(string path)
    {
        dispose();

        vector<RGB> lut;
        int LUT_3D_SIZE = 0;

        // load .cube file
        {
            path = ofToDataPath(path);

            ifstream ifs(path.c_str());

            while (!ifs.eof())
            {
                string line;
                getline(ifs, line);

                if (line.empty()) continue;

                int v;
                if (sscanf(line.c_str(), "LUT_3D_SIZE %i", &v) == 1)
                {
                    LUT_3D_SIZE = v;
                    break;
                }
            }

            if (LUT_3D_SIZE == 0)
            {
                LUT_3D_SIZE = 32;
            }

            while (!ifs.eof())
            {
                string line;
                getline(ifs, line);

                if (line.empty()) continue;

                RGB v;
                if (sscanf(line.c_str(), "%f %f %f", &v.r, &v.g, &v.b) == 3)
                {
                    lut.push_back(v);
                }
            }

			if (lut.size() != (LUT_3D_SIZE * LUT_3D_SIZE * LUT_3D_SIZE)) ofLogError() << "LUT size is incorrect.";
        }

        // gen texture
        {
            glGenTextures(1, &lut_tex);

            glEnable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, lut_tex);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUT_3D_SIZE, LUT_3D_SIZE, LUT_3D_SIZE, 0, GL_RGB, GL_FLOAT, &lut[0]);

            glBindTexture(GL_TEXTURE_3D, 0);

            glDisable(GL_TEXTURE_3D);
        }

        // setup shader
        string fragShaderSrc = STRINGIFY(
            uniform sampler2D tex;
            uniform sampler3D lut_tex;

            void main()
            {
                vec4 c = texture2D(tex, gl_TexCoord[0].xy);
                vec3 src = c.rgb;
                src = clamp(src, 0., 0.98);
                vec3 dst = texture3D(lut_tex, src).rgb;
                gl_FragColor = gl_Color * vec4(dst, c.a);
            }
        );

        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();

        return this;
    }

    void LUTPass::dispose()
    {
        if (lut_tex)
        {
            glDeleteTextures(1, &lut_tex);
            lut_tex = NULL;

            shader.unload();
        }
    }

    void LUTPass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();

        shader.begin();

        shader.setUniformTexture("tex", readFbo.getTexture(), 0);
        shader.setUniformTexture("lut_tex", GL_TEXTURE_3D, lut_tex, 1);

        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());

        shader.end();

        writeFbo.end();
    }

}
