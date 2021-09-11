#include "wren.h"

int TclByteArrayMatch(
    const char *string,
    int strLen,
    const char *pattern,
    int ptnLen,
    int unused
);

void stringsGlobMatch(WrenVM *vm) {
    const char* string = wrenGetSlotString(vm, 1);
    int strLen = (int)wrenGetSlotDouble(vm, 2);
    const char* pattern = wrenGetSlotString(vm, 3);
    int ptnLen = (int)wrenGetSlotDouble(vm, 4);
    int result = TclByteArrayMatch(string, strLen, pattern, ptnLen, 0);
    wrenSetSlotBool(vm, 0, result);
}

/*
 * tclUtil.c --
 *
 *	This file contains utility functions that are used by many Tcl
 *	commands.
 *
 * Copyright © 1987-1993 The Regents of the University of California.
 * Copyright © 1994-1998 Sun Microsystems, Inc.
 * Copyright © 2001 Kevin B. Kenny. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
// https://github.com/tcltk/tcl/blob/core-8-6-11/license.terms
// https://github.com/tcltk/tcl/blob/core-8-6-11/generic/tclUtil.c#L2366

int
TclByteArrayMatch(
    const char *string,/* String. */
    int strLen,                 /* Length of String */
    const char *pattern,
                                /* Pattern, which may contain special
                                 * characters. */
    int ptnLen,                 /* Length of Pattern */
    int unused
)
{
    const char *stringEnd, *patternEnd;
    char p;

    stringEnd = string + strLen;
    patternEnd = pattern + ptnLen;

    while (1) {
        /*
         * See if we're at the end of both the pattern and the string. If so,
         * we succeeded. If we're at the end of the pattern but not at the end
         * of the string, we failed.
         */

        if (pattern == patternEnd) {
            return (string == stringEnd);
        }
        p = *pattern;
        if ((string == stringEnd) && (p != '*')) {
            return 0;
        }

        /*
         * Check for a "*" as the next pattern character. It matches any
         * substring. We handle this by skipping all the characters up to the
         * next matching one in the pattern, and then calling ourselves
         * recursively for each postfix of string, until either we match or we
         * reach the end of the string.
         */

        if (p == '*') {
            /*
             * Skip all successive *'s in the pattern.
             */

            while ((++pattern < patternEnd) && (*pattern == '*')) {
                /* empty body */
            }
            if (pattern == patternEnd) {
                return 1;
            }
            p = *pattern;
            while (1) {
                /*
                 * Optimization for matching - cruise through the string
                 * quickly if the next char in the pattern isn't a special
                 * character.
                 */

                if ((p != '[') && (p != '?') && (p != '\\')) {
                    while ((string < stringEnd) && (p != *string)) {
                        string++;
                    }
                }
                if (TclByteArrayMatch(string, stringEnd - string,
                                pattern, patternEnd - pattern, 0)) {
                    return 1;
                }
                if (string == stringEnd) {
                    return 0;
                }
                string++;
            }
        }

        /*
         * Check for a "?" as the next pattern character. It matches any
         * single character.
         */

        if (p == '?') {
            pattern++;
            string++;
            continue;
        }

        /*
         * Check for a "[" as the next pattern character. It is followed by a
         * list of characters that are acceptable, or by a range (two
         * characters separated by "-").
         */

        if (p == '[') {
            char ch1, startChar, endChar;

            pattern++;
            ch1 = *string;
            string++;
            while (1) {
                if ((*pattern == ']') || (pattern == patternEnd)) {
                    return 0;
                }
                startChar = *pattern;
                pattern++;
                if (*pattern == '-') {
                    pattern++;
                    if (pattern == patternEnd) {
                        return 0;
                    }
                    endChar = *pattern;
                    pattern++;
                    if (((startChar <= ch1) && (ch1 <= endChar))
                            || ((endChar <= ch1) && (ch1 <= startChar))) {
                        /*
                         * Matches ranges of form [a-z] or [z-a].
                         */

                        break;
                    }
                } else if (startChar == ch1) {
                    break;
                }
            }
            while (*pattern != ']') {
                if (pattern == patternEnd) {
                    pattern--;
                    break;
                }
                pattern++;
            }
            pattern++;
            continue;
        }

        /*
         * If the next pattern character is '\', just strip off the '\' so we
         * do exact matching on the character that follows.
         */

        if (p == '\\') {
            if (++pattern == patternEnd) {
                return 0;
            }
        }

        /*
         * There's no special character. Just make sure that the next bytes of
         * each string match.
         */

        if (*string != *pattern) {
            return 0;
        }
        string++;
        pattern++;
    }
}

