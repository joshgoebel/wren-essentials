#module=essentials
class Strings {
    static upcase(s) {
        return s.bytes.map { |x|
            if ((LOWERCASE_A..LOWERCASE_Z).contains(x)) x = x - 32
            return String.fromByte(x)
        }.join("")
    }
    static downcase(s) {
        return s.bytes.map { |x|
            if ((UPPERCASE_A..UPPERCASE_Z).contains(x)) x = x + 32
            return String.fromByte(x)
        }.join("")
    }
    static capitalize(s) {
        if (s.isEmpty) return ""
        if (s.count == 1) return Strings.upcase(s)
        return Strings.upcase(s[0]) + s[1..-1]
    }
    static titlecase(s) {
        return s.split(" ").map {|w| capitalize(w) }.join(" ")
    }
}

var LOWERCASE_A = "a".bytes[0]
var LOWERCASE_Z = "z".bytes[0]
var UPPERCASE_A = "A".bytes[0]
var UPPERCASE_Z = "Z".bytes[0]
