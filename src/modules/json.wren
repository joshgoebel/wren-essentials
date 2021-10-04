import "ensure" for Ensure

// Mainly based on https://github.com/domeengine/dome/blob/develop/src/modules/json.wren
// Some code based on https://github.com/brandly/wren-json/blob/master/json.wren

class JSONOptions {
  static nil { 0 }
  static escapeSolidus { 1 }
  static abortOnError { 2 }
  static checkCircular { 4 }

  static contains(options, option) {
    return ((options & option) != JSONOptions.nil)
  }
}

class JSONError {
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
    return JSONError.new(0, 0, "", false)
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

class JSONStream {
  // Ensure the stream is always a string
  stream_begin(value) {
    Ensure.string(value, "value")
    stream_begin_(value)
  }
  foreign stream_begin_(value)
  
  foreign stream_end()
  foreign next
  foreign value
  foreign error_message
  foreign lineno
  foreign pos

  result { _result }
  error { _error }
  options { _options }
  raw { _raw }

  construct new(raw, options) {
    _result = {}
    _error = JSONError.empty()
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
      _error = JSONError.new(lineno, pos, error_message, true)
      if (JSONOptions.contains(_options, JSONOptions.abortOnError)) {
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

// Protocol for JSON encodable objects
// Prefer this protocol instead of toString
// Override toJSON in the child
class JSONEncodable {
  toJSON {this.toString}
}

class JSONEscapeChars {
  static hexchars {["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"]}

  static toHex(byte) {
    var hex = ""
    while (byte > 0) {
      var c = byte % 16
      hex = hexchars[c] + hex
      byte = byte >> 4
    }
    return hex
  }
  
  static lpad(s, count, with) {
    if (s.count < count) {
      s = "%(with * (count-s.count))%(s)"
    }
    return s
  }

  static escape(text, options) {
    var substrings = []
    var escapeSolidus = JSONOptions.contains(options, JSONOptions.escapeSolidus)
    for (char in text) {
      if (char == "\"") {
        substrings.add("\\\"")
      } else if (char == "\\") {
        substrings.add("\\\\")
      } else if (char == "\b") {
        substrings.add("\\b")
      } else if (char == "\f") {
        substrings.add("\\f")
      } else if (char == "\n") {
        substrings.add("\\n")
      } else if (char == "\r") {
        substrings.add("\\r")
      } else if (char == "\t") {
        substrings.add("\\t")
      } else if (char.bytes[0] <= 0x1f) {
        // Control characters!
        var byte = char.bytes[0]
        var hex = lpad(toHex(byte), 4, "0")
        substrings.add("\\u" + hex)
      } else if (escapeSolidus && char == "/") {
        substrings.add("\\/")
      } else {
        substrings.add(char)
      }
    }
    return substrings.join("")
  }
}

class JSONEncoder {
  construct new(options) {
    _options = options
    _circularStack = JSONOptions.contains(options, JSONOptions.checkCircular) ? [] : null
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

    if (value is Num || value is Bool || value is Null) {
      return value.toString
    }

    if (value is String) {
      return "\"" + JSONEscapeChars.escape(value, _options) + "\""
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

    // Check if the object implements toJSON
    if (value is JSONEncodable) {
      return value.toJSON
    }

    // Default behaviour is to invoke the toString method
    return value.toString
  }
}

class JSON {

  static encode(value, options) { JSONEncoder.new(options).encode(value) }

  static encode(value) {
    return JSON.encode(value, JSONOptions.abortOnError)
  }

  static stringify(value) {
    return JSON.encode(value)
  }

  static decode(value, options) {
    var stream = JSONStream.new(value, options)
    stream.begin()

    var result = stream.result
    if (stream.error.found) {
      result = stream.error
    }

    stream.end()
    return result
  }

  static decode(value) {
    return JSON.decode(value, JSONOptions.abortOnError)
  }

  static parse(value) {
    return JSON.decode(value)
  }
}
