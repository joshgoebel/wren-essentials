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