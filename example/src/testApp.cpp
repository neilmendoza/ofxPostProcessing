#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofBackground(0);
    post.init(ofGetWidth(), ofGetHeight());
    //post.createPass<EdgePass>();
    post.createPass<FxaaPass>();
    //post.createPass<BokehPass>();
    //post->createPass<BloomPass>();
    //post.createPass<KaleidoscopePass>();
    post.createPass<BloomPass>();
    
    for (unsigned i = 0; i < NUM_BOXES; ++i)
    {
        boxes.push_back(ofVec3f(ofRandom(-100, 100), ofRandom(0, 100), ofRandom(-100, 100)));
    }
    light.setPosition(1000, 1000, 1000);
    light.enable();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

//--------------------------------------------------------------
void testApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw()
{
    post.begin(cam);
    for (unsigned i = 0; i < NUM_BOXES; ++i)
    {
        ofBox(boxes[i], 20);
    }
    post.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}