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
const char jsonTypeName[][64] = {
    [JSON_ERROR]      = "wren-essentials.json:JSON_ERROR",
    [JSON_DONE]       = "wren-essentials.json:JSON_DONE",
    [JSON_OBJECT]     = "wren-essentials.json:JSON_OBJECT",
    [JSON_OBJECT_END] = "wren-essentials.json:JSON_OBJECT_END",
    [JSON_ARRAY]      = "wren-essentials.json:JSON_ARRAY",
    [JSON_ARRAY_END]  = "wren-essentials.json:JSON_ARRAY_END",
    [JSON_STRING]     = "wren-essentials.json:JSON_STRING",
    [JSON_NUMBER]     = "wren-essentials.json:JSON_NUMBER",
    [JSON_TRUE]       = "wren-essentials.json:JSON_TRUE",
    [JSON_FALSE]      = "wren-essentials.json:JSON_FALSE",
    [JSON_NULL]       = "wren-essentials.json:JSON_NULL",
};

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
  if (jsonTypeName[type]) {
    wrenSetSlotString(vm, 0, jsonTypeName[type]);
    return;
  }
  wrenSetSlotNull(vm, 0);
}

void jsonStreamEscapeChar(WrenVM * vm) {
  ASSERT_SLOT_TYPE(vm, 1, WREN_TYPE_STRING, "value");
  ASSERT_SLOT_TYPE(vm, 2, WREN_TYPE_NUM, "options");

  const char * value = wrenGetSlotString(vm, 1);
  int options = (int) wrenGetSlotDouble(vm, 2);

  const char * result = value;

  /*
  "\0" // The NUL byte: 0.
  "\"" // A double quote character.
  "\\" // A backslash.
  "\a" // Alarm beep. (Who uses this?)
  "\b" // Backspace.
  "\f" // Formfeed.
  "\n" // Newline.
  "\r" // Carriage return.
  "\t" // Tab.
  "\v" // Vertical tab.
  */
  if (utf8cmp(value, "\0") == 0) {
    result = "\\0";
  } else if (utf8cmp(value, "\"") == 0) {
    result = "\\\"";
  } else if (utf8cmp(value, "\\") == 0) {
    result = "\\\\";
  } else if (utf8cmp(value, "\\") == 0) {
    result = "\\a";
  } else if (utf8cmp(value, "\b") == 0) {
    result = "\\b";
  } else if (utf8cmp(value, "\f") == 0) {
    result = "\\f";
  } else if (utf8cmp(value, "\n") == 0) {
    result = "\\n";
  } else if (utf8cmp(value, "\r") == 0) {
    result = "\\r";
  } else if (utf8cmp(value, "\t") == 0) {
    result = "\\t";
  } else if (utf8cmp(value, "\v") == 0) {
    result = "\\v";
  } else if (utf8cmp(value, "/") == 0) {
    // Escape / (solidus, slash)
    // https://stackoverflow.com/a/9735430
    // The feature of the slash escape allows JSON to be embedded in HTML (as SGML) and XML.
    // https://www.w3.org/TR/html4/appendix/notes.html#h-B.3.2
    // This is optional escaping. Disabled by default.
    // use JsonOptions.escapeSlashes option to enable it
    if ((options & JSON_OPTS_ESCAPE_SLASHES) != JSON_OPTS_NIL) {
        result = "\\/";
    }
  }

  wrenSetSlotString(vm, 0, result);
}
