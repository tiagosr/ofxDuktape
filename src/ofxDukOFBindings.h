//
//  ofxDukOFBindings.h
//  liveCodeExample
//
//  Created by Tiago Rezende on 9/2/17.
//
//

#pragma once

#include "ofMain.h"
#include "ofxDuktape.h"

class ofxDukBindings {
    
    ofxDuktape& duk;
    ofxDukBindings(ofxDuktape& duk): duk(duk) {
        
    }
    virtual ~ofxDukBindings() {}
    
    void onUpdate(ofEventArgs& ev);
    void onDraw(ofEventArgs& ev);
    void onKeyEvent(ofKeyEventArgs& ev);
    void onMouseEvent(ofMouseEventArgs& ev);
    
public:
    static void setup(ofxDuktape& duk);
};
