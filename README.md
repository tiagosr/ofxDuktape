# ofxDuktape

openFrameworks addon with bindings to the [Duktape](http://www.duktape.org) Javascript interpreter
This addon is distributed under the terms of the MIT License.

## Usage

```c++
#include "ofxDuktape.h"

// create a duktape context anywhere
ofxDuktape duk;

// use it in your code
int retcode = duk.pEval("log(\"hello openFrameworks!\"");

// pEval returns 0 if code was executed correctly, and other values for errors
if (!retcode) {
  // -1 is the index to the last object stored in duktape's stack, i.e. the result of the call
  ofLogInfo() << "return value is " << duk.safeToString(-1);
} else {
  ofLogError() << "error while executing javascript";
}
```

## Extensions

The header file ```ofxDukOFBindings.h``` has most of openFrameworks 0.9 API implemented inside the ```of``` object,
and a lot of syntactic sugar - ofSetXXX methods with single parameters and ofGetXXX methods are mostly implemented as properties

```c++
#include "ofxDukOFBindings.h"
ofxDuktape duk;

// setup the 'of' object in the javascript context
ofxDukBindings::setup(duk);

// interact with openFrameworks
duk.pEval("of.windowTitle = \"New window title\"");
```
