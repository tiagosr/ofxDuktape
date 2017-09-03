#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    duk.pushFunction([](ofxDuktape& duk)->duk_ret_t{
        ofLogNotice("ofxDuktape - inside VM")<< "hello!";
        duk.pushBool(true);
        return 1;
    }, 0);
    duk.putGlobalString("hello");
    //duk.pushString("hello()");
    //duk.pushString("hello.js");
    //duk.compile(0);
    duk.pCompileString("hello();", DUK_COMPILE_EVAL);
    duk.call(0);
    ofLogNotice("ofxDuktape - outside VM") << "return value is " << duk.safeToString(-1);
    duk.pop();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
