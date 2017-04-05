#include "testApp.h"

void testApp::setup()
{
    ofBackground(0);
    
    ofSetCoordHandedness(OF_RIGHT_HANDED);
    
    // Setup post-processing chain
    post.init(ofGetWidth(), ofGetHeight());

    post.createPass<BleachBypassPass>()->setEnabled(false);
    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<ContrastPass>()->setEnabled(false);
    post.createPass<ConvolutionPass>()->setEnabled(false);
    post.createPass<DofAltPass>()->setEnabled(false);
    post.createPass<DofPass>()->setEnabled(false);
    post.createPass<EdgePass>()->setEnabled(false);
    post.createPass<FakeSSSPass>()->setEnabled(false);
    post.createPass<FxaaPass>()->setEnabled(false);
    post.createPass<GodRaysPass>()->setEnabled(false);
    post.createPass<HorizontalTiltShifPass>()->setEnabled(false);
    post.createPass<KaleidoscopePass>()->setEnabled(false);
    post.createPass<LimbDarkeningPass>()->setEnabled(false);
    post.createPass<LUTPass>()->setEnabled(false);
    post.createPass<NoiseWarpPass>()->setEnabled(false);
    post.createPass<RGBShiftPass>()->setEnabled(false);
    post.createPass<RimHighlightingPass>()->setEnabled(false);
    post.createPass<SSAOPass>()->setEnabled(false);
    post.createPass<ToonPass>()->setEnabled(false);
    post.createPass<VerticalTiltShifPass>()->setEnabled(false);
    post.createPass<ZoomBlurPass>()->setEnabled(false);
    
    // Setup box positions
    for (unsigned i = 0; i < NUM_BOXES; ++i)
    {
        posns.push_back(ofVec3f(ofRandom(-300, 300), ofRandom(-300, 300), ofRandom(-300, 300)));
        cols.push_back(ofColor::fromHsb(255 * i / (float)NUM_BOXES, 255, 255, 255));
    }
    
    // Setup light
	light.setPosition(1000, 1000, 2000);
    
    // create our own box mesh as there is a bug with
    // normal scaling and ofDrawBox() at the moment
    boxMesh = ofMesh::box(20, 20, 20);
}

void testApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

void testApp::draw()
{
    // copy enable part of gl state
    glPushAttrib(GL_ENABLE_BIT);
    
    // setup gl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    light.enable();
    
    // begin scene to post process
    post.begin(cam);
    
    // draw boxes
    for (unsigned i = 0; i < posns.size(); ++i)
    {
        ofSetColor(cols[i]);
        ofPushMatrix();
        ofTranslate(posns[i]);
        boxMesh.draw();
        ofPopMatrix();
    }
    
    ofDrawAxis(100);
    
    // end scene and draw
    post.end();
    
    // set gl state back to original
    glPopAttrib();
    
    // draw help
    ofSetColor(0, 255, 255);
    ofDrawBitmapString("Number keys toggle effects, mouse rotates scene", 10, 20);
    for (unsigned i = 0; i < post.size(); ++i)
    {
        if (post[i]->getEnabled()) ofSetColor(0, 255, 255);
        else ofSetColor(255, 0, 0);
        ostringstream oss;
        oss << char('A'+i) << ": " << post[i]->getName() << (post[i]->getEnabled()?" (on)":" (off)");
        ofDrawBitmapString(oss.str(), 10, 20 * (i + 2));
    }
}

void testApp::keyPressed(int key)
{
    /**/ if (key >= 'a' && key <= 'z') key = key - 'a' + 'A';
    else if (key < 'A' || key > 'Z' ) return;
    unsigned idx = key - 'A';
    if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
}