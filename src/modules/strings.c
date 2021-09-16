#include "wren.h"
#include "tclGlobMatch.h"
#include "utf8.h"
#include <string.h>

void stringsGlobMatch(WrenVM *vm) {
    const char* string = wrenGetSlotString(vm, 1);
    int strLen = (int)wrenGetSlotDouble(vm, 2);
    const char* pattern = wrenGetSlotString(vm, 3);
    int ptnLen = (int)wrenGetSlotDouble(vm, 4);
    int result = TclByteArrayMatch(string, strLen, pattern, ptnLen, 0);
    wrenSetSlotBool(vm, 0, result);
}

/*
From: utf8.h README
Various functions provided will do case insensitive compares, or transform utf8 strings 
from one case to another. Given the vastness of unicode, and the authors lack of understanding beyond latin 
codepoints on whether case means anything, the following categories are the only ones that will be checked in case insensitive code:
    ASCII
    Latin-1 Supplement
    Latin Extended-A
    Latin Extended-B
    Greek and Coptic
    Cyrillic
*/
void stringsUpcase(WrenVM *vm) {
    const char* string = wrenGetSlotString(vm, 1);
    size_t sz;
    char *str;
    
    sz = strlen(string);
    str = (char *)malloc(sz + 1);
    memcpy(str, string, sz + 1);
    
    utf8upr(str);

    wrenSetSlotString(vm, 0, str);
    free(str);
}

void stringsDowncase(WrenVM *vm) {
    const char* string = wrenGetSlotString(vm, 1);
    size_t sz;
    char *str;
    
    sz = strlen(string);
    str = (char *)malloc(sz + 1);
    memcpy(str, string, sz + 1);
    
    utf8lwr(str);

    wrenSetSlotString(vm, 0, str);
    free(str);
}
