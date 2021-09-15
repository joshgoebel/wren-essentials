#include "json.h"
#include "pdjson.h"

json_stream jsonStream[1];

void jsonStreamBegin(WrenVM * vm) {
  const char * value = wrenGetSlotString(vm, 1);
  json_open_string(jsonStream, value);
  json_set_streaming(jsonStream, 0);
}

void jsonStreamEnd(WrenVM * vm) {
  json_reset(jsonStream);
  json_close(jsonStream);
}

void jsonStreamValue(WrenVM * vm) {
  const char * value = json_get_string(jsonStream, 0);
  wrenSetSlotString(vm, 0, value);
}

void jsonStreamErrorMessage(WrenVM * vm) {
  const char * error = json_get_error(jsonStream);
  if(error) {
    wrenSetSlotString(vm, 0, error);
    return;
  }
  wrenSetSlotString(vm, 0, "");
}

void jsonStreamLineNumber(WrenVM * vm) {
  wrenSetSlotDouble(vm, 0, json_get_lineno(jsonStream));
}

void jsonStreamPos(WrenVM * vm) {
  wrenSetSlotDouble(vm, 0, json_get_position(jsonStream));
}

void jsonStreamNext(WrenVM * vm) {
  enum json_type type = json_next(jsonStream);
  // 0 in the enum seems to be reserved for no more tokens
  if (type > 0) {
    wrenSetSlotDouble(vm, 0, type);
    return;
  }
  wrenSetSlotNull(vm, 0);
}
