# wren-essentials

Wren essentials is effectively the "system lib" of sorts that powers
[wren-console](https://github.com/joshgoebel/wren-console/), the "batteries
included" Wren CLI.  Wren Console strives to have *critical* things and
everything else that seems important (that can't just be handled by a 3rd party
pure-Wren library) ends up here.

## Origin

Originally the idea was you'd build this library separately and link to it at
runtime.  This still works, but the focus lately have shifted to just building
it beside (inside) `wren-console` and that is how all development and testing
work is traditionally done.

AFAIK, the dynamic loading support still works on Mac, but it's not really going
anywhere until it gets more buy in on the Wren official side I don't think.
(see `loadLibrary` in `resolved.wren` in the `wren-console` project).

https://github.com/wren-lang/wren-cli/issues/52

This is very much a Proof of Concept / work in progress.

## How it works

Need to expand this.

### Magic comments for module inclusion

Magic comments in the Wren source determine which module the source will be
compiled into. For example in `strings.wren` you will find:

```js
// The next line is magic comment build directive.
//#module=essentials
import "ensure" for Ensure

class Strings {
// ...
```

Current `//#module=` is the only directive supported. This will ensure that the
`Strings` class is compiled into the `essentials` module such that it can be
referenced via traditional import:

```js
import "essentials" for Strings
```

### CREDIT / Contributions

- Mirror API thanks to [excellent work](https://github.com/mhermier/wren/commits/mirror) by [Michel Hermier][]
- And several other [amazing contributors](https://github.com/joshgoebel/wren-essentials/graphs/contributors)

### TODOS

- [ ] Clean up premake borrowed from Wren-CLI
- [ ] Generate the module registry dynamically from analyzing the module source
- [x] get the registry headers into `wren-cli` so we can make it a dependency (right now they are copied into `essentials.h`)


[Michel Hermier]: https://github.com/mhermier