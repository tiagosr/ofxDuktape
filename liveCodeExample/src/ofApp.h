#pragma once

#include "ofMain.h"
#include "ofxDuktape.h"
#include "ofxGLEditor.h"

class ofApp : public ofBaseApp, public ofxGLEditorListener {

public:
    
    ofxGLEditor editor;
    ofxDuktape duk;
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void saveFileEvent(int& whichEditor);
    void openFileEvent(int& whichEditor);
    void executeScriptEvent(int& whichEditor);
    void evalReplEvent(const string& text);
		
};
