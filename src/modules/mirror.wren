
class Mirror {
  static reflect(reflectee) {
    var mirror = ObjectMirror
    if (reflectee is Class) mirror = ClassMirror
    if (reflectee is Fiber) mirror = FiberMirror

    return mirror.new_(reflectee)
  }
}

class ObjectMirror is Mirror {
  foreign static canInvoke(reflectee, methodName)

  construct new_(reflectee) {
    _reflectee = reflectee
  }

  canInvoke(methodName) { ObjectMirror.canInvoke(_reflectee, methodName) }

  reflectee { _reflectee }

  type { ClassMirror.new_(_reflectee) }
}

class ClassMirror is ObjectMirror {
  foreign static allAttributes(reflectee)
  foreign static hasMethod(reflectee, methodName)
  foreign static methodNames(reflectee)

  construct new_(reflectee) {
    super(reflectee)
    _methods = ClassMirror.methodNames(reflectee)
  }

  attributes {
    var attr = ClassMirror.allAttributes(reflectee)
    return attr != null ? attr.self : null
  }

  hasMethod(methodName) { ClassMirror.hasMethod(reflectee, methodName) }

  methods { _methods }
}

class MethodMirror is Mirror {
  construct new_(class_, signature) {
    _class = class_
    _signature = signature
  }

  attributes {
    var attr = ClassMirror.allAttributes(_class)
    var methods = attr != null ? attr.methods : null
    return methods != null ? methods[signature] : null
  }

  signature { _signature }
}

class StackTraceAccumulator_ {
  construct new() {
    _stackTrace = []
  }

  call(fn, line) {
    _stackTrace.add("at %( fn.module.name ):%( fn.name ) line %( line )")
  }

  toString { _stackTrace.join("\n") }
}

class FiberMirror is ObjectMirror {
  foreign static functionAt_(reflectee, stackTraceIndex)
  foreign static lineAt_(reflectee, stackTraceIndex)
  foreign static stackFramesCount_(reflectee)

  construct new_(reflectee) {
    super(reflectee)
  }

  functionAt(stackTraceIndex) { FiberMirror.functionAt_(reflectee, stackTraceIndex) }
  lineAt_(stackTraceIndex)    { FiberMirror.lineAt_(reflectee, stackTraceIndex) }
  stackFramesCount            { FiberMirror.stackFramesCount_(reflectee) }

  fullStackTrace {
    var stackTraceAccumulator = StackTraceAccumulator_.new()
    fullStackTrace(stackTraceAccumulator)
    return stackTraceAccumulator.toString
  }

  fullStackTrace(cb) { fullStackTrace(cb) { true } }

  fullStackTrace(cb, filter) {
    for (stackTraceIndex in (stackFramesCount - 1)..0) {
      var fn   = functionAt(stackTraceIndex)
      var line = lineAt(stackTraceIndex)

      if (filter.call(fn, line)) {
        cb.call(fn, line)
      }
    }
    return cb
  }

  stackTrace {
    var stackTraceAccumulator = StackTraceAccumulator_.new()
    stackTrace(stackTraceAccumulator)
    return stackTraceAccumulator.toString
  }

  stackTrace(cb) {
    return fullStackTrace(cb) {|fn, line|
      Fiber.abort("IMPLEMENT ME")
    }
  }
}
