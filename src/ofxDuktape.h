//
//  ofxDuktape.h
//  openFrameworks addon for interacting with the Duktape VM
//
//  (c)2015 Tiago Rezende (@tiagosr)
//

#pragma once

#define DUK_USE_CPP_EXCEPTIONS
#include "ofMain.h"
#include "duktape.h"
#include <initializer_list>
#include <tuple>

class ofxDuktape {
public:
    typedef int (*c_function)(ofxDuktape *duk, void* data);
    typedef function<int(ofxDuktape&)> cpp_function;
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
    ofxDuktape(ofxDuktape *parent, duk_context *other_ctx);
    virtual ~ofxDuktape();
    ofEvent<ErrorData> onFatalError;
    
    void threadSetup();
    
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
    
    // normalizes index relative to the bottom of the current frame
    inline duk_idx_t normalizeIndex(duk_idx_t index) { return duk_normalize_index(ctx, index); }
    inline duk_idx_t requireNormalizeIndex(duk_idx_t index) { return duk_require_normalize_index(ctx, index); }
    
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
    inline void concat(duk_idx_t count) { duk_concat(ctx, count); }
    // moves the argument at the top of the stack to the position indicated,
    // shifting all arguments at index and after upwards.
    inline void insert(duk_idx_t index) { duk_insert(ctx, index); }
    // joins a given amount of values into a result string with a separator between each value
    inline void join(duk_idx_t count) { duk_join(ctx, count); }
    // removes white-space characters from both ends of the string at index
    inline void trim(duk_idx_t index) { duk_trim(ctx, index); }
    
    // calls the function at the top of the stack as a constructor
    inline void _new(duk_idx_t num_args) { duk_new(ctx, num_args); }
    // makes the given generator output the next iteration at the top of the stack (with optional value)
    inline bool _next(duk_idx_t index, bool get_value) { return duk_next(ctx, index, get_value); }
    // emits the enum structure for a given object in the stack
    inline void _enum(duk_idx_t obj_index, unsigned int flags) { duk_enum(ctx, obj_index, flags); }
    
    // gets the index for the top of the stack (or the argument count)
    inline int getTop() { return duk_get_top(ctx); }
    // gets the length of the given object in the stack (the result of object.length)
    inline int  getLength(duk_idx_t index) { return duk_get_length(ctx, index); }
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
    // ensures the object at index in the stack frame is a callable object
    inline void requireCallable(duk_idx_t index) { duk_require_callable(ctx, index); }
    // gets the heap pointer at index, throwing an error if the object is not a heap pointer
    
    
    // gets the type of the argument at index
    inline int getType(duk_idx_t index) { return duk_get_type(ctx, index); }
    // gets the type mask of the argument at index
    inline unsigned getTypeMask(duk_idx_t index) { return duk_get_type_mask(ctx, index); }
    
    // checks if the argument at index is of a given type
    inline bool checkType(duk_idx_t index, int type) {
        return duk_check_type(ctx, index, type);
    }
    // checks if the argument at index matches a given type mask
    inline bool checkTypeMask(duk_idx_t index, unsigned int type_mask) {
        return duk_check_type_mask(ctx, index, type_mask);
    }
    // throws an error if the argument at index matches a given type mask
    inline void requireTypeMask(duk_idx_t index, unsigned int type_mask) {
        duk_require_type_mask(ctx, index, type_mask);
    }
    
    // checks if an index is valid in the current stack frame
    inline bool isValidIndex(duk_idx_t index) { return duk_is_valid_index(ctx, index); }
    // throws an error if the index is not valid in the current stack frame
    inline void requireValidIndex(duk_idx_t index) { duk_require_valid_index(ctx, index); }
    
    //
    inline bool next(duk_idx_t enum_index, bool get_value = true) {
        return duk_next(ctx, enum_index, get_value);
    }
    
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
	struct DebugRequestEvent {
		ofxDuktape *duk;
		int n_values;
		int ret_n_values;
	};
    
    ofEvent<ReadEvent> onDebugRead;
    ofEvent<WriteEvent> onDebugWrite;
    ofEvent<PeekEvent> onDebugPeek;
    ofEvent<FlushDetachEvent> onDebugReadFlush, onDebugWriteFlush, onDebugDetach;
	ofEvent<DebugRequestEvent> onDebugRequest;
    
    // attach the debugger (which will respond through the events in the object)
    void attachDebugger();
    // detach the debugger
    void detachDebugger();
    // pulse the vm so that the debugger doesn't get blocked up
    inline void debuggerCooperate() { duk_debugger_cooperate(ctx); }
    
    // compresses the internal representation of the object at the given index
    inline void compact(duk_idx_t index) { duk_compact(ctx, index); }
    
    
    inline bool isArray(duk_idx_t index) { return duk_is_array(ctx, index); }
    inline bool isBoolean(duk_idx_t index) { return duk_is_boolean(ctx, index); }
    inline bool isBoundFunction(duk_idx_t index) { return duk_is_bound_function(ctx, index); }
    inline bool isBuffer(duk_idx_t index) { return duk_is_buffer(ctx, index); }
    inline bool isCFunction(duk_idx_t index) { return duk_is_c_function(ctx, index); }
    inline bool isCallable(duk_idx_t index) { return duk_is_callable(ctx, index); }
    inline bool isConstructorCall() { return duk_is_constructor_call(ctx); }
    inline bool isDynamicBuffer(duk_idx_t index) { return duk_is_dynamic_buffer(ctx, index); }
    inline bool isEcmascriptFunction(duk_idx_t index) { return duk_is_ecmascript_function(ctx, index); }
    inline bool isError(duk_idx_t index) { return duk_is_error(ctx, index); }
    inline bool isFixedBuffer(duk_idx_t index) { return duk_is_fixed_buffer(ctx, index); }
    inline bool isFunction(duk_idx_t index) { return duk_is_function(ctx, index); }
    inline bool isLightfunc(duk_idx_t index) { return duk_is_lightfunc(ctx, index); }
    inline bool isNaN(duk_idx_t index) { return duk_is_nan(ctx, index); }
    inline bool isNull(duk_idx_t index) { return duk_is_null(ctx, index); }
    inline bool isNullOrUndefined(duk_idx_t index) { return duk_is_null_or_undefined(ctx, index); }
    inline bool isNumber(duk_idx_t index) { return duk_is_number(ctx, index); }
    inline bool isObject(duk_idx_t index) { return duk_is_object(ctx, index); }
    inline bool isObjectCoercible(duk_idx_t index) { return duk_is_object_coercible(ctx, index); }
    inline bool isPointer(duk_idx_t index) { return duk_is_pointer(ctx, index); }
    inline bool isPrimitive(duk_idx_t index) { return duk_is_primitive(ctx, index); }
    inline bool isStrictCall() { return duk_is_strict_call(ctx); }
    inline bool isString(duk_idx_t index) { return duk_is_string(ctx, index); }
    inline bool isThread(duk_idx_t index) { return duk_is_thread(ctx, index); }
    inline bool isUndefined(duk_idx_t index) { return duk_is_undefined(ctx, index); }
    
    inline bool isRangeError(duk_idx_t index) { return duk_is_range_error(ctx, index); }
    inline bool isReferenceError(duk_idx_t index) { return duk_is_reference_error(ctx, index); }
    inline bool isSyntaxError(duk_idx_t index) { return duk_is_syntax_error(ctx, index); }
    inline bool isEvalError(duk_idx_t index) { return duk_is_eval_error(ctx, index); }
    inline bool isTypeError(duk_idx_t index) { return duk_is_type_error(ctx, index); }
    inline bool isURIError(duk_idx_t index) { return duk_is_uri_error(ctx, index); }
    
    // pushed 'undefined' to the top of the stack
    inline void pushUndefined() { duk_push_undefined(ctx); }
    // pushes 'null' to the top of the stack
    inline void pushNull() { duk_push_null(ctx); }
    // pushes a NaN value to the top of the stack
    inline void pushNaN() { duk_push_nan(ctx); }
    // pushes the "this" object
    inline void pushThis() { duk_push_this(ctx); }
    // pushes a bare object (with null prototype)
    inline duk_idx_t pushBareObject() { return duk_push_bare_object(ctx); }
    // pushes an empty object to the top of the stack
    inline duk_idx_t pushObject() { return duk_push_object(ctx); }
    // pushes an empty array to the top of the stack
    inline duk_idx_t pushArray() { return duk_push_array(ctx); }
    // pushes the global object to the top of the stack
    inline void pushGlobalObject() { duk_push_global_object(ctx); }
    // pushes the global stash to the top of the stack
    inline void pushGlobalStash() { duk_push_global_stash(ctx); }
    // pushes the heap stash to the top of the stack
    inline void pushHeapStash() { duk_push_heap_stash(ctx); }
    // pushes a boolean value to the top of the stack
    inline void pushBool(bool val) { duk_push_boolean(ctx, val); }
    // pushes the 'true' value to the top of the stack
    inline void pushTrue() { duk_push_true(ctx); }
    // pushes the 'false' value to the top of the stack
    inline void pushFalse() { duk_push_false(ctx); }
    // pushes a string value to the top of the stack
    inline void pushString(const string& s) { duk_push_lstring(ctx, s.c_str(), s.length()); }
    // pushes a number value to the top of the stack
    inline void pushNumber(double n) { duk_push_number(ctx, n); }
    // pushes an integer number value to the top of the stack
    inline void pushInt(duk_int_t n) { duk_push_int(ctx, n); }
    // pushes an unsigned integer number value to the top of the stack
    inline void pushUint(duk_uint_t n) { duk_push_uint(ctx, n); }
    // pushes an empty fixed buffer with a given size to the top of the stack
    inline void* pushFixedBuffer(size_t size) {
        return duk_push_buffer(ctx, size, false);
    }
    // pushes an empty resizable buffer with a given starting size to the top of the stack
    inline void* pushDynamicBuffer(size_t initial_size) {
        return duk_push_buffer(ctx, initial_size, true);
    }
    // pushes an external buffer to the top of the stack
    inline void pushExternalBuffer(void* ptr, size_t len) {
        duk_push_external_buffer(ctx);
        duk_config_buffer(ctx, -1, ptr, len);
    }
    // pushes a buffer object or a buffer view object, with data from the buffer object
    inline void pushBufferObject(duk_idx_t buf, duk_size_t offset, duk_size_t byte_length, duk_uint_t flags) {
        duk_push_buffer_object(ctx, buf, offset, byte_length, flags);
    }
    // pushes a C function with a user-given pointer to the top of the stack
    void pushCFunction(c_function func, int arguments, void* userdata);
    
    // pushes a C++ function (with a single argument)
    void pushFunction(cpp_function func, int arguments);
    
    // pushes a pointer to an in-heap object into the top of the stack
    inline void pushHeapPtr(void* ptr) {
        duk_push_heapptr(ctx, ptr);
    }

    // pushes a pointer to an outside object into the top of the stack
    inline void pushPointer(void* ptr) { duk_push_pointer(ctx, ptr); }

    // pushes current function object into the top of the stack
    inline void pushCurrentFunction() { duk_push_current_function(ctx); }
    
    // pushes a new thread (along with an ofxDuktape context)
    duk_idx_t pushThread();

    // pushes the current running thread to the stack
    inline void pushCurrentThread() { duk_push_current_thread(ctx); }

    // pushes the thread stash related to the thread in the stack
    void pushThreadStash(duk_idx_t thread_index) {
        duk_context *thread_ctx = duk_get_context(ctx, thread_index);
        duk_push_thread_stash(ctx, thread_ctx);
    }

    void pushCurrentThreadStash() {
        duk_push_thread_stash(ctx, ctx);
    }


    // pushes a short description of the current context to the stack
    inline void pushContextDump() { duk_push_context_dump(ctx); }
    
    // gets a string with the current context dump
    inline string getContextDump() {
        pushContextDump();
        return toString(-1);
    }
    
    
    // safe getters
    
    // gets an argument and casts to string
    inline string safeToString(duk_idx_t index) { return duk_safe_to_string(ctx, index); }
    
    inline duk_ret_t safeCall(cpp_function func, int arguments, int rets);
    
    // sets an argument into null
    inline void toNull(duk_idx_t index) { duk_to_null(ctx, index); }
    // sets an argument into undefined
    inline void toUndefined(duk_idx_t index) { duk_to_undefined(ctx, index); }
    // tries coercing an argument into a boolean value
    inline bool toBool(duk_idx_t index) { return duk_to_boolean(ctx, index); }
    // tries coercing an argument into a buffer value
    inline void* toBuffer(duk_idx_t index, size_t* size) { return duk_to_buffer(ctx, index, size); }
    // tries coercing an argument into a fixed buffer value
    inline void* toFixedBuffer(duk_idx_t index, size_t* size) { return duk_to_fixed_buffer(ctx, index, size); }
    // tries coercing an argument into a dynamic buffer value
    inline void* toDynamicBuffer(duk_idx_t index, size_t* size) { return duk_to_dynamic_buffer(ctx, index, size); }
    // tries coercing an argument into an integer
    inline int toInt(duk_idx_t index) { return duk_to_int(ctx, index); }
    // tries coercing an argument into an unsigned integer
    inline unsigned int toUint(duk_idx_t index) { return duk_to_uint(ctx, index); }
    // tries coercing an argument into a 32-bit integer
    inline int32_t toInt32(duk_idx_t index) { return duk_to_int32(ctx, index); }
    // tries coercing an argument into a 16-bit unsigned integer
    inline uint16_t toUint16(duk_idx_t index) { return duk_to_uint16(ctx, index); }
    // tries coercing an argument into a number
    inline double toNumber(duk_idx_t index) { return duk_to_number(ctx, index); }
    // tries coercing an argument into an object
    inline void toObject(duk_idx_t index) { duk_to_object(ctx, index); }
    // tries coercing an argument into a pointer
    inline void* toPointer(duk_idx_t index) { return duk_to_pointer(ctx, index); }
    // tries coercing an argument into a string
    inline string toString(duk_idx_t index) {
        size_t size;
        const char* str = duk_to_lstring(ctx, index, &size);
        return string(str, size);
    }
    // tries coercing an argument into it's primitive type
    inline void toPrimitive(duk_idx_t index, int hint) { duk_to_primitive(ctx, index, hint); }
    
    // requires the presence of null in the given argument (throws an error otherwise)
    inline void requireNull(duk_idx_t index) { duk_require_null(ctx, index); }
    // requires the presence of a value that can be converted into an object
    // in the given argument (throws an error otherwise)
    inline void requireObjectCoercible(duk_idx_t index) { duk_require_object_coercible(ctx, index); }
    // requires the presence of a function (not only a callable object)
    inline void requireFunction(duk_idx_t index) { duk_require_function(ctx, index); }
    // requires the presence of a context/thread
    inline void requireContext(duk_idx_t index) { duk_require_context(ctx, index); }
    
    inline bool getBool(duk_idx_t index) { return duk_get_boolean(ctx, index); }
	inline bool getBool(duk_idx_t index, bool _default) { return duk_get_boolean_default(ctx, index, _default); }
    inline bool requireBool(duk_idx_t index) { return duk_require_boolean(ctx, index); }
    inline bool optionalBool(duk_idx_t index, bool _default) { return duk_opt_boolean(ctx, index, _default); }
    
    inline int  getInt(duk_idx_t index)  { return duk_get_int(ctx, index); }
	inline int  getInt(duk_idx_t index, int _default)  { return duk_get_int_default(ctx, index, _default); }
	inline int requireInt(duk_idx_t index) { return duk_require_int(ctx, index); }
    inline int optionalInt(duk_int_t index, int _default) { return duk_opt_int(ctx, index, _default); }
    
    inline unsigned int getUint(duk_idx_t index) { return duk_get_uint(ctx, index); }
	inline unsigned int getUint(duk_idx_t index, unsigned int _default) { return duk_get_uint_default(ctx, index, _default); }
	inline unsigned int requireUint(duk_idx_t index) { return duk_require_uint(ctx, index); }
    inline unsigned int optionalUInt(duk_int_t index, unsigned int _default) { return duk_opt_uint(ctx, index, _default); }
    
    inline double getNumber(duk_idx_t index) { return duk_get_number(ctx, index); }
	inline double getNumber(duk_idx_t index, double _default) { return duk_get_number_default(ctx, index, _default); }
	inline double requireNumber(duk_idx_t index) { return duk_require_number(ctx, index); }
    inline double optionalNumber(duk_idx_t index, double _default) { return duk_opt_number(ctx, index, _default); }
    
    inline string getString(duk_idx_t index) {
        size_t length;
        const char* str = duk_get_lstring(ctx, index, &length);
        return string(str, length);
    }
	inline string getString(duk_idx_t index, const string& _default) {
		size_t length;
		const char* str = duk_get_lstring_default(ctx, index, &length, _default.c_str(), _default.length());
		return string(str, length);
	}
    inline string requireString(duk_idx_t index) {
        size_t length;
        const char* str = duk_require_lstring(ctx, index, &length);
        return string(str, length);
    }
    inline string optionalString(duk_idx_t index, const string& _default) {
        size_t length;
        const char* str = duk_opt_lstring(ctx, index, &length, _default.c_str(), _default.length());
        return string(str, length);
    }
    
    inline void* getBuffer(duk_idx_t index, size_t& out_size) {
        return duk_get_buffer(ctx, index, &out_size);
    }
    inline void* getBuffer(duk_idx_t index, size_t& out_size, void* default_buffer, size_t default_buffer_size) {
        return duk_get_buffer_default(ctx, index, &out_size, default_buffer, default_buffer_size);
    }
    inline void* requireBuffer(duk_idx_t index, size_t& out_size) {
        return duk_require_buffer(ctx, index, &out_size);
    }
    inline void* optionalBuffer(duk_idx_t index, size_t& out_size, void* default_buffer, size_t default_buffer_size) {
        return duk_opt_buffer(ctx, index, &out_size, default_buffer, default_buffer_size);
    }

    inline void* getBufferData(duk_idx_t index, size_t& out_size) {
        return duk_get_buffer_data(ctx, index, &out_size);
    }
    inline void* getBufferData(duk_idx_t index, size_t& out_size, void* default_buffer, size_t default_buffer_size) {
        return duk_get_buffer_data_default(ctx, index, &out_size, default_buffer, default_buffer_size);
    }
    inline void* requireBufferData(duk_idx_t index, size_t& out_size) {
        return duk_require_buffer_data(ctx, index, &out_size);
    }
    inline void* optionalBufferData(duk_idx_t index, size_t& out_size, void* default_buffer, size_t default_buffer_size) {
        return duk_opt_buffer_data(ctx, index, &out_size, default_buffer, default_buffer_size);
    }

    inline void* getHeapPtr(duk_idx_t index) { return duk_get_heapptr(ctx, index); }
    inline void* getHeapPtr(duk_idx_t index, void* _default) { return duk_get_heapptr_default(ctx, index, _default); }
    inline void* requireHeapPtr(duk_idx_t index) { return duk_require_heapptr(ctx, index); }
    
    inline void* getPointer(duk_idx_t index) { return duk_get_pointer(ctx, index); }
    inline void* getPointer(duk_idx_t index, void* _default) { return duk_get_pointer_default(ctx, index, _default); }
    inline void* requirePointer(duk_idx_t index) { return duk_require_pointer(ctx, index); }
    inline void* optionalPointer(duk_idx_t index, void* _default) { return duk_opt_pointer(ctx, index, _default); }
    
    inline void* resizeBuffer(duk_idx_t index, size_t new_size) {
        return duk_resize_buffer(ctx, index, new_size);
    }
    
    inline bool getProp(duk_idx_t obj_index) { return duk_get_prop(ctx, obj_index); }
    inline bool getPropIndex(duk_idx_t obj_index, int arr_index) {
        return duk_get_prop_index(ctx, obj_index, arr_index);
    }
    inline bool getPropString(duk_idx_t obj_index, const string& key) {
        return duk_get_prop_string(ctx, obj_index, key.c_str());
    }
    inline void getPrototype(duk_idx_t obj_index) { duk_get_prototype(ctx, obj_index); }
    inline bool hasProp(duk_idx_t obj_index) {
        return duk_has_prop(ctx, obj_index);
    }
    inline bool hasPropIndex(duk_idx_t obj_index, int prop_index) {
        return duk_has_prop_index(ctx, obj_index, prop_index);
    }
    inline bool hasPropString(duk_idx_t obj_index, const string& key) {
        return duk_has_prop_string(ctx, obj_index, key.c_str());
    }
    
    
    inline bool putProp(duk_idx_t obj_index) { return duk_put_prop(ctx, obj_index); }
    inline bool putPropIndex(duk_idx_t obj_index, int arr_index) {
        return duk_put_prop_index(ctx, obj_index, arr_index);
    }
    inline bool putPropString(duk_idx_t obj_index, const string& key) {
        return duk_put_prop_string(ctx, obj_index, key.c_str());
    }
    
    inline void putGlobalString(const string& s) { duk_put_global_string(ctx, s.c_str()); }
    inline bool getGlobalString(const string& key) {
        return duk_get_global_string(ctx, key.c_str());
    }
    
    inline void setGlobalObject() { duk_set_global_object(ctx); }
    inline void setMagic(duk_idx_t index, int magic) { duk_set_magic(ctx, index, magic); }
    inline void setPrototype(duk_idx_t index) { duk_set_prototype(ctx, index); }
    inline void setTop(duk_idx_t index) { duk_set_top(ctx, index); }
    
    inline void defineProperty(duk_idx_t obj_index, unsigned int flags) {
        duk_def_prop(ctx, obj_index, flags);
    }
    inline bool deleteProperty(duk_idx_t obj_index) { return duk_del_prop(ctx, obj_index); }
    inline bool deletePropertyIndex(duk_idx_t obj_index, unsigned int array_index) {
        return duk_del_prop_index(ctx, obj_index, array_index);
    }
    inline bool deletePropertyString(duk_idx_t obj_index, const string& key) {
        obj_index = normalizeIndex(obj_index);
        pushString(key);
        return deleteProperty(obj_index);
    }
    
    inline void setFinalizer(duk_idx_t obj_index) {
        duk_set_finalizer(ctx, obj_index);
    }
    
    inline void setFinalizerFunction(duk_idx_t obj_index, cpp_function func) {
        obj_index = normalizeIndex(obj_index);
        pushFunction(func, 1);
        setFinalizer(obj_index);
    }
    
    inline void setLength(duk_idx_t index, size_t new_length) {
        duk_set_length(ctx, index, new_length);
    }
    
    inline void putObjectGetterSetter(duk_idx_t obj, const string& key,
                                      cpp_function getter, cpp_function setter) {
        obj = normalizeIndex(obj);
        pushString(key);
        pushFunction(getter, 0);
        pushFunction(setter, 1);
        defineProperty(obj, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER);
    }
    inline void putObjectGetter(duk_idx_t obj, const string& key, cpp_function getter) {
        obj = normalizeIndex(obj);
        pushString(key);
        pushFunction(getter, 0);
        defineProperty(obj, DUK_DEFPROP_HAVE_GETTER);
    }
    inline void putObjectSetter(duk_idx_t obj, const string& key, cpp_function setter) {
        obj = normalizeIndex(obj);
        pushString(key);
        pushFunction(setter, 1);
        defineProperty(obj, DUK_DEFPROP_HAVE_SETTER);
    }
    
    inline void putObjectParam(duk_idx_t obj, ofParameter<int>& param) {
        putObjectGetterSetter(obj, param.getName(),
                              [&](ofxDuktape& duk) { duk.pushInt(param); return 1; },
                              [&](ofxDuktape& duk) { param = duk.toInt(0); return 0; }
                              );
    }
    inline void putObjectParam(duk_idx_t obj, ofParameter<float>& param) {
        putObjectGetterSetter(obj, param.getName(),
                              [&](ofxDuktape& duk) { duk.pushNumber(param); return 1; },
                              [&](ofxDuktape& duk) { param = duk.toNumber(0); return 0; }
                              );
    }
    inline void putObjectParam(duk_idx_t obj, ofParameter<double>& param) {
        putObjectGetterSetter(obj, param.getName(),
                              [&](ofxDuktape& duk) { duk.pushNumber(param); return 1; },
                              [&](ofxDuktape& duk) { param = duk.toNumber(0); return 0; }
                              );
    }
    inline void putObjectParam(duk_idx_t obj, ofParameter<string>& param) {
        putObjectGetterSetter(obj, param.getName(),
                              [&](ofxDuktape& duk) { duk.pushString(param); return 1; },
                              [&](ofxDuktape& duk) { param = duk.toString(0); return 0; }
                              );
    }
    inline void putObjectParam(duk_idx_t obj, ofParameter<bool>& param) {
        putObjectGetterSetter(obj, param.getName(),
                              [&](ofxDuktape& duk) { duk.pushBool(param); return 1; },
                              [&](ofxDuktape& duk) { param = duk.toBool(0); return 0; }
                              );
    }
    
    
    
    inline void putObjectConstInt(duk_idx_t obj, const string& key, int value) {
        obj = normalizeIndex(obj);
        pushString(key);
        pushInt(value);
        defineProperty(obj, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_HAVE_WRITABLE); // set read-only
    }
    inline void putObjectConstNumber(duk_idx_t obj, const string& key, double value) {
        obj = normalizeIndex(obj);
        pushString(key);
        pushNumber(value);
        defineProperty(obj, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_HAVE_WRITABLE); // set read-only
    }
    inline void putObjectConstString(duk_idx_t obj, const string& key, const string& value) {
        obj = normalizeIndex(obj);
        pushString(key);
        pushString(value);
        defineProperty(obj, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_HAVE_WRITABLE); // set read-only
    }
    inline void putObjectConstBool(duk_idx_t obj, const string& key, bool value) {
        obj = normalizeIndex(obj);
        pushString(key);
        pushBool(value);
        defineProperty(obj, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_HAVE_WRITABLE); // set read-only
    }
    
    struct object_function_value {
        string prop;
        cpp_function func;
        int argcount;
        object_function_value() {}
        object_function_value(const string& prop, cpp_function func, int argcount):
        prop(prop), func(func), argcount(argcount) {}
    };
    
    inline void putObjectFunctions(duk_idx_t obj, initializer_list<object_function_value> init) {
        for(auto& fn: init) {
            putObjectFunction(obj, fn.prop, fn.func, fn.argcount);
        }
    }
    struct object_getter_setter_value {
        string prop;
        cpp_function getter, setter;
        object_getter_setter_value() {}
        object_getter_setter_value(const string& prop, cpp_function getter, cpp_function setter):
        prop(prop), getter(getter), setter(setter) { }
    };
    struct object_getter_value {
        string prop;
        cpp_function getter;
        object_getter_value() {}
        object_getter_value(const string& prop, cpp_function getter):
        prop(prop), getter(getter) { }
    };
    
    struct object_setter_value {
        string prop;
        cpp_function setter;
        object_setter_value() {}
        object_setter_value(const string& prop, cpp_function setter):
        prop(prop), setter(setter) { }
    };
    
    inline void putObjectGettersSetters(duk_idx_t obj, initializer_list<object_getter_setter_value> init) {
        for(auto& getset: init) {
            putObjectGetterSetter(obj, getset.prop, getset.getter, getset.setter);
        }
    }
    inline void putObjectGetters(duk_idx_t obj, initializer_list<object_getter_value> init) {
        for(auto& getset: init) {
            putObjectGetter(obj, getset.prop, getset.getter);
        }
    }
    inline void putObjectSetters(duk_idx_t obj, initializer_list<object_setter_value> init) {
        for(auto& getset: init) {
            putObjectSetter(obj, getset.prop, getset.setter);
        }
    }
    typedef pair<string, int> object_int_value;
    typedef pair<string, double> object_number_value;
    typedef pair<string, string> object_string_value;
    typedef pair<string, bool> object_bool_value;
    
    inline void putObjectInts(duk_idx_t obj, initializer_list<object_int_value> init) {
        for(auto& i: init) {
            putObjectInt(obj, get<0>(i), get<1>(i));
        }
    }
    inline void putObjectConstInts(duk_idx_t obj, initializer_list<object_int_value> init) {
        for(auto& i: init) {
            putObjectConstInt(obj, get<0>(i), get<1>(i));
        }
    }
    inline void putObjectNumbers(duk_idx_t obj, initializer_list<object_number_value> init) {
        for(auto& i: init) {
            putObjectNumber(obj, get<0>(i), get<1>(i));
        }
    }
    inline void putObjectConstNumbers(duk_idx_t obj, initializer_list<object_number_value> init) {
        for(auto& i: init) {
            putObjectConstNumber(obj, get<0>(i), get<1>(i));
        }
    }
    inline void putObjectBools(duk_idx_t obj, initializer_list<object_bool_value> init) {
        for(auto& i: init) {
            putObjectBool(obj, get<0>(i), get<1>(i));
        }
    }
    inline void putObjectConstBools(duk_idx_t obj, initializer_list<object_bool_value> init) {
        for(auto& i: init) {
            putObjectConstBool(obj, get<0>(i), get<1>(i));
        }
    }
    inline void putObjectStrings(duk_idx_t obj, initializer_list<object_string_value> init) {
        for(auto& i: init) {
            putObjectString(obj, get<0>(i), get<1>(i));
        }
    }
    inline void putObjectConstStrings(duk_idx_t obj, initializer_list<object_string_value> init) {
        for(auto& i: init) {
            putObjectConstString(obj, get<0>(i), get<1>(i));
        }
    }
    
    inline bool equals(duk_idx_t index1, duk_idx_t index2) {
        return duk_equals(ctx, index1, index2);
    }
    
    inline bool strictEquals(duk_idx_t index1, duk_idx_t index2) {
        return duk_strict_equals(ctx, index1, index2);
    }
    
    inline void substring(duk_idx_t index, size_t start, size_t end) {
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
    
    class InvalidKeyException {
    public:
        InvalidKeyException(ofxDuktape *duk, const string& key, const string& description):
        duk(duk),
        key(key),
        description(description) {}
        ofxDuktape *duk;
        string key;
        string description;
    };
    class InvalidIndexException {
    public:
        InvalidIndexException(ofxDuktape *duk, duk_idx_t index, const string& description):
        duk(duk),
        index(index),
        description(description) {}
        ofxDuktape *duk;
        duk_idx_t index;
        string description;
    };
    
    class InvalidObjectException {
    public:
        InvalidObjectException(ofxDuktape *duk, int index, const string& description):
        duk(duk), index(index), description(description) {}
        ofxDuktape *duk;
        int index;
        string description;
    };
    
    inline int getGlobalStringInt(const string& key) {
        if(getGlobalString(key)) return getInt(-1);
        throw(InvalidKeyException(this, key, "not found in global object"));
    }
    
    inline int getGlobalStringUInt(const string& key) {
        if(getGlobalString(key)) return getUint(-1);
        throw(InvalidKeyException(this, key, "not found in global object"));
    }
    
    inline double getGlobalStringNumber(const string& key) {
        if (getGlobalString(key)) return getNumber(-1);
        throw(InvalidKeyException(this, key, "not found in global object"));
    }
    
    inline bool getGlobalStringBool(const string& key) {
        if (getGlobalString(key)) return getBool(-1);
        throw(InvalidKeyException(this, key, "not found in global object"));
    }
    
    inline string getGlobalStringString(const string& key) {
        if (getGlobalString(key)) return getString(-1);
        throw(InvalidKeyException(this, key, "not found in global object"));
    }
    
    inline void* getGlobalStringHeapPtr(const string& key) {
        if (getGlobalString(key)) return getHeapPtr(-1);
        throw(InvalidKeyException(this, key, "not found in global object"));
    }
    
    inline bool getObjectBool(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return getBool(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline bool getObjectBool(duk_idx_t obj, duk_idx_t index) {
        if(getPropIndex(obj, index)) return getBool(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    
    inline int getObjectInt(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return getInt(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline int getObjectInt(duk_idx_t obj, duk_idx_t index) {
        if(getPropIndex(obj, index)) return getInt(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    inline double getObjectNumber(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return getNumber(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline double getObjectNumber(duk_idx_t obj, duk_idx_t index) {
        if(getPropIndex(obj, index)) return getNumber(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }

    inline string getObjectString(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return getString(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline string getObjectString(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return getString(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    inline string getObjectSafeString(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return safeToString(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline string getObjectSafeString(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return safeToString(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    inline void* getObjectHeapPtr(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return getHeapPtr(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline void* getObjectHeapPtr(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return getHeapPtr(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    inline void* getObjectPointer(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return getPointer(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline void* getObjectPointer(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return getPointer(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }


    inline void* getObjectBuffer(duk_idx_t obj, const string& key, size_t& buf_size) {
        if (getPropString(obj, key)) return getBuffer(-1, buf_size);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline void* getObjectBuffer(duk_idx_t obj, duk_idx_t index, size_t& buf_size) {
        if (getPropIndex(obj, index)) return getBuffer(-1, buf_size);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    inline duk_idx_t getObjectObject(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return normalizeIndex(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }

    inline duk_idx_t getObjectObject(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return normalizeIndex(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }

    inline bool isObjectPropUndefined(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return isUndefined(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }

    inline bool isObjectPropUndefined(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return isUndefined(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }

    inline bool isObjectPropNull(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return isNull(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    
    inline bool isObjectPropNull(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return isNull(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    inline bool isObjectPropArray(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return isArray(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    
    inline bool isObjectPropArray(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return isArray(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    
    inline bool isObjectPropObject(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return isObject(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }

    inline bool isObjectPropObject(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return isNull(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    

    
    inline bool isObjectPropBool(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return isBoolean(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline bool isObjectPropBool(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return isBoolean(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    

    
    inline bool isObjectPropNumber(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return isNumber(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline bool isObjectPropNumber(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return isNumber(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    

    
    inline bool isObjectPropBuffer(duk_idx_t obj, const string& key) {
        if (getPropString(obj, key)) return isBuffer(-1);
        throw(InvalidKeyException(this, key, "not found in object"));
    }
    inline bool isObjectPropBuffer(duk_idx_t obj, duk_idx_t index) {
        if (getPropIndex(obj, index)) return isBuffer(-1);
        throw(InvalidIndexException(this, index, "not found in object"));
    }
    

    
    
    
    
    inline void putGlobalStringNull(const string& key) {
        pushNull();
        putGlobalString(key);
    }
    
    inline void putGlobalStringUndefined(const string& key) {
        pushUndefined();
        putGlobalString(key);
    }
    
    inline void putGlobalStringTrue(const string& key) {
        pushTrue();
        putGlobalString(key);
    }
    
    inline void putGlobalStringFalse(const string& key) {
        pushFalse();
        putGlobalString(key);
    }
    
    
    
    inline void putGlobalStringString(const string& key, const string& str) {
        pushString(str);
        putGlobalString(key);
    }
    inline void putGlobalStringBool(const string& key, bool b) {
        pushBool(b);
        putGlobalString(key);
    }
    inline void putGlobalStringInt(const string& key, int i) {
        pushInt(i);
        putGlobalString(key);
    }
    inline void putGlobalStringNumber(const string& key, double d) {
        pushNumber(d);
        putGlobalString(key);
    }
    inline void putGlobalStringHeapPtr(const string& key, void* ptr) {
        pushHeapPtr(ptr);
        putGlobalString(key);
    }
    
    inline void putGlobalStringFunction(const string& key, cpp_function func, int arguments) {
        pushFunction(func, arguments);
        putGlobalString(key);
    }
    
    inline void putGlobalStringFixedBuffer(const string& key, int size, void** bufptr) {
        *bufptr = pushFixedBuffer(size);
        putGlobalString(key);
    }
    
    inline void putGlobalStringDynamicBuffer(const string& key, int initial_size, void** bufptr) {
        *bufptr = pushDynamicBuffer(initial_size);
        putGlobalString(key);
    }
    
    inline void putGlobalStringExternalBuffer(const string& key, void* ptr, size_t len) {
        pushExternalBuffer(ptr, len);
        putGlobalString(key);
    }
    
    
    inline void putObjectNull(duk_idx_t obj, const string& key) {
        pushNull();
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectNull(duk_idx_t obj, duk_idx_t i) {
        pushNull();
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    
    inline void putObjectUndefined(duk_idx_t obj, const string& key) {
        pushUndefined();
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectUndefined(duk_idx_t obj, duk_idx_t i) {
        pushUndefined();
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectTrue(duk_idx_t obj, const string& key) {
        pushTrue();
        if (!putPropString(obj >0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectTrue(duk_idx_t obj, duk_idx_t i) {
        pushTrue();
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectFalse(duk_idx_t obj, const string& key) {
        pushFalse();
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectFalse(duk_idx_t obj, duk_idx_t i) {
        pushFalse();
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectBool(duk_idx_t obj, const string&key, bool b) {
        pushBool(b);
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectBool(duk_idx_t obj, duk_idx_t i, bool b) {
        pushBool(b);
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectInt(duk_idx_t obj, const string&key, int i) {
        pushInt(i);
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectInt(duk_idx_t obj, duk_idx_t i, int n) {
        pushInt(n);
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectNumber(duk_idx_t obj, const string&key, double d) {
        pushNumber(d);
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectNumber(duk_idx_t obj, duk_idx_t i, double d) {
        pushNumber(d);
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectString(duk_idx_t obj, const string&key, const string& value) {
        pushString(value);
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectString(duk_idx_t obj, duk_idx_t i, const string& value) {
        pushString(value);
        if (!putPropIndex(obj > 0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectPointer(duk_idx_t obj, const string&key, void* ptr) {
        pushPointer(ptr);
        if (!putPropString(obj > 0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }

    inline void putObjectHeapPtr(duk_idx_t obj, const string& key, void* ptr) {
        pushHeapPtr(ptr);
        if (!putPropString(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectHeapPtr(duk_idx_t obj, duk_idx_t i, void* ptr) {
        pushHeapPtr(ptr);
        if (!putPropIndex(obj>0?obj:obj - 1, i)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectFunction(duk_idx_t obj, const string& key, cpp_function func, int args) {
        pushFunction(func, args);
        if (!putPropString(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectFunction(duk_idx_t obj, duk_idx_t key, cpp_function func, int args) {
        pushFunction(func, args);
        if (!putPropIndex(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectDynamicBuffer(duk_idx_t obj, const string& key, int initial_size, void** bufptr) {
        *bufptr = pushDynamicBuffer(initial_size);
        if (!putPropString(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectDynamicBuffer(duk_idx_t obj, duk_idx_t key, int initial_size, void** bufptr) {
        *bufptr = pushDynamicBuffer(initial_size);
        if (!putPropIndex(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectFixedBuffer(duk_idx_t obj, const string& key, int size, void** bufptr) {
        *bufptr = pushFixedBuffer(size);
        if (!putPropString(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectFixedBuffer(duk_idx_t obj, duk_idx_t key, int size, void** bufptr) {
        *bufptr = pushFixedBuffer(size);
        if (!putPropIndex(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectExternalBuffer(duk_idx_t obj, const string& key, void* ptr, size_t len) {
        pushExternalBuffer(ptr, len);
        if (!putPropString(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectExternalBuffer(duk_idx_t obj, duk_idx_t key, void* ptr, size_t len) {
        pushExternalBuffer(ptr, len);
        if (!putPropIndex(obj>0?obj:obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectGlobalStash(duk_idx_t obj, const string& key) {
        pushGlobalStash();
        if (!putPropString(obj > 0 ? obj : obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectGlobalStash(duk_idx_t obj, duk_idx_t key) {
        pushGlobalStash();
        if (!putPropIndex(obj > 0 ? obj : obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectHeapStash(duk_idx_t obj, const string& key) {
        pushHeapStash();
        if (!putPropString(obj > 0 ? obj : obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void putObjectHeapStash(duk_idx_t obj, duk_idx_t key) {
        pushHeapStash();
        if (!putPropIndex(obj > 0 ? obj : obj - 1, key)) {
            throw(InvalidObjectException(this, obj, "invalid object"));
        }
    }
    
    inline void base64Decode(duk_idx_t obj) {
        duk_base64_decode(ctx, obj);
    }
    inline string base64Encode(duk_idx_t obj) {
        return duk_base64_encode(ctx, obj);
    }
    inline void base64Decode(const string& str) {
        pushString(str);
        base64Decode(-1);
    }
    
    inline void jsonDecode(duk_idx_t obj) {
        duk_json_decode(ctx, obj);
    }
    inline string jsonEncode(duk_idx_t obj) {
        return duk_json_encode(ctx, obj);
    }
    inline void jsonDecode(const string& str) {
        pushString(str);
        jsonDecode(-1);
    }
    
};

