#include "essentials.h"

#include "modules/time.h"
#include "modules/mirror.h"
#include "modules/json.h"
#include "modules/wren_code.inc"



// To locate foreign classes and modules, we build a big directory for them in
// static data. The nested collection initializer syntax gets pretty noisy, so
// define a couple of macros to make it easier.
#define SENTINEL_METHOD { false, NULL, NULL }
#define SENTINEL_CLASS { NULL, { SENTINEL_METHOD } }
#define SENTINEL_MODULE {NULL, NULL, { SENTINEL_CLASS } }

#define MODULE(name) { #name, &name##ModuleSource, {
#define END_MODULE SENTINEL_CLASS } },

#define CLASS(name) { #name, {
#define END_CLASS SENTINEL_METHOD } },

#define METHOD(signature, fn) { false, signature, fn },
#define STATIC_METHOD(signature, fn) { true, signature, fn },
#define ALLOCATE(fn) { true, "<allocate>", (WrenForeignMethodFn)fn },
#define FINALIZE(fn) { true, "<finalize>", (WrenForeignMethodFn)fn },

// The array of built-in modules.
ModuleRegistry essentialRegistry[] =
{
  MODULE(mirror)
    CLASS(ClassMirror)
      STATIC_METHOD("allAttributes(_)", mirrorClassMirrorAllAttributes)
      STATIC_METHOD("hasMethod(_,_)", mirrorClassMirrorHasMethod)
      STATIC_METHOD("methodNames(_)",mirrorClassMirrorMethodNames)
    END_CLASS
    CLASS(FiberMirror)
      STATIC_METHOD("methodAt_(_,_)", mirrorFiberMirrorMethodAt)
      STATIC_METHOD("lineAt_(_,_)",mirrorFiberLineAt)
      STATIC_METHOD("stackFramesCount_(_)",mirrorFiberStackFramesCount)
     END_CLASS
    CLASS(MethodMirror)
      STATIC_METHOD("module_(_)", mirrorMethodMirrorModule_)
      STATIC_METHOD("signature_(_)", mirrorMethodMirrorSignature_)
    END_CLASS
    CLASS(ObjectMirror)
      STATIC_METHOD("canInvoke(_,_)", mirrorObjectMirrorCanInvoke)
    END_CLASS
    CLASS(ModuleMirror)
      STATIC_METHOD("fromName_(_)", mirrorModuleMirrorFromName_)
      STATIC_METHOD("name_(_)", mirrorModuleMirrorName_)
    END_CLASS
  END_MODULE
  MODULE(essentials)
    CLASS(Time)
      STATIC_METHOD("now()", timeNow)
      STATIC_METHOD("highResolution()", timeHighResolution)
    END_CLASS
  END_MODULE

  MODULE(json)
    CLASS(JsonStream)
      METHOD("stream_begin(_)", jsonStreamBegin)
      METHOD("stream_end()", jsonStreamEnd)
      METHOD("next", jsonStreamNext)
      METHOD("value", jsonStreamValue)
      METHOD("error_message", jsonStreamErrorMessage)
      METHOD("lineno", jsonStreamLineNumber)
      METHOD("pos", jsonStreamPos)
      STATIC_METHOD("escapechar(_,_)", jsonStreamEscapeChar)
    END_CLASS
  END_MODULE

  SENTINEL_MODULE
};

// this the API we export as a dynamic library
ModuleRegistry* returnRegistry() {
  return essentialRegistry;
}


#undef SENTINEL_METHOD
#undef SENTINEL_CLASS
#undef SENTINEL_MODULE
#undef MODULE
#undef END_MODULE
#undef CLASS
#undef END_CLASS
#undef METHOD
#undef STATIC_METHOD
#undef FINALIZER