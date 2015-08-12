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
#include "SSAOPass.h"
#include "ofMain.h"

namespace itg
{
    SSAOPass::SSAOPass(const ofVec2f& aspect, bool arb, float cameraNear, float cameraFar, float fogNear, float fogFar, bool fogEnabled, bool onlyAO, float aoClamp, float lumInfluence) :
        cameraNear(cameraNear), cameraFar(cameraFar), fogNear(fogNear), fogFar(fogFar), fogEnabled(fogEnabled), onlyAO(onlyAO), aoClamp(aoClamp), lumInfluence(lumInfluence), RenderPass(aspect, arb, "SSAO")
    {
        
        string fragShaderSrc = STRINGIFY(
            uniform float cameraNear;
            uniform float cameraFar;

            uniform float fogNear;
            uniform float fogFar;

            uniform bool fogEnabled;		// attenuate AO with linear fog
            uniform bool onlyAO; 		// use only ambient occlusion pass?

            uniform vec2 size;			// texture width, height
            uniform float aoClamp; 		// depth clamp - reduces haloing at screen edges

            uniform float lumInfluence;  // how much luminance affects occlusion

            uniform sampler2D tDiffuse;
            uniform sampler2D tDepth;

            //const float PI = 3.14159265;
            const float DL = 2.399963229728653; // PI * ( 3.0 - sqrt( 5.0 ) )
            const float EULER = 2.718281828459045;

            // helpers

            float width = size.x; 	// texture width
            float height = size.y; 	// texture height

            float cameraFarPlusNear = cameraFar + cameraNear;
            float cameraFarMinusNear = cameraFar - cameraNear;
            float cameraCoef = 2.0 * cameraNear;

            // user variables

            const int samples = 8; 		// ao sample count
            const float radius = 5.0; 	// ao radius

            const bool useNoise = false; 		 // use noise instead of pattern for sample dithering
            const float noiseAmount = 0.0003; // dithering amount

            const float diffArea = 0.4; 		// self-shadowing reduction
            const float gDisplace = 0.4; 	// gauss bell center

            const vec3 onlyAOColor = vec3( 1.0, 0.7, 0.5 );
            //const vec3 onlyAOColor = vec3( 1.0, 1.0, 1.0 );,


            // RGBA depth

            float unpackDepth( const in vec4 rgba_depth ) {
                const vec4 bit_shift = vec4( 1.0 / ( 256.0 * 256.0 * 256.0 ), 1.0 / ( 256.0 * 256.0 ), 1.0 / 256.0, 1.0 );
                float depth = dot( rgba_depth, bit_shift );
                return depth;
            }

            // generating noise / pattern texture for dithering

            vec2 rand( const vec2 coord ) {

                vec2 noise;

                if ( useNoise ) {
                 
                 float nx = dot ( coord, vec2( 12.9898, 78.233 ) );
                 float ny = dot ( coord, vec2( 12.9898, 78.233 ) * 2.0 );
                 
                 noise = clamp( fract ( 43758.5453 * sin( vec2( nx, ny ) ) ), 0.0, 1.0 );
                 
                } else {
                 
                 float ff = fract( 1.0 - coord.s * ( width / 2.0 ) );
                 float gg = fract( coord.t * ( height / 2.0 ) );
                 
                 noise = vec2( 0.25, 0.75 ) * vec2( ff ) + vec2( 0.75, 0.25 ) * gg;
                 
                }

                return ( noise * 2.0  - 1.0 ) * noiseAmount;

            }

            float doFog() {
                vec2 vUv = gl_TexCoord[0].st;
                float zdepth = unpackDepth( texture2D( tDepth, vUv ) );
                float depth = -cameraFar * cameraNear / ( zdepth * cameraFarMinusNear - cameraFar );

                return smoothstep( fogNear, fogFar, depth );

            }

            float readDepth( const in vec2 coord ) {

                //return ( 2.0 * cameraNear ) / ( cameraFar + cameraNear - unpackDepth( texture2D( tDepth, coord ) ) * ( cameraFar - cameraNear ) );,
                return cameraCoef / ( cameraFarPlusNear - unpackDepth( texture2D( tDepth, coord ) ) * cameraFarMinusNear );


            }

            float compareDepths( const in float depth1, const in float depth2, inout int far ) {

                float garea = 2.0; 						 // gauss bell width
                float diff = ( depth1 - depth2 ) * 100.0; // depth difference (0-100)

                // reduce left bell width to avoid self-shadowing

                if ( diff < gDisplace ) {
                 
                 garea = diffArea;
                 
                } else {
                 
                 far = 1;
                 
                }

                float dd = diff - gDisplace;
                float gauss = pow( EULER, -2.0 * dd * dd / ( garea * garea ) );
                return gauss;

            }

            float calcAO( float depth, float dw, float dh ) {
                vec2 vUv = gl_TexCoord[0].st;
                float dd = radius - depth * radius;
                vec2 vv = vec2( dw, dh );
                vec2 coord1 = vUv + dd * vv;
                vec2 coord2 = vUv - dd * vv;

                float temp1 = 0.0;
                float temp2 = 0.0;
                int far = 0;
                temp1 = compareDepths( depth, readDepth( coord1 ), far );

                // DEPTH EXTRAPOLATION

                if ( far > 0 ) {
                 temp2 = compareDepths( readDepth( coord2 ), depth, far );
                 temp1 += ( 1.0 - temp1 ) * temp2;
                }
                return temp1;
            }
            void main() {
                vec2 vUv = gl_TexCoord[0].st;
                vec2 noise = rand( vUv );
                float depth = readDepth( vUv );
                float tt = clamp( depth, aoClamp, 1.0 );
                float w = ( 1.0 / width )  / tt + ( noise.x * ( 1.0 - noise.x ) );
                float h = ( 1.0 / height ) / tt + ( noise.y * ( 1.0 - noise.y ) );
                float pw;
                float ph;
                float ao;
                float dz = 1.0 / float( samples );
                float z = 1.0 - dz / 2.0;
                float l = 0.0;
                for ( int i = 0; i <= samples; i ++ ) {
                 float r = sqrt( 1.0 - z );
                 pw = cos( l ) * r;
                 ph = sin( l ) * r;
                 ao += calcAO( depth, pw * w, ph * h );
                 z = z - dz;
                 l = l + DL;
                }
                ao /= float( samples );
                ao = 1.0 - ao;
                if ( fogEnabled ) {
                 ao = mix( ao, 1.0, doFog() );
                }
                vec3 color = texture2D( tDiffuse, vUv ).rgb;
                vec3 lumcoeff = vec3( 0.299, 0.587, 0.114 );
                float lum = dot( color.rgb, lumcoeff );
                vec3 luminance = vec3( lum );
                vec3 final = vec3( color * mix( vec3( ao ), vec3( 1.0 ), luminance * lumInfluence ) );
                if ( onlyAO ) {
                    final = onlyAOColor * vec3( mix( vec3( ao ), vec3( 1.0 ), luminance * lumInfluence ) );
                }
                gl_FragColor = vec4( final, 1.0 );
            }
        );
        
    
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
        shader.linkProgram();
        
    }
    

    void SSAOPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        writeFbo.begin();
        
        
        shader.begin();
        
        shader.setUniformTexture("tDiffuse", readFbo.getTexture(), 0);
        shader.setUniformTexture("tDepth", depthTex, 1);
        shader.setUniform2f("size", writeFbo.getWidth(), writeFbo.getHeight());
        shader.setUniform1f("cameraNear", cameraNear);
        shader.setUniform1f("cameraFar", cameraFar);
        shader.setUniform1f("fogNear", fogNear);
        shader.setUniform1f("fogFar", fogFar);
        shader.setUniform1i("fogEnabled", fogEnabled ? 1 : 0 );
        shader.setUniform1i("onlyAO", onlyAO ? 1 : 0);
        shader.setUniform1f("aoClamp", aoClamp);
        shader.setUniform1f("lumInfluence", lumInfluence);
        
        texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
        
        shader.end();
        writeFbo.end();
    }
}