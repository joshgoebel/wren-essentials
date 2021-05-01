import "../deps/wren-testie/testie" for Testie
import "../deps/wren-assert/Assert" for Assert
import "essentials:essentials" for Strings

Testie.new("Strings") { |it, skip|
    it.should("upcase") {
        Assert.equal("FORCE",Strings.upcase("force"))
    }
    it.should("downcase") {
        Assert.equal("mary had a little lamb", Strings.downcase("MARY had a LiTtlE lamb"))
    }
}.run()