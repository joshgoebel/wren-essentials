#include "essentials.h"

#include "modules/time.h"
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
ModuleRegistry moduleRegistry[] =
{
  MODULE(essentials)
    CLASS(Time)
      STATIC_METHOD("now()", timeNow)
      STATIC_METHOD("highResolution()", timeHighResolution)
    END_CLASS
  END_MODULE

  SENTINEL_MODULE
};

// this the API we export as a dynamic library
ModuleRegistry* returnRegistry() {
  return moduleRegistry;
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