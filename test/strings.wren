import "../deps/wren-testie/testie" for Testie
import "../deps/wren-assert/Assert" for Assert
import "essentials" for Strings

Testie.new("Strings") { |it, skip|
    it.should("upcase") {
        Assert.equal("FORCE",Strings.upcase("force"))
    }

    it.should("downcase") {
        Assert.equal("mary had a little lamb", Strings.downcase("MARY had a LiTtlE lamb"))
    }
    it.should("titlecase") {
        Assert.equal("", Strings.titlecase(""))
        Assert.equal("A", Strings.titlecase("a"))
        Assert.equal("A Frog", Strings.titlecase("a frog"))
        Assert.equal("Abc", Strings.titlecase("abc"))
        Assert.equal("Mcgregor", Strings.titlecase("mcgregor"))
        Assert.equal("Pride And Prejudice", Strings.titlecase("pride and prejudice"))
        Assert.equal(Strings.titlecase("PRIDE AND PREJUDICE"), "Pride And Prejudice")
    }

    it.should("globMatch: *") {
        Assert.ok(Strings.globMatch("abc", "ab*c"))
        Assert.ok(Strings.globMatch("abc", "ab**c"))
        Assert.ok(Strings.globMatch("abcdef", "ab*"))
        Assert.ok(Strings.globMatch("abc", "*c"))
        Assert.ok(Strings.globMatch("0123456789", "*3*6*9"))
        Assert.not(Strings.globMatch("01234567890", "*3*6*9"))
    }
    it.should("globMatch: ?") {
        Assert.ok(Strings.globMatch("abc", "a?c"))
        Assert.not(Strings.globMatch("abc", "a??c"))
        Assert.ok(Strings.globMatch("0123456789", "?1??4???8?"))
    }
    it.should("globMatch: []") {
        Assert.ok(Strings.globMatch("abc", "[abc]bc"))
    }
    it.should("globMatch: badly formed patterns") {
        Assert.not(Strings.globMatch("[]", "[]"))
        Assert.not(Strings.globMatch("[", "["))
    }
    it.should("globMatch: more []") {
        Assert.ok(Strings.globMatch("abc", "a[abc]c"))
        Assert.not(Strings.globMatch("abc", "a[xyz]c"))
        Assert.ok(Strings.globMatch("12345", "12[2-7]45"))
        Assert.ok(Strings.globMatch("12345", "12[ab2-4cd]45"))
        Assert.ok(Strings.globMatch("12b45", "12[ab2-4cd]45"))
        Assert.ok(Strings.globMatch("12d45", "12[ab2-4cd]45"))
        Assert.not(Strings.globMatch("12145", "12[ab2-4cd]45"))
        Assert.not(Strings.globMatch("12545", "12[ab2-4cd]45"))
    }
    it.should("globMatch: more [] forwards ranges") {
        Assert.not(Strings.globMatch("z", "[k-w]"))
        Assert.ok(Strings.globMatch("w", "[k-w]"))
        Assert.ok(Strings.globMatch("r", "[k-w]"))
        Assert.ok(Strings.globMatch("k", "[k-w]"))
        Assert.not(Strings.globMatch("a", "[k-w]"))
    }
    it.should("globMatch: more [] reverse ranges") {
        Assert.not(Strings.globMatch("z", "[w-k]"))
        Assert.ok(Strings.globMatch("w", "[w-k]"))
        Assert.ok(Strings.globMatch("r", "[w-k]"))
        Assert.ok(Strings.globMatch("k", "[w-k]"))
        Assert.not(Strings.globMatch("a", "[w-k]"))
    }
    it.should("globMatch: escaping") {
        Assert.ok(Strings.globMatch("a*b", "a\\*b"))
        Assert.not(Strings.globMatch("ab", "a\\*b"))
        Assert.ok(Strings.globMatch("a*?[]\\x", "a\\*\\?\\[\\]\\\\x"))
        // with raw strings
        Assert.ok(Strings.globMatch("""a*?[]\x""", """a\*\?\[\]\\x"""))
    }
    it.should("globMatch: empty string") {
        Assert.ok(Strings.globMatch("", "*"))
        Assert.ok(Strings.globMatch("", "**"))
        Assert.ok(Strings.globMatch("", "***"))
        Assert.not(Strings.globMatch("", "*."))
        Assert.ok(Strings.globMatch("", ""))
    }
    it.should("globMatch: badly formed patterns: unclosed bracket") {
        Assert.ok(Strings.globMatch("a", "[a"))
    }
    it.should("globMatch: badly formed patterns: unclosed range") {
        Assert.not(Strings.globMatch("Ax", "[A-]x"))
    }
    it.should("globMatch: close bracket in a bracket expr") {
          Assert.ok(Strings.globMatch("A]x", "A]x"))
          Assert.ok(Strings.globMatch("Ax", "[A-]]x"))
          Assert.ok(Strings.globMatch("Bx", "[A-]]x"))
          Assert.not(Strings.globMatch("hx", "[A-]]x"))
          Assert.ok(Strings.globMatch("hx", "[A-]h]x"))
          Assert.not(Strings.globMatch("_", "[_]]"))
          Assert.ok(Strings.globMatch("_]", "[_]]"))
          Assert.not(Strings.globMatch("_", "[]_]"))
          Assert.not(Strings.globMatch("_]", "[]_]"))
      }
}.run()
