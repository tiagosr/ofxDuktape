//
//  ofxDuktape.h
//  openFrameworks addon for interacting with the Duktape VM
//
//  (c)2015 Tiago Rezende (@tiagosr)
//

#ifndef __ofxDuktape__
#define __ofxDuktape__

#include "ofMain.h"
#include "duktape.h"

class ofxDuktape {
public:
    typedef int (*c_function)(ofxDuktape *duk, void* data);
    struct ErrorData {
        int code;
        string description;
    };
protected:
    duk_context* ctx;
public:
    ofxDuktape();
    // constructs an object as a thread of the first
    ofxDuktape(ofxDuktape *parent, bool newenv=false);
    virtual ~ofxDuktape();
    ofEvent<ErrorData> onFatalError;
    
    // triggers a round of garbage collection
    inline void gc() { duk_gc(ctx, 0); }
    
    // pops (ignores) the topmost argument in the stack
    inline void pop() { duk_pop(ctx); }
    // pops a given argument in the stack
    inline void pop(int count) { duk_pop_n(ctx, count); }
    // duplicates a given argument onto the topmost element at the stack
    inline void dup(int index) { duk_dup(ctx, index); }
    // duplicates the topmost argument in the stack
    inline void dupTop() { duk_dup_top(ctx); }
    // swaps arguments between two positions
    inline void swap(int a, int b) { duk_swap(ctx, a, b); }
    // swaps arguments between a given position and the top of the stack
    inline void swapTop(int other) { duk_swap_top(ctx, other); }
    // copies an argument to another, overwriting the second one
    inline void copy(int from, int to) { duk_copy(ctx, from, to); }
    
    // copies topmost count arguments in stack from one thread to another
    inline void xcopyTop(ofxDuktape* other, int count) {
        duk_xcopy_top(ctx, other->ctx, count);
    }
    
    // moves topmost count arguments from another thread to this thread
    inline void xmoveTop(ofxDuktape* other, int count) {
        duk_xmove_top(ctx, other->ctx, count);
    }
    
    // throws within the javascript stack; does not return
    inline void _throw() { duk_throw(ctx); }
    // throws an error message within the javascript thread; does not return
    inline void _error(duk_errcode_t errcode, const string& errmsg) { duk_error(ctx, errcode, errmsg.c_str()); }
    
    // concatenates a given amount of arguments into a string, put at the top of the stack
    inline void concat(int count) { duk_concat(ctx, count); }
    // moves the argument at the top of the stack to the position indicated,
    // shifting all arguments at index and after upwards.
    inline void insert(int index) { duk_insert(ctx, index); }
    // joins a given amount of values into a result string with a separator between each value
    inline void join(int count) { duk_join(ctx, count); }
    // removes white-space characters from both ends of the string at index
    inline void trim(int index) { duk_trim(ctx, index); }
    
    // calls the function at the top of the stack as a constructor
    inline void _new(int num_args) { duk_new(ctx, num_args); }
    // makes the given generator output the next iteration at the top of the stack (with optional value)
    inline bool _next(int index, bool get_value) { return duk_next(ctx, index, get_value); }
    // emits the enum structure for a given object in the stack
    inline void _enum(int obj_index, unsigned int flags) { duk_enum(ctx, obj_index, flags); }
    
    // gets the index for the top of the stack (or the argument count)
    inline int getTop() { return duk_get_top(ctx); }
    // gets the length of the given object in the stack (the result of object.length)
    inline int  getLength(int index) { return duk_get_length(ctx, index); }
    // checks if the stack has a certain amount of space available
    inline bool checkStack(int extra) { return duk_check_stack(ctx, extra); }
    // checks if there is space in the stack for at least the amount of objects for reaching top
    inline bool checkStackTop(int top) { return duk_check_stack_top(ctx, top); }
    // like checkStack(), but throwing an error if the space is not available
    inline void requireStack(int extra) { return duk_require_stack(ctx, extra); }
    // like checkStackTop(), but throwing an error if the space is not available
    inline void requireStackTop(int top) { return duk_require_stack_top(ctx, top); }
    // gets the current top index of the current stack frame
    inline int  getTopIndex() { return duk_get_top_index(ctx); }
    // gets the current top index of the current stack frame, throwing an error if there is no stack
    inline int  requireTopIndex() { return duk_require_top_index(ctx); }
    
    // gets the type of the argument at index
    inline int getType(int index) { return duk_get_type(ctx, index); }
    // gets the type mask of the argument at index
    inline unsigned getTypeMask(int index) { return duk_get_type_mask(ctx, index); }
    
    // checks if the argument at index is of a given type
    inline bool checkType(int index, int type) {
        return duk_check_type(ctx, index, type);
    }
    // checks if the argument at index matches a given type mask
    inline bool checkTypeMask(int index, unsigned int type_mask) {
        return duk_check_type_mask(ctx, index, type_mask);
    }
    // throws an error if the argument at index matches a given type mask
    inline void requireTypeMask(int index, unsigned int type_mask) {
        duk_require_type_mask(ctx, index, type_mask);
    }
    
    // checks if an index is valid in the current stack frame
    inline bool isValidIndex(int index) { return duk_is_valid_index(ctx, index); }
    // throws an error if the index is not valid in the current stack frame
    inline void requireValidIndex(int index) { duk_require_valid_index(ctx, index); }

    struct ReadEvent {
        ofxDuktape *duk;
        char* buffer;
        size_t length;
    };
    struct WriteEvent {
        ofxDuktape *duk;
        const char* buffer;
        size_t length;
    };
    struct PeekEvent {
        ofxDuktape *duk;
        size_t ret_length;
    };
    struct FlushDetachEvent {
        ofxDuktape *duk;
    };
    
    ofEvent<ReadEvent> onDebugRead;
    ofEvent<WriteEvent> onDebugWrite;
    ofEvent<PeekEvent> onDebugPeek;
    ofEvent<FlushDetachEvent> onDebugReadFlush, onDebugWriteFlush, onDebugDetach;
    
    // attach the debugger (which will respond through the events in the object)
    void attachDebugger();
    // detach the debugger
    void detachDebugger();
    // pulse the vm so that the debugger doesn't get blocked up
    inline void debuggerCooperate() { duk_debugger_cooperate(ctx); }
    
    // compresses the internal representation of the object at the given index
    inline void compact(int index) { duk_compact(ctx, index); }
    
    
    inline bool isArray(int index) { return duk_is_array(ctx, index); }
    inline bool isBoolean(int index) { return duk_is_boolean(ctx, index); }
    inline bool isBoundFunction(int index) { return duk_is_bound_function(ctx, index); }
    inline bool isBuffer(int index) { return duk_is_buffer(ctx, index); }
    inline bool isCFunction(int index) { return duk_is_c_function(ctx, index); }
    inline bool isCallable(int index) { return duk_is_callable(ctx, index); }
    inline bool isConstructorCall() { return duk_is_constructor_call(ctx); }
    inline bool isDynamicBuffer(int index) { return duk_is_dynamic_buffer(ctx, index); }
    inline bool isEcmascriptFunction(int index) { return duk_is_ecmascript_function(ctx, index); }
    inline bool isError(int index) { return duk_is_error(ctx, index); }
    inline bool isFixedBuffer(int index) { return duk_is_fixed_buffer(ctx, index); }
    inline bool isFunction(int index) { return duk_is_function(ctx, index); }
    inline bool isLightfunc(int index) { return duk_is_lightfunc(ctx, index); }
    inline bool isNaN(int index) { return duk_is_nan(ctx, index); }
    inline bool isNull(int index) { return duk_is_null(ctx, index); }
    inline bool isNullOrUndefined(int index) { return duk_is_null_or_undefined(ctx, index); }
    inline bool isNumber(int index) { return duk_is_number(ctx, index); }
    inline bool isObject(int index) { return duk_is_object(ctx, index); }
    inline bool isObjectCoercible(int index) { return duk_is_object_coercible(ctx, index); }
    inline bool isPointer(int index) { return duk_is_pointer(ctx, index); }
    inline bool isPrimitive(int index) { return duk_is_primitive(ctx, index); }
    inline bool isStrictCall() { return duk_is_strict_call(ctx); }
    inline bool isString(int index) { return duk_is_string(ctx, index); }
    inline bool isThread(int index) { return duk_is_thread(ctx, index); }
    inline bool isUndefined(int index) { return duk_is_undefined(ctx, index); }
    

    // pushes 'null' to the top of the stack
    inline void pushNull() { duk_push_null(ctx); }
    // pushes a NaN value to the top of the stack
    inline void pushNaN() { duk_push_nan(ctx); }
    // pushes an empty object to the top of the stack
    inline void pushObject() { duk_push_object(ctx); }
    // pushes an empty array to the top of the stack
    inline void pushArray() { duk_push_array(ctx); }
    // pushes a global object
    inline void pushGlobalObject() { duk_push_global_object(ctx); }
    inline void pushGlobalStash() { duk_push_global_stash(ctx); }
    inline void pushHeapStash() { duk_push_heap_stash(ctx); }
    inline void pushBool(bool val) { duk_push_boolean(ctx, val); }
    inline void pushString(const string& s) { duk_push_lstring(ctx, s.c_str(), s.length()); }
    inline void pushNumber(double n) { duk_push_number(ctx, n); }
    inline void pushInt(int n) { duk_push_int(ctx, n); }
    inline void pushUint(unsigned int n) { duk_push_uint(ctx, n); }
    inline void* pushFixedBuffer(size_t size) {
        return duk_push_buffer(ctx, size, false);
    }
    inline void* pushDynamicBuffer(size_t initial_size) {
        return duk_push_buffer(ctx, initial_size, true);
    }
    
    void pushCFunction(c_function func, int arguments, void* userdata);
    
    inline void pushHeapPtr(void* ptr) { duk_push_heapptr(ctx, ptr); }
    
    inline void toNull(int index) { duk_to_null(ctx, index); }
    inline void toUndefined(int index) { duk_to_undefined(ctx, index); }
    inline bool toBool(int index) { return duk_to_boolean(ctx, index); }
    inline void* toBuffer(int index, size_t* size) { return duk_to_buffer(ctx, index, size); }
    inline void* toFixedBuffer(int index, size_t* size) { return duk_to_fixed_buffer(ctx, index, size); }
    inline void* toDynamicBuffer(int index, size_t* size) { return duk_to_dynamic_buffer(ctx, index, size); }
    inline int toInt(int index) { return duk_to_int(ctx, index); }
    inline unsigned int toUint(int index) { return duk_to_uint(ctx, index); }
    inline int32_t toInt32(int index) { return duk_to_int32(ctx, index); }
    inline uint16_t toUint16(int index) { return duk_to_uint16(ctx, index); }
    inline double toNumber(int index) { return duk_to_number(ctx, index); }
    inline void toObject(int index) { duk_to_object(ctx, index); }
    inline void* toPointer(int index) { return duk_to_pointer(ctx, index); }
    inline string toString(int index) {
        size_t size;
        const char* str = duk_to_lstring(ctx, index, &size);
        return string(str, size);
    }
    inline void toDefaultValue(int index, int hint) { duk_to_defaultvalue(ctx, index, hint); }
    inline void toPrimitive(int index, int hint) { duk_to_primitive(ctx, index, hint); }
    
    inline void requireNull(int index) { duk_require_null(ctx, index); }
    inline void requireObjectCoercible(int index) { duk_require_object_coercible(ctx, index); }
    
    inline bool getBool(int index) { return duk_get_boolean(ctx, index); }
    inline bool requireBool(int index) { return duk_require_boolean(ctx, index); }
    inline int  getInt(int index)  { return duk_get_int(ctx, index); }
    inline int requireInt(int index) { return duk_require_int(ctx, index); }
    inline unsigned int getUint(int index) { return duk_get_uint(ctx, index); }
    inline unsigned int requireUint(int index) { return duk_require_uint(ctx, index); }
    inline double getNumber(int index) { return duk_get_number(ctx, index); }
    inline double requireNumber(int index) { return duk_require_number(ctx, index); }
    inline string getString(int index) {
        size_t length;
        const char* str = duk_get_lstring(ctx, index, &length);
        return string(str, length);
    }
    inline string requireString(int index) {
        size_t length;
        const char* str = duk_require_lstring(ctx, index, &length);
        return string(str, length);
    }
    inline void* getBuffer(int index, size_t* out_size) {
        return duk_get_buffer(ctx, index, out_size);
    }
    inline void* requireBuffer(int index, size_t* out_size) {
        return duk_require_buffer(ctx, index, out_size);
    }
    inline void* getHeapPtr(int index) { return duk_get_heapptr(ctx, index); }
    inline void* requireHeapPtr(int index) { return duk_require_heapptr(ctx, index); }
    inline void* getPointer(int index) { return duk_get_pointer(ctx, index); }
    inline void* requirePointer(int index) { return duk_require_pointer(ctx, index); }
    
    inline void* resizeBuffer(int index, size_t new_size) {
        return duk_resize_buffer(ctx, index, new_size);
    }
    
    inline void getProp(int obj_index) { return duk_get_prop(ctx, obj_index); }
    inline void getPropIndex(int obj_index, int arr_index) {
        duk_get_prop_index(ctx, obj_index, arr_index);
    }
    inline void getPropString(int obj_index, const string& key) {
        duk_get_prop_string(ctx, obj_index, key.c_str());
    }
    inline void getPrototype(int obj_index) { duk_get_prototype(ctx, obj_index); }
    inline bool hasProp(int obj_index) {
        return duk_has_prop(ctx, obj_index);
    }
    inline bool hasPropIndex(int obj_index, int prop_index) {
        return duk_has_prop_index(ctx, obj_index, prop_index);
    }
    inline bool hasPropString(int obj_index, const string& key) {
        return duk_has_prop_string(ctx, obj_index, key.c_str());
    }
    
    inline void putGlobalString(const string& s) { duk_put_global_string(ctx, s.c_str()); }
    inline bool getGlobalString(const string& key) {
        return duk_get_global_string(ctx, key.c_str());
    }
    
    inline void setGlobalObject() { duk_set_global_object(ctx); }
    inline void setMagic(int index, int magic) { duk_set_magic(ctx, index, magic); }
    inline void setPrototype(int index) { duk_set_prototype(ctx, index); }
    inline void setTop(int index) { duk_set_top(ctx, index); }
    
    inline void defineProperty(int obj_index, unsigned int flags) { duk_def_prop(ctx, obj_index, flags); }
    inline bool deleteProperty(int obj_index) { return duk_del_prop(ctx, obj_index); }
    inline bool deletePropertyIndex(int obj_index, unsigned int array_index) {
        return duk_del_prop_index(ctx, obj_index, array_index);
    }
    inline bool deletePropertyString(int obj_index, string key) {
        return duk_del_prop_string(ctx, obj_index, key.c_str());
    }
    
    inline bool equals(int index1, int index2) {
        return duk_equals(ctx, index1, index2);
    }
    
    inline bool strictEquals(int index1, int index2) {
        return duk_strict_equals(ctx, index1, index2);
    }
    
    inline void substring(int index, size_t start, size_t end) {
        duk_substring(ctx, index, start, end);
    }
    
    inline void call(int num_arguments) {
        duk_call(ctx, num_arguments);
    }
    inline void callMethod(int num_arguments) {
        duk_call_method(ctx, num_arguments);
    }
    inline void callProp(int obj_index, int num_arguments) {
        duk_call_prop(ctx, obj_index, num_arguments);
    }
    inline void eval() { duk_eval(ctx); }
    inline void evalNoResult() { duk_eval_noresult(ctx); }
    inline void evalString(const string& s) {
        duk_eval_lstring(ctx, s.c_str(), s.length());
    }
    inline void evalStringNoResult(const string& s) {
        duk_eval_lstring_noresult(ctx, s.c_str(), s.length());
    }
    inline void compile(unsigned int flags) {
        duk_compile(ctx, flags);
    }
    
    inline int pCall(int num_arguments) {
        return duk_pcall(ctx, num_arguments);
    }
    inline int pCallMethod(int num_arguments) {
        return duk_pcall_method(ctx, num_arguments);
    }
    inline int pCallProp(int obj_index, int num_arguments) {
        return duk_pcall_prop(ctx, obj_index, num_arguments);
    }
    inline int pCompile(unsigned int flags) {
        return duk_pcompile(ctx, flags);
    }
    inline int pCompileString(const string& s, unsigned int flags) {
        return duk_pcompile_lstring(ctx, flags, s.c_str(), s.length());
    }
    int pCompileStringFilename(const string& filename, string s, unsigned int flags) {
        pushString(filename);
        return duk_pcompile_lstring_filename(ctx, flags, s.c_str(), s.length());
    }
    inline int pEval() { return duk_peval(ctx); }
    inline int pEvalNoResult() { return duk_peval_noresult(ctx); }
    inline int pEvalString(const string& s)  {
        return duk_peval_lstring(ctx, s.c_str(), s.length());
    }
    
    
};


#endif /* defined(__ofxDuktape__) */
