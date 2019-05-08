#include "ofApp.h"

void ofApp::setup()
{
    ofBackground(0);
    ofSetFrameRate(60);
    
    // Setup post-processing chain
    post.init(ofGetWidth(), ofGetHeight());

    post.createPass<BleachBypassPass>();
    post.createPass<BloomPass>();
    post.createPass<ContrastPass>();
    post.createPass<ConvolutionPass>();
    post.createPass<DofAltPass>();
    post.createPass<DofPass>();
    post.createPass<EdgePass>();
    post.createPass<FakeSSSPass>();
    post.createPass<FxaaPass>();
    post.createPass<GodRaysPass>();
    post.createPass<HorizontalTiltShifPass>();
    //post.createPass<HsbShiftPass>(); // use of undeclared identifier 'HsbShiftPass'
    post.createPass<KaleidoscopePass>();
    post.createPass<LimbDarkeningPass>();
    post.createPass<LUTPass>();
    post.createPass<NoiseWarpPass>();
    post.createPass<PixelatePass>();
    post.createPass<RGBShiftPass>();
    post.createPass<RimHighlightingPass>();
    post.createPass<SSAOPass>();
    post.createPass<ToonPass>();
    post.createPass<VerticalTiltShifPass>();
    post.createPass<ZoomBlurPass>();

    gui.setup("panel");
    gui.add(spread.set("spread", 100.0f, 50.0f, 300.0f));
    gui.add(scale.set("scale", 20.0f, 20.0f, 200.0f));

    for(auto & pass : post.getPasses()) {
        pass->setEnabled(false);
        ofParameter<bool> param;
        gui.add(param.set(pass->getName(), false));
        param.addListener(this, &ofApp::toggleListener);
        passes[pass->getName()] = pass;
    }

    // Setup box positions
    for (unsigned i = 0; i < NUM_BOXES; ++i) {
        posns.push_back(ofVec3f(ofRandomf(), ofRandomf(), ofRandomf()));
        cols.push_back(ofColor::fromHsb(i * 255.0f / NUM_BOXES, 255, ofRandom(255), 255));
    }
    
    // Setup light
	light.setPosition(1000, 1000, 2000);
    light.enable();

    // create our own box mesh as there is a bug with
    // normal scaling and ofDrawBox() at the moment
    boxMesh = ofMesh::box(1, 1, 1);
}

void ofApp::toggleListener(const void * sender, bool & value) {
    auto name = ((const ofParameter<bool>*) sender)->getName();
    passes[name]->setEnabled(value);
}

void ofApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

void ofApp::draw()
{
    // setup gl state
    ofEnableDepthTest();
    ofEnableLighting();

    // begin scene to post process
    post.begin(cam);
    
    // draw boxes
    for (unsigned i = 0; i < posns.size(); ++i)
    {
        ofSetColor(cols[i]);
        ofPushMatrix();
        ofTranslate(posns[i] * spread);
        ofRotateXRad(i);
        ofRotateYRad(i*3);
        ofScale(scale);
        boxMesh.draw();
        ofPopMatrix();
    }
    
    ofDrawAxis(100);
    
    // end scene and draw
    post.end();
    
    ofDisableDepthTest();
    ofDisableLighting();

    ofSetColor(ofColor::white);
    gui.draw();
}
