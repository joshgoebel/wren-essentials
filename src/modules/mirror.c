#include "mirror.h"

#include <string.h>
#include "wren_vm.h"

static ObjClass* mirrorGetSlotClass(WrenVM* vm, int slot)
{
  Value classVal = vm->apiStack[slot];
  if (!IS_CLASS(classVal)) return NULL;

  return AS_CLASS(classVal);
}

static ObjFiber* mirrorGetSlotFiber(WrenVM* vm, int slot)
{
  Value fiberVal = vm->apiStack[slot];
  if (!IS_FIBER(fiberVal)) return NULL;

  return AS_FIBER(fiberVal);
}

static void mirrorClassMirrorAllAttributes(WrenVM* vm)
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

static void mirrorClassMirrorHasMethod(WrenVM* vm)
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

static void mirrorClassMirrorMethodNames(WrenVM* vm)
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

static void mirrorFiberFunctionAt(WrenVM* vm)
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
  wrenSetSlot(vm, 0, OBJ_VAL(frame->closure));
}

static void mirrorFiberLineAt(WrenVM* vm)
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

static void mirrorFiberStackFramesCount(WrenVM* vm)
{
  ObjFiber* fiber = mirrorGetSlotFiber(vm, 1);

  if (fiber == NULL)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  wrenSetSlotDouble(vm, 0, fiber->numFrames);
}

static void mirrorObjectMirrorCanInvoke(WrenVM* vm)
{
  ObjClass* classObj = wrenGetClassInline(vm, vm->apiStack[1]);
  vm->apiStack[1] = OBJ_VAL(classObj);

  mirrorClassMirrorHasMethod(vm);
}

WrenForeignMethodFn wrenMirrorBindForeignMethod(WrenVM* vm,
                                                const char* className,
                                                bool isStatic,
                                                const char* signature)
{
  if (strcmp(className, "ClassMirror") == 0)
  {
    if (isStatic &&
        strcmp(signature, "allAttributes(_)") == 0)
    {
      return mirrorClassMirrorAllAttributes;
    }

    if (isStatic &&
        strcmp(signature, "hasMethod(_,_)") == 0)
    {
      return mirrorClassMirrorHasMethod;
    }
    if (isStatic &&
        strcmp(signature, "methodNames(_)") == 0)
    {
      return mirrorClassMirrorMethodNames;
    }
  }

  if (strcmp(className, "FiberMirror") == 0)
  {
    if (isStatic &&
        strcmp(signature, "functionAt_(_,_)") == 0)
    {
      return mirrorFiberFunctionAt;
    }
    if (isStatic &&
        strcmp(signature, "lineAt_(_,_)") == 0)
    {
      return mirrorFiberLineAt;
    }
    if (isStatic &&
        strcmp(signature, "stackFramesCount_(_)") == 0)
    {
      return mirrorFiberStackFramesCount;
    }
  }

  if (strcmp(className, "ObjectMirror") == 0)
  {
    if (isStatic &&
        strcmp(signature, "canInvoke(_,_)") == 0)
    {
      return mirrorObjectMirrorCanInvoke;
    }
  }

  ASSERT(false, "Unknown method.");
  return NULL;
}

