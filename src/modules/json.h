#ifndef wren_json_h
#define wren_json_h

#include "wren_common.h"
#include "wren.h"
#include "pdjson.h"

// JSON Parser Events

void jsonStreamBegin(WrenVM * vm);
void jsonStreamEnd(WrenVM * vm);
void jsonStreamValue(WrenVM * vm);
void jsonStreamErrorMessage(WrenVM * vm);
void jsonStreamLineNumber(WrenVM * vm);
void jsonStreamPos(WrenVM * vm);
void jsonStreamNext(WrenVM * vm);
void jsonStreamEscapeChar(WrenVM * vm);

#endif
