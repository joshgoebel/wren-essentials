#module=essentials
class Strings {
    static upcase(s) {
        return s.bytes.map { |x|
            if ((97..122).contains(x)) x = x - 32
            return String.fromByte(x)
        }.join("")
    }
    static downcase(s) {
        return s.bytes.map { |x|
            if ((65..90).contains(x)) x = x + 32
            return String.fromByte(x)
        }.join("")
    }
}
