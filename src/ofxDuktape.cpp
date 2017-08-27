//
//  ofxDuktape.cpp
//  openFrameworks addon for interacting with the Duktape VM
//
//  (c)2015 Tiago Rezende (@tiagosr)
//

#include "ofxDuktape.h"

const char* ofxDuktapeProp = "\xff""ofxDuktape";
static void* ofxDuktapeMalloc(ofxDuktape* duk, duk_size_t size) {
    return malloc(size);
}
static void* ofxDuktapeRealloc(ofxDuktape* duk, void* ptr, duk_size_t size) {
    return realloc(ptr, size);
}
static void ofxDuktapeFree(ofxDuktape* duk, void* ptr) {
    return free(ptr);
}
static void ofxDuktapeFatal(duk_context *ctx, duk_errcode_t code, const char* msg) {
    duk_memory_functions mem;
    duk_get_memory_functions(ctx, &mem);
    
}

void ofxDuktape::threadSetup() {
    duk_idx_t thread = duk_push_thread(ctx);
    putObjectHeapPtr(thread, ofxDuktapeProp, (void*)this);
    setFinalizerFunction(thread, [](ofxDuktape&duk) {
        ofxDuktape *dduk = static_cast<ofxDuktape*>(duk.getObjectHeapPtr(0, ofxDuktapeProp));
        if(dduk) delete dduk;
        duk.putObjectHeapPtr(0, ofxDuktapeProp, 0);
        return 0;
    });
    pop();
}

duk_idx_t ofxDuktape::pushThread() {
    duk_idx_t thread = duk_push_thread(ctx);
    duk_context *octx = duk_get_context(ctx, thread);
    if(octx) {
        if(!hasPropString(thread, ofxDuktapeProp)) {
            ofxDuktape *duk = new ofxDuktape(this, octx);
            putObjectHeapPtr(thread, ofxDuktapeProp, duk);
        }
    }
    return thread;
}
ofxDuktape::ofxDuktape(): ctx(NULL) {
    ctx = duk_create_heap((duk_alloc_function)ofxDuktapeMalloc,
                          (duk_realloc_function)ofxDuktapeRealloc,
                          (duk_free_function)ofxDuktapeFree,
                          (void*)this,
                          (duk_fatal_function)ofxDuktapeFatal);
    threadSetup();
}

ofxDuktape::ofxDuktape(ofxDuktape*parent, bool newenv) {
    if(parent && parent->ctx) {
        if(newenv){
            duk_push_thread_new_globalenv(parent->ctx);
        } else {
            duk_push_thread(parent->ctx);
        }
        ctx = duk_get_context(parent->ctx, -1);
    } else {
        ctx = duk_create_heap((duk_alloc_function)ofxDuktapeMalloc,
                              (duk_realloc_function)ofxDuktapeRealloc,
                              (duk_free_function)ofxDuktapeFree,
                              (void*)this,
                              (duk_fatal_function)ofxDuktapeFatal);
    }
    threadSetup();
}
ofxDuktape::ofxDuktape(ofxDuktape*parent, duk_context *other_ctx) {
    ctx = other_ctx;
    threadSetup();
}

ofxDuktape::~ofxDuktape() {
    duk_idx_t thread = pushThread();
    // clear internal pointer to avoid double-freeing oneself
    putObjectHeapPtr(thread, ofxDuktapeProp, 0);
    duk_destroy_heap(ctx);
}

class ofxDuktapeCPPFunctionWrapper {
public:
    ofxDuktapeCPPFunctionWrapper(ofxDuktape::cpp_function func): func(func) {}
    ~ofxDuktapeCPPFunctionWrapper(){}
    ofxDuktape::cpp_function func;
    
};


const char* ofxDuktapeSpecialFnPtr = "\xff" "ofxDuktape_fn";
const char* ofxDuktapeSpecialCtxPtr = "\xff" "ofxDuktape_ctx";
const char* ofxDuktapeSpecialUserPtr = "\xff" "ofxDuktape_user";

static duk_ret_t ofxDuktapeCPPFunctionWrapperFinalizer(duk_context *ctx) {
    duk_get_prop_string(ctx, -1, ofxDuktapeSpecialFnPtr);
    ofxDuktapeCPPFunctionWrapper *wrapper = (ofxDuktapeCPPFunctionWrapper*)duk_get_pointer(ctx, -1);
    delete wrapper;
    duk_pop(ctx);
    return 0;
}

static duk_ret_t internal_cpp_function_call(duk_context *ctx) {
    duk_push_current_function(ctx);
    duk_get_prop_string(ctx, -1, ofxDuktapeSpecialFnPtr);
    duk_get_prop_string(ctx, -2, ofxDuktapeSpecialCtxPtr);
    ofxDuktapeCPPFunctionWrapper *wrapper = (ofxDuktapeCPPFunctionWrapper*)duk_get_pointer(ctx, -2);
    ofxDuktape* duk = (ofxDuktape *)duk_get_pointer(ctx, -1);
    duk_pop(ctx);
    return wrapper->func(*duk);
}

static duk_ret_t internal_c_function_call(duk_context *ctx) {
    duk_push_current_function(ctx);
    duk_get_prop_string(ctx, -1, ofxDuktapeSpecialFnPtr);
    duk_get_prop_string(ctx, -2, ofxDuktapeSpecialCtxPtr);
    duk_get_prop_string(ctx, -3, ofxDuktapeSpecialUserPtr);
    ofxDuktape::c_function fn = (ofxDuktape::c_function)duk_get_pointer(ctx, -3);
    ofxDuktape* context = (ofxDuktape*)duk_get_pointer(ctx, -2);
    void* user = duk_get_pointer(ctx, -1);
    duk_pop(ctx);
    return fn(context, user);
}

void ofxDuktape::pushCFunction(c_function func, int arguments, void* userdata) {
    duk_push_c_function(ctx, internal_c_function_call, arguments);
    duk_push_pointer(ctx, (void*)func);
    duk_put_prop_string(ctx, -2, ofxDuktapeSpecialFnPtr);
    //duk_pop(ctx);
    duk_push_pointer(ctx, (void*)this);
    duk_put_prop_string(ctx, -2, ofxDuktapeSpecialCtxPtr);
    //duk_pop(ctx);
    duk_push_pointer(ctx, userdata);
    duk_put_prop_string(ctx, -2, ofxDuktapeSpecialUserPtr);
    duk_pop(ctx);
}

void ofxDuktape::pushFunction(cpp_function func, int arguments) {
    duk_push_c_function(ctx, internal_cpp_function_call, arguments);
    ofxDuktapeCPPFunctionWrapper *wrapper = new ofxDuktapeCPPFunctionWrapper(func);
    duk_push_pointer(ctx, (void*)wrapper);
    duk_put_prop_string(ctx, -2, ofxDuktapeSpecialFnPtr);
    //duk_pop(ctx);
    duk_push_pointer(ctx, (void*)this);
    duk_put_prop_string(ctx, -2, ofxDuktapeSpecialCtxPtr);
    //duk_pop(ctx);
    duk_push_c_function(ctx, ofxDuktapeCPPFunctionWrapperFinalizer, 1);
    duk_set_finalizer(ctx, -2);
}

static duk_ret_t ofxDuktapeSafeCallFunc(duk_context *ctx, void * udata) {
    if(duk_get_global_string(ctx, ofxDuktapeProp)) {
        ofxDuktape *duk = (ofxDuktape*)duk_get_pointer(ctx, -1);
        ofxDuktapeCPPFunctionWrapper *wrapper = (ofxDuktapeCPPFunctionWrapper *)duk_get_pointer(ctx, -1);
        if(wrapper) {
            return wrapper->func(*duk);
        }
    }
    return -1;
}

duk_ret_t ofxDuktape::safeCall(cpp_function func, int arguments, int rets) {
    shared_ptr<ofxDuktapeCPPFunctionWrapper> wrapper = make_shared<ofxDuktapeCPPFunctionWrapper>(func);
    duk_push_pointer(ctx, wrapper.get());
    return duk_safe_call(ctx, ofxDuktapeSafeCallFunc, NULL, arguments+1, rets);
}

static duk_size_t ofxDuktapeDebugReadCB(void* data, char* buffer, duk_size_t length) {
    ofxDuktape::ReadEvent ev;
    ev.duk = (ofxDuktape*)data;
    ev.buffer = buffer;
    ev.length = length;
    ofNotifyEvent(ev.duk->onDebugRead, ev);
    return ev.length;
}

static duk_size_t ofxDuktapeDebugWriteCB(void* data, const char*buffer, duk_size_t length) {
    ofxDuktape::WriteEvent ev;
    ev.duk = (ofxDuktape*)data;
    ev.buffer = buffer;
    ev.length = length;
    ofNotifyEvent(ev.duk->onDebugWrite, ev);
    return ev.length;
}

static duk_size_t ofxDuktapeDebugPeekCB(void* data) {
    ofxDuktape::PeekEvent ev;
    ev.duk = (ofxDuktape*)data;
    ev.ret_length = 0;
    ofNotifyEvent(ev.duk->onDebugPeek, ev);
    return ev.ret_length;
}

static void ofxDuktapeDebugReadFlushCB(void* data) {
    ofxDuktape::FlushDetachEvent ev;
    ev.duk = (ofxDuktape *) data;
    ofNotifyEvent(ev.duk->onDebugReadFlush, ev);
}
static void ofxDuktapeDebugWriteFlushCB(void* data) {
    ofxDuktape::FlushDetachEvent ev;
    ev.duk = (ofxDuktape *) data;
    ofNotifyEvent(ev.duk->onDebugWriteFlush, ev);
}

static duk_idx_t ofxDuktapeDebugRequestCB(duk_context *duk, void* data, duk_idx_t num_values) {
    ofxDuktape::DebugRequestEvent ev;
    ev.duk = (ofxDuktape *) data;
    ev.n_values = num_values;
    ev.ret_n_values = 0;
    ofNotifyEvent(ev.duk->onDebugRequest, ev);
    return ev.ret_n_values;
}

static void ofxDuktapeDebugDetachCB(duk_context *duk, void* data) {
    ofxDuktape::FlushDetachEvent ev;
    ev.duk = (ofxDuktape *) data;
    ofNotifyEvent(ev.duk->onDebugDetach, ev);
}



void ofxDuktape::attachDebugger() {
    duk_debugger_attach(ctx,
                        ofxDuktapeDebugReadCB,
                        ofxDuktapeDebugWriteCB,
                        ofxDuktapeDebugPeekCB,
                        ofxDuktapeDebugReadFlushCB,
                        ofxDuktapeDebugWriteFlushCB,
                        ofxDuktapeDebugRequestCB,
                        ofxDuktapeDebugDetachCB,
                        this);
}

void ofxDuktape::detachDebugger() {
    duk_debugger_detach(ctx);
}
