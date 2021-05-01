#ifndef time_h
#define time_h

#include "uv.h"
#include "wren.h"

void timeNow(WrenVM* vm);
void timeHighResolution(WrenVM* vm);

#endif