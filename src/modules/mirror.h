#ifndef wren_opt_mirror_h
#define wren_opt_mirror_h

#include "wren_common.h"
#undef ALLOCATE
#include "wren.h"

void mirrorClassMirrorAllAttributes(WrenVM* vm);
void mirrorClassMirrorHasMethod(WrenVM* vm);
void mirrorClassMirrorMethodNames(WrenVM* vm);
void mirrorFiberMirrorMethodAt(WrenVM* vm);
void mirrorFiberLineAt(WrenVM* vm);
void mirrorFiberStackFramesCount(WrenVM* vm);
void mirrorModuleMirrorFromName_(WrenVM* vm);
void mirrorModuleMirrorName_(WrenVM* vm);
void mirrorMethodMirrorModule_(WrenVM* vm);
void mirrorMethodMirrorSignature_(WrenVM* vm);
void mirrorObjectMirrorCanInvoke(WrenVM* vm);

#endif
