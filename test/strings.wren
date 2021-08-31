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
    }
}.run()
    it.should("strmatch") {
        Assert.ok(Strings.strmatch("", ""))
        Assert.not(Strings.strmatch("", "b"))
        Assert.ok(Strings.strmatch("", "*"))
        Assert.not(Strings.strmatch("", "?"))
        Assert.not(Strings.strmatch("baaabab", ""))
        Assert.ok(Strings.strmatch("baaabab", "*****ba*****ab"))
        Assert.ok(Strings.strmatch("baaabab", "ba*****ab"))
        Assert.ok(Strings.strmatch("baaabab", "ba*ab"))
        Assert.not(Strings.strmatch("baaabab", "a*ab"))
        Assert.not(Strings.strmatch("baaabab", "a*****ab"))
        Assert.ok(Strings.strmatch("baaabab", "*a*****ab"))
        Assert.ok(Strings.strmatch("baaabab", "ba*ab****"))
        Assert.ok(Strings.strmatch("baaabab", "****"))
        Assert.ok(Strings.strmatch("baaabab", "*"))
        Assert.not(Strings.strmatch("baaabab", "aa?ab"))
        Assert.ok(Strings.strmatch("baaabab", "b*b"))
        Assert.not(Strings.strmatch("baaabab", "a*a"))
        Assert.ok(Strings.strmatch("baaabab", "baaabab"))
        Assert.not(Strings.strmatch("baaabab", "?baaabab"))
        Assert.ok(Strings.strmatch("baaabab", "*baaaba*"))
    }
}.run()
