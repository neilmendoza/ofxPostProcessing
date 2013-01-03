#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofBackground(0);
    
    ofSetCoordHandedness(OF_RIGHT_HANDED);
    
    // Setup post-processing chain
    post.init(ofGetWidth(), ofGetHeight());
    post.createPass<BloomPass>();
    post.createPass<BokehPass>();
    post.createPass<KaleidoscopePass>();
    post.createPass<NoiseWarpPass>();
    //post.createPass<EdgePass>();
    //post.createPass<FxaaPass>();
    
    // Setup box positions
    for (unsigned i = 0; i < NUM_BOXES; ++i)
    {
        //posns.push_back(ofVec3f(ofRandom(100, ofGetWidth() - 100), ofRandom(100, ofGetHeight() - 100), ofRandom(-100, 100)));
        posns.push_back(ofVec3f(ofRandom(-300, 300), ofRandom(-300, 300), ofRandom(-300, 300)));
        cols.push_back(ofColor::fromHsb(255 * i / (float)NUM_BOXES, 255, 255, 255));
    }
    
    // Setup light
	light.setPosition(0, 0, 3000);
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
    for (unsigned i = 0; i < posns.size(); ++i)
    {
        ofSetColor(cols[i]);
        ofBox(posns[i], 20);
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