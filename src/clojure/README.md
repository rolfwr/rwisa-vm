# Clojure implementation of rwisa-vm (rwa2)

Implementation by: [Stian Eikeland](https://twitter.com/stianeikeland)

```
$ lein run 3-test.rwa2
```

Or run from REPL (an UI terminal is prefered, as the REPL owns stdio, you might
be able to get around it using `lein trampoline`)..

```
(run-vm (->vm (file->byte-vec "3-test.rwa2")
              (terminal/->lanterna-terminal :swing)))
```