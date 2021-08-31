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

    /* String matching with wildcards '*' and '?'
     * Algorithm from https://www.geeksforgeeks.org/wildcard-pattern-matching/
     */
    static strmatch(s, pattern) {
        // empty pattern can only match with empty string
        if (pattern.isEmpty) return s.isEmpty

        // only non-empty pattern an empty string can match is "*"
        if (s.isEmpty) return pattern == "*"

        var n = s.count
        var m = pattern.count

        // lookup table for storing results of subproblems
        var lookup = (0..n).reduce([]) {|l, i| l + [[false] * (m+1)]}

        // empty pattern can match with empty string
        lookup[0][0] = true

        // Only '*' can match with empty string
        for (j in (1..m)) {
            if (pattern[j - 1] == "*") {
                lookup[0][j] = lookup[0][j - 1]
            }
        }

        // fill the table in bottom-up fashion
        for (i in (1..n)) {
            for (j in (1..m)) {

                // Two cases if we see a '*'
                // a) We ignore ‘*’ character and move to next character in the
                //    pattern, i.e., ‘*’ indicates an empty sequence.
                // b) '*' character matches with ith character in input
                if (pattern[j - 1] == "*") {
                    lookup[i][j] = lookup[i][j - 1] || lookup[i - 1][j]

                // Current characters are considered as matching in two cases
                // a) current character of pattern is '?'
                // b) characters actually match
                } else if (pattern[j - 1] == "?" || s[i - 1] == pattern[j - 1]) {
                    lookup[i][j] = lookup[i - 1][j - 1]

                // If characters don't match
                } else {
                    lookup[i][j] = false
                }
            }
        }

        return lookup[n][m]
    }
}

var LOWERCASE_A = "a".bytes[0]
var LOWERCASE_Z = "z".bytes[0]
var UPPERCASE_A = "A".bytes[0]
var UPPERCASE_Z = "Z".bytes[0]
