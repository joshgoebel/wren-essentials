#include "wren.h"
#include "../vendor/tclGlobMatch.h"

void stringsGlobMatch(WrenVM *vm) {
    const char* string = wrenGetSlotString(vm, 1);
    int strLen = (int)wrenGetSlotDouble(vm, 2);
    const char* pattern = wrenGetSlotString(vm, 3);
    int ptnLen = (int)wrenGetSlotDouble(vm, 4);
    int result = TclByteArrayMatch(string, strLen, pattern, ptnLen, 0);
    wrenSetSlotBool(vm, 0, result);
}
