#include "mirror.h"

#include <string.h>
#include "wren_vm.h"

#define IS_MODULE(value) (wrenIsObjType(value, OBJ_MODULE))     // ObjModule

// Wren doesn't expose this to us
static void validateApiSlot(WrenVM* vm, int slot)
{
  ASSERT(slot >= 0, "Slot cannot be negative.");
  ASSERT(slot < wrenGetSlotCount(vm), "Not that many slots.");
}

Value* wrenSlotAtUnsafe(WrenVM* vm, int slot)
{
  validateApiSlot(vm, slot);
  return &vm->apiStack[slot];
}

ObjModule* wrenGetModule(WrenVM* vm, Value name)
{
  Value moduleValue = wrenMapGet(vm->modules, name);
  return !IS_UNDEFINED(moduleValue) ? AS_MODULE(moduleValue) : NULL;
}

static inline Method *wrenClassGetMethod(WrenVM* vm, const ObjClass* classObj,
                                         int symbol)
{
  Method* method;
  if (symbol >= 0 && symbol < classObj->methods.count &&
      (method = &classObj->methods.data[symbol])->type != METHOD_NONE)
  {
    return method;
  }
  return NULL;
}

void wrenSetSlot(WrenVM* vm, int slot, Value value)
{
  validateApiSlot(vm, slot);
  vm->apiStack[slot] = value;
}

/* ^------------------ Wren extensions */

static ObjClass* mirrorGetSlotClass(WrenVM* vm, int slot)
{
  Value classVal = vm->apiStack[slot];
  if (!IS_CLASS(classVal)) return NULL;

  return AS_CLASS(classVal);
}

static ObjClosure* mirrorGetSlotClosure(WrenVM* vm, int slot)
{
  Value closureVal = *wrenSlotAtUnsafe(vm, slot);
  if (!IS_CLOSURE(closureVal)) return NULL;

  return AS_CLOSURE(closureVal);
}

static ObjFiber* mirrorGetSlotFiber(WrenVM* vm, int slot)
{
  Value fiberVal = vm->apiStack[slot];
  if (!IS_FIBER(fiberVal)) return NULL;

  return AS_FIBER(fiberVal);
}

static ObjModule* mirrorGetSlotModule(WrenVM* vm, int slot)
{
  Value moduleVal = *wrenSlotAtUnsafe(vm, slot);
  if (!IS_MODULE(moduleVal)) return NULL;

  return AS_MODULE(moduleVal);
}

 void mirrorClassMirrorAllAttributes(WrenVM* vm)
{
  ObjClass* classObj = mirrorGetSlotClass(vm, 1);

  if (classObj != NULL)
  {
    wrenSetSlot(vm, 0, classObj->attributes);
  }
  else
  {
    wrenSetSlotNull(vm, 0);
  }
}

 void mirrorClassMirrorHasMethod(WrenVM* vm)
{
  ObjClass* classObj = mirrorGetSlotClass(vm, 1);
  const char* method = wrenGetSlotString(vm, 2);

  bool hasMethod = false;
  if (classObj != NULL &&
      method != NULL)
  {
    int symbol = wrenSymbolTableFind(&vm->methodNames, method, strlen(method));
    hasMethod = wrenClassGetMethod(vm, classObj, symbol) != NULL;
  }
  wrenSetSlotBool(vm, 0, hasMethod);
}

 void mirrorClassMirrorMethodNames(WrenVM* vm)
{
  ObjClass* classObj = mirrorGetSlotClass(vm, 1);

  if (!classObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  wrenSetSlotNewList(vm, 0);
  for (size_t symbol = 0; symbol < classObj->methods.count; symbol++)
  {
    Method* method = wrenClassGetMethod(vm, classObj, symbol);
    if (method == NULL) continue;

    wrenSetSlot(vm, 1, OBJ_VAL(vm->methodNames.data[symbol]));
    wrenInsertInList(vm, 0, -1, 1);
  }
}

 void mirrorFiberMirrorMethodAt(WrenVM* vm)
{
  ObjFiber* fiber = mirrorGetSlotFiber(vm, 1);
  size_t index = wrenGetSlotDouble(vm, 2);
  CallFrame* frame;

  if (fiber == NULL ||
      (frame = &fiber->frames[index])->closure == NULL)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }
  *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(frame->closure);
}

 void mirrorFiberLineAt(WrenVM* vm)
{
  ObjFiber* fiber = mirrorGetSlotFiber(vm, 1);
  size_t index = wrenGetSlotDouble(vm, 2);
  CallFrame* frame;
  ObjFn* fn;

  if (fiber == NULL ||
      (frame = &fiber->frames[index]) == NULL ||
      (fn = frame->closure->fn) == NULL ||
      fn->debug->sourceLines.data == NULL)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  size_t line = fn->debug->sourceLines.data[frame->ip - fn->code.data - 1];

  wrenSetSlotDouble(vm, 0, line);
}

 void mirrorFiberStackFramesCount(WrenVM* vm)
{
  ObjFiber* fiber = mirrorGetSlotFiber(vm, 1);

  if (fiber == NULL)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }
  wrenSetSlotDouble(vm, 0, fiber->numFrames);
}

void mirrorMethodMirrorModule_(WrenVM* vm)
{
  ObjClosure* closureObj = mirrorGetSlotClosure(vm, 1);

  if (!closureObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(closureObj->fn->module);
}

void mirrorMethodMirrorSignature_(WrenVM* vm)
{
  ObjClosure* closureObj = mirrorGetSlotClosure(vm, 1);

  if (!closureObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  wrenSetSlotString(vm, 0, closureObj->fn->debug->name);
}

void mirrorModuleMirrorFromName_(WrenVM* vm)
{
  const char* moduleName = wrenGetSlotString(vm, 1);

  if (!moduleName)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  // Special case for "core"
  if (strcmp(moduleName, "core") == 0)
  {
    wrenSetSlotNull(vm, 1);
  }

  ObjModule* module = wrenGetModule(vm, *wrenSlotAtUnsafe(vm, 1));
  if (module != NULL)
  {
    *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(module);
  }
  else
  {
    wrenSetSlotNull(vm, 0);
  }
}

void mirrorModuleMirrorName_(WrenVM* vm)
{
  ObjModule* moduleObj = mirrorGetSlotModule(vm, 1);
  if (!moduleObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  if (moduleObj != NULL)
  {
    if (moduleObj->name)
    {
      *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(moduleObj->name);
    }
    else
    {
      // Special case for "core"
      wrenSetSlotString(vm, 0, "core");
    }
  }
  else
  {
    wrenSetSlotNull(vm, 0);
  }
}

 void mirrorObjectMirrorCanInvoke(WrenVM* vm)
{
  ObjClass* classObj = wrenGetClassInline(vm, vm->apiStack[1]);
  vm->apiStack[1] = OBJ_VAL(classObj);

  mirrorClassMirrorHasMethod(vm);
}
