//
//  ofxDukOFBindings.cpp
//  liveCodeExample
//
//  Created by Tiago Rezende on 9/2/17.
//
//

#include "ofxDukOFBindings.h"

static ofColor ofColorFromObject(ofxDuktape& duk, duk_idx_t index) {
    if (duk.isNumber(index)) {
        return ofColor(duk.getNumber(index));
    }
    if (duk.isArray(index)) {
        size_t len = duk.getLength(index);
        if (len > 3) {
            return ofColor(duk.getObjectNumber(index, 0),
                           duk.getObjectNumber(index, 1),
                           duk.getObjectNumber(index, 2),
                           duk.getObjectNumber(index, 3));
        } else if (len == 3) {
            return ofColor(duk.getObjectNumber(index, 0),
                           duk.getObjectNumber(index, 1),
                           duk.getObjectNumber(index, 2));
        } else if (len == 2) {
            return ofColor(duk.getObjectNumber(index, 0),
                           duk.getObjectNumber(index, 1));
        } else if (len == 1) {
            return ofColor(duk.getObjectNumber(index, 0));
        }
    }
    if (duk.hasPropString(index, "r") &&
        duk.hasPropString(index, "g") &&
        duk.hasPropString(index, "b"))
    {
        if (duk.hasPropString(index, "a")) {
            return ofColor(duk.getObjectNumber(index, "r"),
                           duk.getObjectNumber(index, "g"),
                           duk.getObjectNumber(index, "b"),
                           duk.getObjectNumber(index, "a"));
        } else {
            return ofColor(duk.getObjectNumber(index, "r"),
                           duk.getObjectNumber(index, "g"),
                           duk.getObjectNumber(index, "b"));
        }
    }
    return ofColor();
}
static ofRectangle ofRectangleFromObject(ofxDuktape& duk, duk_idx_t index) {
    return ofRectangle(
                       duk.getObjectNumber(index, "x"),
                       duk.getObjectNumber(index, "y"),
                       duk.getObjectNumber(index, "width"),
                       duk.getObjectNumber(index, "height"));
}
static duk_idx_t objectFromofColor(ofxDuktape& duk, const ofColor& c) {
    duk_idx_t obj = duk.pushObject();
    duk.putObjectInts(obj, {
        {"r", c.r},
        {"g", c.g},
        {"b", c.b},
        {"a", c.a}
    });
    return obj;
}
static duk_idx_t objectFromofRectangle(ofxDuktape& duk, const ofRectangle& c) {
    duk_idx_t obj = duk.pushObject();
    duk.putObjectNumbers(obj, {
        {"x", c.x},
        {"y", c.y},
        {"width", c.width},
        {"height", c.height}
    });
    return obj;
}

static duk_idx_t objectFromofVec2f(ofxDuktape& duk, ofVec2f v) {
    duk_idx_t obj = duk.pushObject();
    duk.putObjectNumbers(obj, { {"x", v.x}, {"y", v.y} });
    return obj;
}

static ofVec2f ofVec2fFromObject(ofxDuktape& duk, duk_idx_t i) {
    return ofVec2f(duk.getObjectNumber(i, "x"), duk.getObjectNumber(i, "y"));
}

static duk_idx_t objectFromofVec3f(ofxDuktape& duk, ofVec3f v) {
    duk_idx_t obj = duk.pushObject();
    duk.putObjectNumbers(obj, { {"x", v.x}, {"y", v.y}, {"z", v.z} });
    return obj;
}

static ofVec3f ofVec3fFromObject(ofxDuktape& duk, duk_idx_t i) {
    return ofVec3f(duk.getObjectNumber(i, "x"), duk.getObjectNumber(i, "y"), duk.getObjectNumber(i, "z"));
}

ofxDukBindings::ofxDukBindings(ofxDuktape& duk): duk(duk) {
    ofAddListener(ofEvents().update, this, &ofxDukBindings::onUpdate);
    ofAddListener(ofEvents().draw, this, &ofxDukBindings::onDraw);
    
    ofAddListener(ofEvents().mouseMoved, this, &ofxDukBindings::onMouseEvent);
    ofAddListener(ofEvents().mousePressed, this, &ofxDukBindings::onMouseEvent);
    ofAddListener(ofEvents().mouseReleased, this, &ofxDukBindings::onMouseEvent);
    ofAddListener(ofEvents().mouseEntered, this, &ofxDukBindings::onMouseEvent);
    ofAddListener(ofEvents().mouseExited, this, &ofxDukBindings::onMouseEvent);
    ofAddListener(ofEvents().mouseDragged, this, &ofxDukBindings::onMouseEvent);
    ofAddListener(ofEvents().mouseScrolled, this, &ofxDukBindings::onMouseEvent);
    
    ofAddListener(ofEvents().keyPressed, this, &ofxDukBindings::onKeyEvent);
    ofAddListener(ofEvents().keyReleased, this, &ofxDukBindings::onKeyEvent);

    ofAddListener(ofEvents().fileDragEvent, this, &ofxDukBindings::onDragEvent);
    ofAddListener(ofEvents().messageEvent, this, &ofxDukBindings::onMessageEvent);
}

ofxDukBindings::~ofxDukBindings() {
    ofRemoveListener(ofEvents().update, this, &ofxDukBindings::onUpdate);
    ofRemoveListener(ofEvents().draw, this, &ofxDukBindings::onDraw);
    
    ofRemoveListener(ofEvents().mouseMoved, this, &ofxDukBindings::onMouseEvent);
    ofRemoveListener(ofEvents().mousePressed, this, &ofxDukBindings::onMouseEvent);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxDukBindings::onMouseEvent);
    ofRemoveListener(ofEvents().mouseEntered, this, &ofxDukBindings::onMouseEvent);
    ofRemoveListener(ofEvents().mouseExited, this, &ofxDukBindings::onMouseEvent);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxDukBindings::onMouseEvent);
    ofRemoveListener(ofEvents().mouseScrolled, this, &ofxDukBindings::onMouseEvent);
    
    ofRemoveListener(ofEvents().keyPressed, this, &ofxDukBindings::onKeyEvent);
    ofRemoveListener(ofEvents().keyReleased, this, &ofxDukBindings::onKeyEvent);
    
    ofRemoveListener(ofEvents().fileDragEvent, this, &ofxDukBindings::onDragEvent);
    ofRemoveListener(ofEvents().messageEvent, this, &ofxDukBindings::onMessageEvent);
}

void ofxDukBindings::onDraw(ofEventArgs &ev) {
    auto top = duk.getTop();
    duk.getGlobalString("of");
    auto of_events = duk.getObjectObject(-1, "events");
    if (duk.getPropString(of_events, "draw")) {
        if (duk.isCallable(-1)) {
            duk.call(-1);
        }
    }
    duk.setTop(top);
}

void ofxDukBindings::onUpdate(ofEventArgs &ev) {
    auto top = duk.getTop();
    duk.getGlobalString("of");
    auto of_events = duk.getObjectObject(-1, "events");
    if (duk.getPropString(of_events, "update")) {
        if (duk.isCallable(-1)) {
            duk.call(0);
        }
    }
    duk.setTop(top);
}

void ofxDukBindings::onKeyEvent(ofKeyEventArgs &ev) {
    auto top = duk.getTop();
    duk.getGlobalString("of");
    auto of_events = duk.getObjectObject(-1, "events");
    string type = "keyPressed";
    switch(ev.type) {
        case ofKeyEventArgs::Pressed:
            type = "keyPressed";
            break;
        case ofKeyEventArgs::Released:
            type = "keyReleased";
            break;
    }
    if (duk.getPropString(-1, type)) {
        if (duk.isCallable(-1)) {
            auto ev_args = duk.pushObject();
            duk.putObjectInt(ev_args, "key", ev.key);
            duk.putObjectInt(ev_args, "keycode", ev.keycode);
            duk.putObjectInt(ev_args, "scancode", ev.scancode);
            duk.putObjectUint(ev_args, "codepoint", ev.codepoint);
            duk.putObjectString(ev_args, "type", type);
            duk.call(1);
        }
    }
    duk.setTop(top);
}

void ofxDukBindings::onMouseEvent(ofMouseEventArgs &ev) {
    auto top = duk.getTop();
    duk.getGlobalString("of");
    auto of_events = duk.getObjectObject(-1, "events");
    string type = "mouseMoved";
    switch(ev.type) {
        case ofMouseEventArgs::Moved:
            type = "mouseMoved";
            break;
        case ofMouseEventArgs::Pressed:
            type = "mousePressed";
            break;
        case ofMouseEventArgs::Released:
            type = "mouseReleased";
            break;
        case ofMouseEventArgs::Entered:
            type = "mouseEntered";
            break;
        case ofMouseEventArgs::Exited:
            type = "mouseExited";
            break;
        case ofMouseEventArgs::Dragged:
            type = "mouseDragged";
            break;
        case ofMouseEventArgs::Scrolled:
            type = "mouseScrolled";
            break;
    }
    if (duk.getPropString(-1, type)) {
        if (duk.isCallable(-1)) {
            auto ev_args = duk.pushObject();
            duk.putObjectInt(ev_args, "button", ev.button);
            duk.putObjectNumber(ev_args, "scrollX", ev.scrollX);
            duk.putObjectNumber(ev_args, "scrollY", ev.scrollY);
            duk.putObjectNumber(ev_args, "x", ev.x);
            duk.putObjectNumber(ev_args, "y", ev.y);
            duk.putObjectString(ev_args, "type", type);
            duk.call(1);
        }
    }
    duk.setTop(top);
}

void ofxDukBindings::onWindowResizeEvent(ofResizeEventArgs& ev) {
    auto top = duk.getTop();
    duk.getGlobalString("of");
    auto of_events = duk.getObjectObject(-1, "events");
    if (duk.getPropString(of_events, "windowResized")) {
        if (duk.isCallable(-1)) {
            duk.pushInt(ev.width);
            duk.pushInt(ev.height);
            duk.call(2);
        }
    }
    duk.setTop(top);
}

void ofxDukBindings::onDragEvent(ofDragInfo& dragInfo) {
    auto top = duk.getTop();
    duk.getGlobalString("of");
    auto of_events = duk.getObjectObject(-1, "events");
    if (duk.getPropString(of_events, "dragEvent")) {
        if (duk.isCallable(-1)) {
            auto ev_args = duk.pushObject();
            auto files_arr = duk.pushArray();
            int counter = 0;
            for (auto file: dragInfo.files) {
                duk.putObjectString(files_arr, counter, file);
                counter++;
            }
            duk.putPropString(ev_args, "files");
            auto position = duk.pushObject();
            duk.putObjectInt(position, "x", dragInfo.position.x);
            duk.putObjectInt(position, "y", dragInfo.position.y);
            duk.putPropString(ev_args, "position");
            duk.call(1);
        }
    }
    duk.setTop(top);
}

void ofxDukBindings::onMessageEvent(ofMessage &message) {
    auto top = duk.getTop();
    duk.getGlobalString("of");
    auto of_events = duk.getObjectObject(-1, "events");
    if (duk.getPropString(of_events, "gotMessage")) {
        if (duk.isCallable(-1)) {
            duk.pushString(message.message);
            duk.call(1);
        }
    }
    duk.setTop(top);
}

ofxDukBindings& ofxDukBindings::setup(ofxDuktape& duk) {
    auto of = duk.pushObject();
    
    void* bindings_store = duk.pushFixedBuffer(sizeof(ofxDukBindings));
    
    ofxDukBindings* bindings = new (bindings_store) ofxDukBindings(duk);
    
    /*
    duk.setFinalizerFunction(-1, [bindings](ofxDuktape& duk) {
        bindings->~ofxDukBindings();
        return 0;
    });
     */
    duk.putPropString(of, "\xff""bindings");
    
    auto of_events = duk.pushObject();
    // fill event slots with null
    duk.putObjectNull(of_events, "update");
    duk.putObjectNull(of_events, "draw");
    duk.putObjectNull(of_events, "mouseMoved");
    duk.putObjectNull(of_events, "mousePressed");
    duk.putObjectNull(of_events, "mouseReleased");
    duk.putObjectNull(of_events, "mouseDragged");
    duk.putObjectNull(of_events, "mouseEntered");
    duk.putObjectNull(of_events, "mouseExited");
    duk.putObjectNull(of_events, "mouseScrolled");
    duk.putObjectNull(of_events, "keyPressed");
    duk.putObjectNull(of_events, "keyReleased");
    duk.putObjectNull(of_events, "windowResized");
    duk.putObjectNull(of_events, "dragEvent");
    duk.putObjectNull(of_events, "gotMessage");
    
    duk.putPropString(of, "events");
    
    duk.putObjectConstInts(of,{
        {"LOOP_NONE", OF_LOOP_NONE},
        {"LOOP_PALINDROME", OF_LOOP_PALINDROME},
        {"LOOP_NORMAL",OF_LOOP_NORMAL},
        
        {"TARGET_OSX", OF_TARGET_OSX},
        {"TARGET_MINGW", OF_TARGET_MINGW},
        {"TARGET_WINVS", OF_TARGET_WINVS},
        {"TARGET_IOS", OF_TARGET_IOS},
        {"TARGET_ANDROID", OF_TARGET_ANDROID},
        {"TARGET_LINUX", OF_TARGET_LINUX},
        {"TARGET_LINUX64", OF_TARGET_LINUX64},
        {"TARGET_LINUXARMV6L", OF_TARGET_LINUXARMV6L},
        {"TARGET_LINUXARMV7L", OF_TARGET_LINUXARMV7L},
        {"TARGET_EMSCRIPTEN", OF_TARGET_EMSCRIPTEN},
        
        {"WINDOW", OF_WINDOW},
        {"FULLSCREEN", OF_FULLSCREEN},
        {"GAME_MODE", OF_GAME_MODE},
        
        {"ASPECT_RATIO_IGNORE", OF_ASPECT_RATIO_IGNORE},
        {"ASPECT_RATIO_KEEP", OF_ASPECT_RATIO_KEEP},
        {"ASPECT_RATIO_KEEP_BY_EXPANDING", OF_ASPECT_RATIO_KEEP_BY_EXPANDING},
        
        {"ALIGN_VERT_IGNORE", OF_ALIGN_VERT_IGNORE},
        {"ALIGN_VERT_TOP",    OF_ALIGN_VERT_TOP},
        {"ALIGN_VERT_BOTTOM", OF_ALIGN_VERT_BOTTOM},
        {"ALIGN_VERT_CENTER", OF_ALIGN_VERT_CENTER},
        
        {"ALIGN_HORZ_IGNORE", OF_ALIGN_HORZ_IGNORE},
        {"ALIGN_HORZ_LEFT",   OF_ALIGN_HORZ_LEFT},
        {"ALIGN_HORZ_RIGHT",  OF_ALIGN_HORZ_RIGHT},
        {"ALIGN_HORZ_CENTER", OF_ALIGN_HORZ_CENTER},
        
        {"RECTMODE_CORNER", OF_RECTMODE_CORNER},
        {"RECTMODE_CENTER", OF_RECTMODE_CENTER},
        
        {"SCALEMODE_FIT",             OF_SCALEMODE_FIT},
        {"SCALEMODE_FILL",            OF_SCALEMODE_FILL},
        {"SCALEMODE_CENTER",          OF_SCALEMODE_CENTER},
        {"SCALEMODE_STRETCH_TO_FILL", OF_SCALEMODE_STRETCH_TO_FILL},
        
        {"IMAGE_GRAYSCALE",   OF_IMAGE_GRAYSCALE},
        {"IMAGE_COLOR",       OF_IMAGE_COLOR},
        {"IMAGE_COLOR_ALPHA", OF_IMAGE_COLOR_ALPHA},
        {"IMAGE_UNDEFINED",   OF_IMAGE_UNDEFINED},
        
        {"BLENDMODE_DISABLED", OF_BLENDMODE_DISABLED},
        {"BLENDMODE_ALPHA",    OF_BLENDMODE_ALPHA},
        {"BLENDMODE_ADD",      OF_BLENDMODE_ADD},
        {"BLENDMODE_SUBTRACT", OF_BLENDMODE_SUBTRACT},
        {"BLENDMODE_MULTIPLY", OF_BLENDMODE_MULTIPLY},
        {"BLENDMODE_SCREEN",   OF_BLENDMODE_SCREEN},
        
        {"ORIENTATION_DEFAULT",  OF_ORIENTATION_DEFAULT},
        {"ORIENTATION_180",      OF_ORIENTATION_180},
        {"ORIENTATION_90_LEFT",  OF_ORIENTATION_90_LEFT},
        {"ORIENTATION_90_RIGHT", OF_ORIENTATION_90_RIGHT},
        {"ORIENTATION_UNKNOWN",  OF_ORIENTATION_UNKNOWN},
        
        {"GRADIENT_CIRCULAR", OF_GRADIENT_CIRCULAR},
        {"GRADIENT_LINEAR", OF_GRADIENT_LINEAR},
        {"GRADIENT_BAR", OF_GRADIENT_BAR},
        
        {"POLY_WINDING_ODD", OF_POLY_WINDING_ODD},
        {"POLY_WINDING_NONZERO", OF_POLY_WINDING_NONZERO},
        {"POLY_WINDING_POSITIVE", OF_POLY_WINDING_POSITIVE},
        {"POLY_WINDING_NEGATIVE", OF_POLY_WINDING_NEGATIVE},
        {"POLY_WINDING_ABS_GEQ_TWO", OF_POLY_WINDING_ABS_GEQ_TWO},
        
        {"LEFT_HANDED", OF_LEFT_HANDED},
        {"RIGHT_HANDED", OF_RIGHT_HANDED},
        
        {"MATRIX_MODELVIEW", OF_MATRIX_MODELVIEW},
        {"MATRIX_PROJECTION", OF_MATRIX_PROJECTION},
        {"MATRIX_TEXTURE", OF_MATRIX_TEXTURE},
        
        {"KEY_MODIFIER",  OF_KEY_MODIFIER},
        {"KEY_RETURN",    OF_KEY_RETURN},
        {"KEY_ESC",       OF_KEY_ESC},
        {"KEY_TAB",       OF_KEY_TAB},
        {"KEY_COMMAND",   OF_KEY_COMMAND},
        {"KEY_BACKSPACE", OF_KEY_BACKSPACE},
        {"KEY_DEL",       OF_KEY_DEL},
        {"KEY_F1",        OF_KEY_F1},
        {"KEY_F2",        OF_KEY_F2},
        {"KEY_F3",        OF_KEY_F3},
        {"KEY_F4",        OF_KEY_F4},
        {"KEY_F5",        OF_KEY_F5},
        {"KEY_F6",        OF_KEY_F6},
        {"KEY_F7",        OF_KEY_F7},
        {"KEY_F8",        OF_KEY_F8},
        {"KEY_F9",        OF_KEY_F9},
        {"KEY_F10",       OF_KEY_F10},
        {"KEY_F11",       OF_KEY_F11},
        {"KEY_F12",       OF_KEY_F12},
        
        {"KEY_LEFT",      OF_KEY_LEFT},
        {"KEY_UP",        OF_KEY_UP},
        {"KEY_RIGHT",     OF_KEY_RIGHT},
        {"KEY_DOWN",      OF_KEY_DOWN},
        {"KEY_PAGE_UP",   OF_KEY_PAGE_UP},
        {"KEY_PAGE_DOWN", OF_KEY_PAGE_DOWN},
        {"KEY_HOME",      OF_KEY_HOME},
        {"KEY_END",       OF_KEY_END},
        {"KEY_INSERT",    OF_KEY_INSERT},
        
        {"KEY_CONTROL",   OF_KEY_CONTROL},
        {"KEY_LEFT_CONTROL",OF_KEY_LEFT_CONTROL},
        {"KEY_RIGHT_CONTROL",OF_KEY_RIGHT_CONTROL},
        {"KEY_ALT",       OF_KEY_ALT},
        {"KEY_LEFT_ALT",  OF_KEY_LEFT_ALT},
        {"KEY_RIGHT_ALT", OF_KEY_RIGHT_ALT},
        {"KEY_SHIFT",     OF_KEY_SHIFT},
        {"KEY_LEFT_SHIFT",OF_KEY_LEFT_SHIFT},
        {"KEY_RIGHT_SHIFT",OF_KEY_RIGHT_SHIFT},
        {"KEY_SUPER",     OF_KEY_SUPER},
        {"KEY_LEFT_SUPER",OF_KEY_LEFT_SUPER},
        {"KEY_RIGHT_SUPER",OF_KEY_RIGHT_SUPER},
        {"KEY_LEFT_COMMAND", OF_KEY_LEFT_COMMAND},
        {"KEY_RIGHT_COMMAND", OF_KEY_RIGHT_COMMAND},
        
        {"MOUSE_BUTTON_1", OF_MOUSE_BUTTON_1},
        {"MOUSE_BUTTON_2", OF_MOUSE_BUTTON_2},
        {"MOUSE_BUTTON_3", OF_MOUSE_BUTTON_3},
        {"MOUSE_BUTTON_4", OF_MOUSE_BUTTON_4},
        {"MOUSE_BUTTON_5", OF_MOUSE_BUTTON_5},
        {"MOUSE_BUTTON_6", OF_MOUSE_BUTTON_6},
        {"MOUSE_BUTTON_7", OF_MOUSE_BUTTON_7},
        {"MOUSE_BUTTON_8", OF_MOUSE_BUTTON_8},
        {"MOUSE_BUTTON_LAST", OF_MOUSE_BUTTON_LAST},
        {"MOUSE_BUTTON_LEFT", OF_MOUSE_BUTTON_LEFT},
        {"MOUSE_BUTTON_MIDDLE", OF_MOUSE_BUTTON_MIDDLE},
        {"MOUSE_BUTTON_RIGHT", OF_MOUSE_BUTTON_RIGHT},
        
        {"CONSOLE_COLOR_RESTORE", OF_CONSOLE_COLOR_RESTORE},
        {"CONSOLE_COLOR_BLACK",   OF_CONSOLE_COLOR_BLACK},
        {"CONSOLE_COLOR_RED",     OF_CONSOLE_COLOR_RED},
        {"CONSOLE_COLOR_GREEN",   OF_CONSOLE_COLOR_GREEN},
        {"CONSOLE_COLOR_YELLOW",  OF_CONSOLE_COLOR_YELLOW},
        {"CONSOLE_COLOR_BLUE",    OF_CONSOLE_COLOR_BLUE},
        {"CONSOLE_COLOR_PURPLE",  OF_CONSOLE_COLOR_PURPLE},
        {"CONSOLE_COLOR_CYAN",    OF_CONSOLE_COLOR_CYAN},
        {"CONSOLE_COLOR_WHITE",   OF_CONSOLE_COLOR_WHITE},
        
        {"PIXELS_GRAY",       OF_PIXELS_GRAY},
        {"PIXELS_MONO",       OF_PIXELS_MONO},
        {"PIXELS_R",          OF_PIXELS_R},
        {"PIXELS_GRAY_ALPHA", OF_PIXELS_GRAY_ALPHA},
        {"PIXELS_RG",         OF_PIXELS_RG},
        {"PIXELS_RGB",        OF_PIXELS_RGB},
        {"PIXELS_BGR",        OF_PIXELS_BGR},
        {"PIXELS_RGBA",       OF_PIXELS_RGBA},
        {"PIXELS_BGRA",       OF_PIXELS_BGRA},
        {"PIXELS_RGB565",     OF_PIXELS_RGB565},
        {"PIXELS_NV12",       OF_PIXELS_NV12},
        {"PIXELS_NV21",       OF_PIXELS_NV21},
        {"PIXELS_YV12",       OF_PIXELS_YV12},
        {"PIXELS_I420",       OF_PIXELS_I420},
        {"PIXELS_YUY2",       OF_PIXELS_YUY2},
        {"PIXELS_UYVY",       OF_PIXELS_UYVY},
        {"PIXELS_Y",          OF_PIXELS_Y},
        {"PIXELS_U",          OF_PIXELS_U},
        {"PIXELS_V",          OF_PIXELS_V},
        {"PIXELS_UV",         OF_PIXELS_UV},
        {"PIXELS_VU",         OF_PIXELS_VU},
        {"PIXELS_NUM_FORMATS", OF_PIXELS_NUM_FORMATS},
        {"PIXELS_UNKNOWN",    OF_PIXELS_UNKNOWN},
        {"PIXELS_NATIVE",     OF_PIXELS_NATIVE},
        
        {"BITMAPMODE_SIMPLE",   OF_BITMAPMODE_SIMPLE},
        {"BITMAPMODE_SCREEN",   OF_BITMAPMODE_SCREEN},
        {"BITMAPMODE_VIEWPORT", OF_BITMAPMODE_VIEWPORT},
        {"BITMAPMODE_MODEL",    OF_BITMAPMODE_MODEL},
        {"BITMAPMODE_MODEL_BILLBOARD", OF_BITMAPMODE_MODEL_BILLBOARD},
        
        {"LOG_SILENT",  OF_LOG_SILENT},
        {"LOG_NOTICE",  OF_LOG_NOTICE},
        {"LOG_WARNING", OF_LOG_WARNING},
        {"LOG_VERBOSE", OF_LOG_VERBOSE},
        {"LOG_ERROR",   OF_LOG_ERROR},
        {"LOG_FATAL_ERROR", OF_LOG_FATAL_ERROR},
        
        {"ENCODING_UTF8",        OF_ENCODING_UTF8},
        {"ENCODING_ISO_8859_15", OF_ENCODING_ISO_8859_15},
        
    });
    
    duk.putObjectConstNumbers(of, {
        {"PI", PI},
        {"HALF_PI", HALF_PI},
        {"TWO_PI", TWO_PI},
        {"DEG_TO_RAD", DEG_TO_RAD},
        {"RAD_TO_DEG", RAD_TO_DEG},
        
    });
    
    // functions
    duk.putObjectSetters(of, {
        {"windowTitle", [](ofxDuktape&duk) { ofSetWindowTitle(duk.toString(0)); return 0; }},
        {"lineWidth", [](ofxDuktape&duk) { ofSetLineWidth(duk.getNumber(0)); return 0; }},
        {"escapeQuitsApp", [](ofxDuktape&duk) { ofSetEscapeQuitsApp(duk.toBool(0)); return 0; }},
        {"arbTex", [](ofxDuktape&duk) {
            if(duk.toBool(0)) { ofEnableArbTex(); }
            else { ofDisableArbTex(); }
            return 0;
        }},
        {"lighting", [](ofxDuktape&duk) {
            if(duk.toBool(0)) { ofEnableLighting(); }
            else { ofDisableLighting(); }
            return 0;
        }},
        {"separateSpecularLight", [](ofxDuktape&duk) {
            if(duk.toBool(0)) { ofEnableSeparateSpecularLight(); }
            else { ofDisableSeparateSpecularLight(); }
            return 0;
        }},
        {"depthTest", [](ofxDuktape&duk) {
            ofSetDepthTest(duk.toBool(0));
            return 0;
        }},
        {"smoothing", [](ofxDuktape&duk) {
            if(duk.toBool(0)) { ofEnableSmoothing(); }
            else { ofDisableSmoothing(); }
            return 0;
        }},
        {"dataPath", [](ofxDuktape&duk) {
            if(duk.toBool(0)) { ofEnableDataPath(); }
            else { ofDisableDataPath(); }
            return 0;
        }},
        {"dataPathRoot", [](ofxDuktape&duk) {
            ofSetDataPathRoot(duk.requireString(0));
            return 0;
        }},
    });
    
    
    duk.putObjectGetters(of, {
        {"windowWidth", [&](ofxDuktape& duk) { duk.pushNumber(ofGetWindowWidth()); return 1; }},
        {"windowHeight", [&](ofxDuktape& duk) { duk.pushNumber(ofGetWindowHeight()); return 1; }},
        {"screenWidth", [](ofxDuktape& duk) { duk.pushNumber(ofGetScreenWidth()); return 1; }},
        {"screenHeight", [](ofxDuktape& duk) { duk.pushNumber(ofGetScreenHeight()); return 1; }},
        {"windowPositionX", [](ofxDuktape& duk) { duk.pushNumber(ofGetWindowPositionX()); return 1; }},
        {"windowPositionY", [](ofxDuktape& duk) { duk.pushNumber(ofGetWindowPositionY()); return 1; }},
        {"mouseX", [](ofxDuktape& duk){ duk.pushNumber(ofGetMouseX()); return 1; }},
        {"mouseY", [](ofxDuktape& duk){ duk.pushNumber(ofGetMouseY()); return 1; }},
        {"previousMouseX", [](ofxDuktape& duk){ duk.pushNumber(ofGetPreviousMouseX()); return 1; }},
        {"previousMouseY", [](ofxDuktape& duk){ duk.pushNumber(ofGetPreviousMouseY()); return 1; }},
        {"anyKeyPressed", [](ofxDuktape& duk){ duk.pushBool(ofGetKeyPressed()); return 1; }},
        {"mousePressed", [](ofxDuktape& duk){ duk.pushBool(ofGetMousePressed()); return 1; }},
        {"seconds", [](ofxDuktape& duk){ duk.pushInt(ofGetSeconds()); return 1; }},
        {"minutes", [](ofxDuktape& duk){ duk.pushInt(ofGetMinutes()); return 1; }},
        {"hour", [](ofxDuktape& duk){ duk.pushInt(ofGetHours()); return 1; }},
        {"day", [](ofxDuktape& duk){ duk.pushInt(ofGetDay()); return 1; }},
        {"weekday", [](ofxDuktape& duk){ duk.pushInt(ofGetWeekday()); return 1; }},
        {"month", [](ofxDuktape& duk){ duk.pushInt(ofGetMonth()); return 1; }},
        {"year", [](ofxDuktape& duk){ duk.pushInt(ofGetYear()); return 1; }},
        {"frameNum", [](ofxDuktape& duk){ duk.pushUint(ofGetFrameNum()); return 1; }},
        {"lastFrameTime", [](ofxDuktape& duk){ duk.pushNumber(ofGetLastFrameTime()); return 1; }},
        {"elapsedTime", [](ofxDuktape& duk){ duk.pushNumber(ofGetElapsedTimef()); return 1; }},
        {"elapsedTimeMillis", [](ofxDuktape& duk){ duk.pushUint(ofGetElapsedTimeMillis()); return 1; }},
        {"elapsedTimeMicros", [](ofxDuktape& duk){ duk.pushUint(ofGetElapsedTimeMicros()); return 1; }},
        {"unixTime", [](ofxDuktape& duk){ duk.pushUint(ofGetUnixTime()); return 1; }},
        {"systemTime", [](ofxDuktape& duk){ duk.pushUint(ofGetSystemTime()); return 1; }},
        {"systemTimeMicros", [](ofxDuktape& duk){ duk.pushUint(ofGetSystemTimeMicros()); return 1; }},
        {"versionMajor", [](ofxDuktape& duk){ duk.pushUint(ofGetVersionMajor()); return 1; }},
        {"versionMinor", [](ofxDuktape& duk){ duk.pushUint(ofGetVersionMinor()); return 1; }},
        {"versionPatch", [](ofxDuktape& duk){ duk.pushUint(ofGetVersionPatch()); return 1; }},
    });
    
    duk.putObjectGettersSetters(of,{
        {"frameRate",
            [](ofxDuktape& duk){ duk.pushNumber(ofGetFrameRate()); return 1;},
            [](ofxDuktape& duk){ ofSetFrameRate(duk.getNumber(0)); return 0;}},
        {"backgroundColor",
            [](ofxDuktape& duk){ objectFromofColor(duk,ofGetBackgroundColor()); return 1;},
            [](ofxDuktape& duk){ ofSetBackgroundColor(ofColorFromObject(duk, 0)); return 0;}},
        {"viewport",
            [](ofxDuktape& duk){ objectFromofRectangle(duk,ofGetCurrentViewport()); return 1;},
            [](ofxDuktape& duk){ ofViewport(ofRectangleFromObject(duk, 0)); return 0;}},
        {"orientation",
            [](ofxDuktape& duk){ duk.pushUint(ofGetOrientation()); return 1; },
            [](ofxDuktape& duk){ ofSetOrientation((ofOrientation)duk.getUint(0)); return 0; }},
        {"logLevel",
            [](ofxDuktape& duk){ duk.pushUint(ofGetLogLevel()); return 1; },
            [](ofxDuktape& duk){ ofSetLogLevel((ofLogLevel)duk.getUint(0)); return 0; }},
        {"boxResolution",
            [](ofxDuktape& duk){ objectFromofVec3f(duk, ofGetBoxResolution()); return 1; },
            [](ofxDuktape& duk){
                if(duk.isObject(0)){
                    ofVec3f v = ofVec3fFromObject(duk, 0);
                    ofSetBoxResolution(v.x, v.y, v.z);
                } else {
                    ofSetBoxResolution(duk.getInt(0));
                }
                return 0; }},
        {"sphereResolution",
            [](ofxDuktape& duk){ duk.pushInt(ofGetSphereResolution()); return 1; },
            [](ofxDuktape& duk){ ofSetSphereResolution(duk.getInt(0)); return 0; }},
        {"icoSphereResolution",
            [](ofxDuktape& duk){ duk.pushInt(ofGetIcoSphereResolution()); return 1; },
            [](ofxDuktape& duk){ ofSetIcoSphereResolution(duk.getInt(0)); return 0; }},
        {"coneResolution",
            [](ofxDuktape& duk){ objectFromofVec3f(duk, ofGetConeResolution()); return 1; },
            [](ofxDuktape& duk){
                ofVec3f v = ofVec3fFromObject(duk, 0);
                ofSetConeResolution(v.x, v.y, v.z); return 0; }},
        {"cylinderResolution",
            [](ofxDuktape& duk){ objectFromofVec3f(duk, ofGetCylinderResolution()); return 1; },
            [](ofxDuktape& duk){
                ofVec3f v = ofVec3fFromObject(duk, 0);
                ofSetCylinderResolution(v.x, v.y, v.z); return 0; }},
        {"planeResolution",
            [](ofxDuktape& duk){ objectFromofVec2f(duk, ofGetPlaneResolution()); return 1; },
            [](ofxDuktape& duk){
                ofVec2f v = ofVec2fFromObject(duk, 0);
                ofSetPlaneResolution(v.x, v.y); return 0; }},
        /*
         {"",
         [](ofxDuktape& duk){ return 1; },
         [](ofxDuktape& duk){ return 0; }},
         */
    });
    
    duk.putObjectFunctions(of, {
        {"enableArbTex", [](ofxDuktape&duk) {
            ofEnableArbTex();
            return 0;
        }, 0},
        {"disableArbTex", [](ofxDuktape&duk) {
            ofDisableArbTex();
            return 0;
        }, 0},
        {"clear", [](ofxDuktape&duk) {
            ofClear(duk.getInt(0),
                    duk.getInt(1),
                    duk.getInt(2));
            return 0;
        }, 3},
        {"background", [&](ofxDuktape& duk) {
            switch(duk.getTop()-2) {
                case 4:
                    ofBackground(duk.getInt(0),
                                 duk.getInt(1),
                                 duk.getInt(2),
                                 duk.getInt(3));
                    break;
                case 3:
                    ofBackground(duk.getInt(0),
                                 duk.getInt(1),
                                 duk.getInt(2));
                    break;
                case 1:
                    if(duk.isObject(0))
                        ofBackground(ofColorFromObject(duk, 0));
                    else
                        ofBackground(duk.getNumber(0));
                    break;
                default: return DUK_RET_RANGE_ERROR;
            }
            return 0;
        }, DUK_VARARGS},
        {"backgroundGradient", [](ofxDuktape& duk) {
            ofBackgroundGradient(ofColorFromObject(duk, 0),
                                 ofColorFromObject(duk, 1));
            return 0;
        }, 2},
        {"disableAlphaBlending", [](ofxDuktape&) { ofDisableAlphaBlending(); return 0; }, 0},
        {"enableAlphaBlending", [](ofxDuktape&) { ofEnableAlphaBlending(); return 0; }, 0},
        {"color", [](ofxDuktape& duk) {
            //ofLogNotice("of.color")<<duk.getTop();
            //ofLogNotice("of.color")<<duk.toString(4)<<", " << duk.getType(5);
            switch(duk.getTop()-2) {
                case 4:
                    objectFromofColor(duk, ofColor(duk.getNumber(0),
                                                   duk.getNumber(1),
                                                   duk.getNumber(2),
                                                   duk.getNumber(3)));
                    return 1;
                case 3:
                    objectFromofColor(duk, ofColor(duk.getNumber(0),
                                                   duk.getNumber(1),
                                                   duk.getNumber(2)));
                    return 1;
                case 2:
                    objectFromofColor(duk, ofColor(duk.getNumber(0),
                                                   duk.getNumber(1)));
                    return 1;
                case 1:
                    if(duk.isObject(0)) {
                        objectFromofColor(duk, ofColorFromObject(duk, 0));
                    } else {
                        objectFromofColor(duk, ofColor(duk.getNumber(0)));
                    }
                    return 1;
            }
            return DUK_ERR_RANGE_ERROR;
        }, DUK_VARARGS},
        {"rectangle", [](ofxDuktape& duk) {
            objectFromofRectangle(duk, ofRectangle(duk.getNumber(0),
                                                   duk.getNumber(1),
                                                   duk.getNumber(2),
                                                   duk.getNumber(3)));
            return 1;
        }, 4},
        {"vec2", [](ofxDuktape& duk) {
            objectFromofVec2f(duk, ofVec2f(duk.getNumber(0), duk.getNumber(1)));
            return 1;
        }, 2},
        {"vec3", [](ofxDuktape& duk) {
            objectFromofVec3f(duk,
                              ofVec3f(duk.getNumber(0),
                                      duk.getNumber(1),
                                      duk.getNumber(2)));
            return 1;
        }, 3},
        {"drawBox", [](ofxDuktape& duk) {
            ofDrawBox(duk.getNumber(0),
                      duk.getNumber(1),
                      duk.getNumber(2),
                      duk.getNumber(3),
                      duk.getNumber(4),
                      duk.getNumber(5));
            return 0;
        }, 6},
        {"drawCone", [](ofxDuktape& duk) {
            ofDrawCone(duk.getNumber(0),
                       duk.getNumber(1),
                       duk.getNumber(2),
                       duk.getNumber(3),
                       duk.getNumber(4));
            return 0;
        }, 5},
        {"drawCylinder", [](ofxDuktape& duk) {
            ofDrawCylinder(duk.getNumber(0),
                           duk.getNumber(1),
                           duk.getNumber(2),
                           duk.getNumber(3),
                           duk.getNumber(4));
            return 0;
        }, 5},
        {"drawIcoSphere", [](ofxDuktape& duk) {
            ofDrawIcoSphere(duk.getNumber(0),
                            duk.getNumber(1),
                            duk.getNumber(2),
                            duk.getNumber(3));
            return 0;
        }, 4},
        {"drawSphere", [](ofxDuktape& duk) {
            ofDrawSphere(duk.getNumber(0),
                         duk.getNumber(1),
                         duk.getNumber(2),
                         duk.getNumber(3));
            return 0;
        }, 4},
        {"drawPlane", [](ofxDuktape& duk) {
            ofDrawPlane   (duk.getNumber(0),
                           duk.getNumber(1),
                           duk.getNumber(2),
                           duk.getNumber(3),
                           duk.getNumber(4));
            return 0;
        }, 5},
        {"drawAxis", [](ofxDuktape& duk) {
            ofDrawAxis(duk.getNumber(0));
            return 0;
        }, 1},
        {"drawRotationAxes", [](ofxDuktape& duk) {
            ofDrawRotationAxes(duk.getNumber(0));
            return 0;
        }, 1},
        {"drawGrid", [](ofxDuktape& duk) {
            ofDrawGrid();
            return 0;
        }, 0},
        {"drawArrow", [](ofxDuktape& duk) {
            ofDrawArrow(ofVec3fFromObject(duk, 0),
                        ofVec3fFromObject(duk, 1));
            return 0;
        }, 2},
        {"drawLine", [](ofxDuktape& duk) {
            switch (duk.getTop()-2) {
                case 6:
                    ofDrawLine(duk.getNumber(0),
                               duk.getNumber(1),
                               duk.getNumber(2),
                               duk.getNumber(3),
                               duk.getNumber(4),
                               duk.getNumber(5));
                    break;
                case 4:
                    ofDrawLine(duk.getNumber(0),
                               duk.getNumber(1),
                               duk.getNumber(2),
                               duk.getNumber(3));
                    break;
                case 2:
                    ofDrawLine(ofVec3fFromObject(duk, 0),
                               ofVec3fFromObject(duk, 1));
                    break;
                default:
                    return DUK_ERR_SYNTAX_ERROR;
            }
            return 0;
        }, DUK_VARARGS},
        {"drawBezier", [](ofxDuktape& duk) {
            switch (duk.getTop()-2) {
                case 12:
                    ofDrawBezier(duk.getNumber(0), duk.getNumber(1), duk.getNumber(2),
                                 duk.getNumber(3), duk.getNumber(4), duk.getNumber(5),
                                 duk.getNumber(6), duk.getNumber(7), duk.getNumber(8),
                                 duk.getNumber(9), duk.getNumber(10), duk.getNumber(11));
                    break;
                case 8:
                    ofDrawBezier(duk.getNumber(0), duk.getNumber(1),
                                 duk.getNumber(2), duk.getNumber(3),
                                 duk.getNumber(4), duk.getNumber(5),
                                 duk.getNumber(6), duk.getNumber(7));
                    break;
                case 4:
                {
                    ofVec3f a = ofVec3fFromObject(duk, 0),
                    b = ofVec3fFromObject(duk, 1),
                    c = ofVec3fFromObject(duk, 2),
                    d = ofVec3fFromObject(duk, 3);
                    ofDrawBezier(a.x, a.y, a.z,
                                 b.x, b.y, b.z,
                                 c.x, c.y, c.z,
                                 d.x, d.y, d.z);
                }
                    break;
                default:
                    return DUK_ERR_SYNTAX_ERROR;
            }
            return 0;
        }, DUK_VARARGS},
    });
    
    duk.putGlobalString("of");
    
    // Logger
    duk.putGlobalStringFunction("log", [&](ofxDuktape& duk) {
        ofLogNotice() << duk.safeToString(0); return 0;
    }, 1);
    
    return *bindings;
}
