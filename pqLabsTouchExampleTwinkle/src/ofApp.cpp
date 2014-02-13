#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    ofBackground(0);
    ofEnableAlphaBlending();
    //ofSetLogLevel(OF_LOG_VERBOSE);
    
    touchServer.connect("127.0.0.1");
    touchServer.addEventListeners(this);
    
    /*ofFbo::Settings settings;
    settings.width = ofGetWidth();
    settings.height = ofGetHeight();
    settings.internalformat = GL_RGBA32F_ARB;*/
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F_ARB);
    fbo.begin();
	ofClear(255,255,255, 0);
    fbo.end();
    
    ofSetCircleResolution(120);
    //ofSetCoordHandedness(OF_RIGHT_HANDED);
    
    // possible colors
    colors.resize(100);
    for(int i = 0; i < colors.size(); i++) {
        colors[i].setHsb(ofRandom(255), 255, 255);
    }
    
    
    doProcessing = false;
    post.init(ofGetWidth(), ofGetHeight());
    shared_ptr<PixelatePass> pass = post.createPass<PixelatePass>();
    pass->setEnabled(true);
    //pass->setEnabled(true);
    
    doBlur = true;
    gpuBlur.setup(ofGetWidth(), ofGetHeight(), 1, .2, 3);
    gpuBlur.begin();
    ofClear(255,255,255, 0);
    gpuBlur.end();
}

//--------------------------------------------------------------
void ofApp::update(){

    //fbo.begin();
    ofEnableAlphaBlending();
    if(doBlur) gpuBlur.begin();
    int fadeAmnt = 2;
    ofSetColor(0,0,0, fadeAmnt);
	ofRect(0,0,ofGetWidth(),ofGetHeight());
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    vector<ofxPQLabsTouchEventArgs>& touches = touchServer.getActiveTouches();    
    for (int i = 0; i < touches.size(); i++) {
        ofSetColor(colors[touches[i].id]);
        ofCircle(touches[i].x * touchServer.getServerWidth(), touches[i].y * touchServer.getServerHeight(), 30);
    }
    
    //fbo.end();
    ofDisableBlendMode();
    for (int i = 0; i < touches.size(); i++) {
        ofSetColor(colors[touches[i].id]);
        ofCircle(touches[i].x * touchServer.getServerWidth(), touches[i].y * touchServer.getServerHeight(), 30);
    }
    if(doBlur) gpuBlur.end();
        
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofEnableAlphaBlending();
    stringstream report;
    report << "fps: " << ofGetFrameRate() << endl;
    report << "connected: " << touchServer.isConnected() << endl;
    report << "touches: " << touchServer.getActiveTouchCount() << endl;
    report << "server res: " << touchServer.getServerWidth() << " x " << touchServer.getServerHeight() << endl;
    report << "app res: " << ofGetWidth() << " x " << ofGetHeight();
    ofDrawBitmapStringHighlight(report.str(), 20, 20);
    
    //touchServer.debugDraw();
    /*if(doProcessing) post.begin();
    if(doBlur) gpuBlur.begin();
    ofSetColor(255);
    fbo.draw(0,0);
    if(doBlur) {
       gpuBlur.end();
       gpuBlur.draw();
    }
    if(doProcessing) post.end();*/
    
    ofSetColor(255);
    
    gpuBlur.draw();
    
    
    
    
}

void ofApp::onPQLabsTouchAdded(ofxPQLabsTouchEventArgs& args) {
    
    ofLogVerbose() << "ADDED id: " << args.id << " // type: " << args.type << " // pos: " << args.x << "," << args.y;
    if(touchServer.getActiveTouchCount() == 1) {
        ofRandomize(colors);
    }
} 

void ofApp::onPQLabsTouchUpdated(ofxPQLabsTouchEventArgs& args) {
    
    ofLogVerbose() << "UPDATED id: " << args.id << " // type: " << args.type << " // pos: " << args.x << "," << args.y;}

void ofApp::onPQLabsTouchRemoved(ofxPQLabsTouchEventArgs& args) {
    
    ofLogVerbose() << "REMOVED id: " << args.id << " // type: " << args.type << " // pos: " << args.x << "," << args.y;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    if(key == 'x') {
        
        if(touchServer.isConnected() )
            touchServer.disconnect();
        else
            touchServer.connect("127.0.0.1");
    } else if(key == 's') {
        ofRandomize(colors);
    } else if(key == 'p') {
        doProcessing = !doProcessing;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
