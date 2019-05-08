#pragma once

#include "ofMain.h"
#include "ofxPostProcessing.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
public:
    static const unsigned NUM_BOXES = 100;
    
    void setup();
    void update();
    void draw();

    // scene stuff
    ofxPostProcessing post;
    ofEasyCam cam;
    ofLight light;
    
    // boxes
    vector<ofVec3f> posns;
    vector<ofColor> cols;
    ofVboMesh boxMesh;

    ofParameter<float> spread;
    ofParameter<float> scale;

    std::map<std::string, RenderPass::Ptr> passes;

    ofxPanel gui;

    void toggleListener(const void * sender, bool & value);
};
