#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    
    touchServer.connect("127.0.0.1");
    touchServer.addEventListeners(this);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    stringstream report;
    report << "fps: " << ofGetFrameRate() << endl;
    report << "connected: " << touchServer.isConnected() << endl;
    report << "touches: " << touchServer.getActiveTouchCount() << endl;
    report << "server res: " << touchServer.getServerWidth() << " x " << touchServer.getServerHeight() << endl;
    report << "app res: " << ofGetWidth() << " x " << ofGetHeight();
    ofDrawBitmapStringHighlight(report.str(), 20, 20);
    touchServer.debugDraw(); 
    
}

void ofApp::onPQLabsTouchAdded(ofxPQLabsTouchEventArgs& args) {
    
    ofLogVerbose() << "ADDED id: " << args.id << " // type: " << args.type << " // pos: " << args.x << "," << args.y;    
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
