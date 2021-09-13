#include "json.h"
#include "utf8.h"
#include "pdjson.h"

// Extracted from DOME engine
#define VM_ABORT(vm, error) do {\
  wrenSetSlotString(vm, 0, error);\
  wrenAbortFiber(vm, 0); \
} while(false);

#define ASSERT_SLOT_TYPE(vm, slot, type, fieldName) \
  if (wrenGetSlotType(vm, slot) != type) { \
    VM_ABORT(vm, #fieldName " was not " #type); \
    return; \
  }

// Json API

// We have to use C functions for escaping chars
// because a bug in compiler throws error when using \ in strings
// inside Wren files.
// TODO: Check this in the future.
enum JsonOptions {
    JSON_OPTS_NIL = 0,
    JSON_OPTS_ESCAPE_SLASHES = 1,
    JSON_OPTS_ABORT_ON_ERROR = 2
};

json_stream jsonStream[1];

void jsonStreamBegin(WrenVM * vm) {
  ASSERT_SLOT_TYPE(vm, 1, WREN_TYPE_STRING, "value");
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
