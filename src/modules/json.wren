// Extracted from https://github.com/domeengine/dome/blob/develop/src/modules/json.wren

class JsonOptions {
  static nil { 0 }
  static escapeSlashes { 1 }
  static abortOnError { 2 }
  static checkCircular { 4 }

  static contains(options, option) {
    return ((options & option) != JsonOptions.nil)
  }
}

class JsonError {
  line { _line }
  position { _position }
  message { _message }
  found { _found }

  construct new(line, pos, message, found) {
    _line = line
    _position = pos
    _message = message
    _found = found
  }

  static empty() {
    return JsonError.new(0, 0, "", false)
  }
}

// pdjson.h:

// enum json_type {
//     JSON_ERROR = 1, JSON_DONE,
//     JSON_OBJECT, JSON_OBJECT_END, JSON_ARRAY, JSON_ARRAY_END,
//     JSON_STRING, JSON_NUMBER, JSON_TRUE, JSON_FALSE, JSON_NULL
// };

class Token {
  static isError { 1 }
  static isDone { 2 }
  static isObject { 3 }
  static isObjectEnd { 4 }
  static isArray { 5 }
  static isArrayEnd { 6 }
  static isString { 7 }
  static isNumeric { 8 }
  static isBoolTrue { 9 }
  static isBoolFalse { 10 }
  static isNull { 11 }
}

class JsonStream {
  foreign stream_begin(value)
  foreign stream_end()
  foreign next
  foreign value
  foreign error_message
  foreign lineno
  foreign pos
  foreign static escapechar(value, options)

  result { _result }
  error { _error }
  options { _options }
  raw { _raw }

  construct new(raw, options) {
    _result = {}
    _error = JsonError.empty()
    _lastEvent = null
    _raw = raw
    _options = options
  }

  begin() {
    stream_begin(_raw)
    _result = process(next)
  }

  end() {
    stream_end()
  }

  process(event) {
    _lastEvent = event

    if (event == Token.isError) {
      _error = JsonError.new(lineno, pos, error_message, true)
      if (JsonOptions.contains(_options, JsonOptions.abortOnError)) {
        end()
        Fiber.abort("JSON error - line %(lineno) pos %(pos): %(error_message)")
      }
      return
    }

    if (event == Token.isDone) {
      return
    }

    if (event == Token.isBoolTrue || event == Token.isBoolFalse) {
      return (event == Token.isBoolTrue)
    }

    if (event == Token.isNumeric) {
      return Num.fromString(this.value)
    }

    if (event == Token.isString) {
      return this.value
    }

    if (event == Token.isNull) {
      return null
    }

    if (event == Token.isArray) {
      var elements = []
      while (true) {
        event = next
        _lastEvent = event
        if (event == Token.isArrayEnd) {
          break
        }
        elements.add(process(event))
      }
      return elements
    }

    if (event == Token.isObject) {
      var elements = {}
      while (true) {
        event = next
        _lastEvent = event
        if (event == Token.isObjectEnd) {
            break
        }
        elements[this.value] = process(next)
      }
      return elements
    }
  }
}

// protocol for Json encodable values
// So they can override how to
class JsonEncodable {
  toJson {this.toString}
  toJSON {toJson}
}

class JsonEncoder {
  construct new(options) {
    _options = options
    _circularStack = JsonOptions.contains(options, JsonOptions.checkCircular) ? [] : null
  }

  isCircle(value) {
    if (_circularStack == null) {
      return false
    }
    return _circularStack.any { |v| Object.same(value, v) }
  }

  push(value) {
    if (_circularStack != null) {
      _circularStack.add(value)
    }
  }
  pop() {
    if (_circularStack != null) {
      _circularStack.removeAt(-1)
    }
  }

  encode(value) {
    if (isCircle(value)) {
      Fiber.abort("Circular JSON")
    }

    // Loosely based on https://github.com/brandly/wren-json/blob/master/json.wren
    if (value is Num || value is Bool || value is Null) {
      return value.toString
    }

    if (value is String) {
      // Escape special characters
      var substrings = []
      for (char in value) {
        substrings.add(JsonStream.escapechar(char, _options))
      }

      // Compile error if you use normal escaping sequence
      // so we have to use bytes to string method for the single " char
      return String.fromByte(0x22) + substrings.join("") + String.fromByte(0x22)
    }

    if (value is List) {
      push(value)
      var substrings = []
      for (item in value) {
        substrings.add(encode(item))
      }
      pop()
      return "[" + substrings.join(",") + "]"
    }

    if (value is Map) {
      push(value)
      var substrings = []
      for (key in value.keys) {
        var keyValue = this.encode(value[key])
        var encodedKey = this.encode(key)
        substrings.add("%(encodedKey):%(keyValue)")
      }
      pop()
      return "{" + substrings.join(",") + "}"
    }

    if (value is JsonEncodable) {
      return value.toJson
    }

    // Default behaviour is to invoke the toString method
    return value.toString
  }
}

class Json {

  static encode(value, options) { JsonEncoder.new(options).encode(value) }

  static encode(value) {
    return Json.encode(value, JsonOptions.abortOnError)
  }

  static parse(value) {
    return Json.decode(value)
  }

  static stringify(value) {
    return Json.encode(value)
  }

  static decode(value, options) {
    var stream = JsonStream.new(value, options)
    stream.begin()

    var result = stream.result
    if (stream.error.found) {
      result = stream.error
    }

    stream.end()
    return result
  }

  static decode(value) {
    return Json.decode(value, JsonOptions.abortOnError)
  }
}

var JSON = Json
var JSONOptions = JsonOptions
var JSONError = JsonError
var JSONEncodable = JsonEncodable