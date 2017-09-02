#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetLogLevel("ofxDuktapeLiveEditor", OF_LOG_VERBOSE);
    string meta = ofxEditor::getSuperAsModifier() ? "Cmd" : "Ctrl";
    ofSetWindowTitle(string("ofxDuktape live editor: ")+meta+"+[0-9]: editors, "+meta+"+R: REPL, "+meta+"+F: toggle fullscreen");
    ofxEditor::loadFont("fonts/PrintChar21.ttf", 21);
    ofxRepl::setReplBanner("Oh hi, welcome to ofxDuktape!");
    ofxRepl::setReplPrompt(">");
    editor.setup(this);
}

void ofApp::saveFileEvent(int& whichEditor) {
    
}

void ofApp::openFileEvent(int& whichEditor) {
    
}

void ofApp::executeScriptEvent(int& whichEditor) {
    string script = editor.getText(whichEditor);
    string filename = editor.getEditorFilename(whichEditor);
    int retcode = duk.pCompileStringFilename(filename, script, 0);
    if (retcode != 0) {
        editor.evalReplReturn(string("error: ") + duk.safeToString(-1));
    } else {
        int call_retcode = duk.pCall(0);
        
        editor.evalReplReturn(duk.safeToString(-1));
    }
}

void ofApp::evalReplEvent(const string& text) {
    int retcode = duk.pEvalString(text);
    string ret = duk.safeToString(-1);
    if (retcode != 0) {
        editor.evalReplReturn(ret);
    } else {
        editor.evalReplReturn(ret);
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    editor.draw();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    // make some room for the bottom editor info text
    editor.resize(w, h);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    bool modifier = ofGetKeyPressed(ofxEditor::getSuperAsModifier() ? OF_KEY_SUPER : OF_KEY_CONTROL);
    if(modifier) {
        switch(key) {
            case 'f': case 6:
            ofToggleFullscreen();
            return;
            case 'l': case 12:
            editor.setLineWrapping(!editor.getLineWrapping());
            return;
            case 'n': case 14:
            editor.setLineNumbers(!editor.getLineNumbers());
            return;
            case 'k': case 26:
            editor.setAutoFocus(!editor.getAutoFocus());
            return;
        }
    }
    
    editor.keyPressed(key);
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
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
