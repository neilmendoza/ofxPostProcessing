#pragma once

#include "ofMain.h"
#include "ofxPostProcessing.h"

class testApp : public ofBaseApp
{
public:
    static const unsigned NUM_BOXES = 100;
    
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxPostProcessing post;

    ofEasyCam cam;
    ofLight light;
    ofVboMesh mesh;

    vector<ofVec3f> boxes;

};
