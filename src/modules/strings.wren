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

    /************************************************************************
     * String matching with wildcards '*' and '?'
     * Algorithm from https://www.geeksforgeeks.org/wildcard-pattern-matching/
     */
    static simpleMatch(s, pattern) {
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

    /************************************************************************
     * Adapting the C implementation of Tcl's `string match` command
     * https://github.com/tcltk/tcl/blob/core-8-6-11/generic/tclUtil.c#L2366
     *
     * Special characters that can appear in a pattern:
        *
            Matches any sequence of characters in string, including a null
            string.
        ?
            Matches any single character in string.
        [chars]
            Matches any character in the set given by chars. If a sequence
            of the form x-y appears in chars, then any character between x
            and y, inclusive, will match. Ranges can be forward ([a-z]) or
            backward ([z-a]).
        \x
            Matches the single character x. This provides a way of
            avoiding the special interpretation of the characters *?[]\ in
            pattern. In a wren string, the backslash itself must be escaped:
                Strings.globMatch("a*b", "a\\*b")   // expect: true
     *
     */
    static globMatch(s, pattern) { globMatchRec_(s, 0, pattern, 0) }

    static globMatchRec_(s, sidx, pattern, pidx) {
        var slen = s.bytes.count
        var plen = pattern.bytes.count
        var p
        var ch1
        var startChar
        var endChar

        while (true) {
            /* if we're at then end of the pattern,
             * we must also be at the end of the string
             */
            if (pidx == plen) {
                return sidx == slen
            }
            /* if we're at the end of the string,
             * match succeeds if the rest of the pattern is "*"
             */
            if (sidx == slen) {
                while (pidx < plen) {
                    if (pattern[pidx] != "*") {
                        return false
                    }
                    pidx = pidx + 1
                }
                return true
            }

            p = pattern[pidx]

            // "*" matches any substring
            if (p == "*") {
                // skip all successive "*"s
                while (pidx < plen && pattern[pidx] == "*") {
                    pidx = pidx + 1
                }
                if (pidx == plen) {
                    return true
                }
                p = pattern[pidx]
                while (true) {
                    if ((p != "[") && (p != "?") && (p != "\\")) {
                        while ((sidx < slen) && (p != s[sidx])) {
                            sidx = sidx + 1
                        }
                    }
                    if (globMatchRec_(s, sidx, pattern, pidx)) {
                        return true
                    }
                    if (sidx == slen) {
                        return false
                    }
                    sidx = sidx + 1
                }
            }

            // "?" matches any single character
            if (p == "?") {
                pidx = pidx + 1
                sidx = sidx + 1
                continue
            }

            // "[" in the pattern is followed by a list of characters or a range
            if (p == "[") {
                pidx = pidx + 1
                ch1 = s[sidx].bytes[0]
                sidx = sidx + 1
                while (true) {
                    if (pidx == plen) {
                        // pattern ended before bracket expression was closed
                        return false
                    }

                    // this construct handles escaped close brackets
                    if (pattern[pidx] == "\\") {
                        pidx = pidx + 1
                        if (pidx == plen) {
                            return false
                        }
                    } else if (pattern[pidx] == "]") {
                        return false
                    }

                    startChar = pattern[pidx].bytes[0]
                    pidx = pidx + 1
                    if (pidx < plen && pattern[pidx] == "-") {
                        pidx = pidx + 1
                        if (pidx == plen) {
                            // end of pattern in a range expression
                            return false
                        }
                        // handles escaped close brackets as range end char
                        if (pattern[pidx] == "\\") {
                            pidx = pidx + 1
                            if (pidx == plen) {
                                return false
                            }
                        } else if (pattern[pidx] == "]") {
                            // unended range
                            return false
                        }

                        endChar = pattern[pidx].bytes[0]
                        pidx = pidx + 1
                        // matching forward ranges [a-z] or backwards [z-a]
                        if ((startChar <= ch1 && ch1 <= endChar) ||
                                (endChar <= ch1 && ch1 <= startChar)) {
                            break
                        }
                    } else {
                        /* not in a range expression, test the string's ch1
                         * against this char in the bracket expr
                         */
                        if (startChar == ch1) {
                            break
                        }
                    }
                }
                if (pidx == plen) {
                    // matched the character, but the bracket expr is unclosed
                    return false
                }
                while (pattern[pidx] != "]") {
                    pidx = pidx + 1
                }
                pidx = pidx + 1
                continue
            }

            /* if the next character is "\\", just remove it
             * so we do exact matching on the char that follows
             */
            if (p == "\\") {
                pidx = pidx + 1
                if (pidx == plen) {
                    return false
                }
            }

            // there's no special character. Make sure these match
            if (s[sidx] != pattern[pidx]) {
                return false
            }

            pidx = pidx + 1
            sidx = sidx + 1
        }
    }
}

var LOWERCASE_A = "a".bytes[0]
var LOWERCASE_Z = "z".bytes[0]
var UPPERCASE_A = "A".bytes[0]
var UPPERCASE_Z = "Z".bytes[0]
