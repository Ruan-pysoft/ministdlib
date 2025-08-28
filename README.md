# Ruan's Mini Stdlib

A toy stdlib for c89 made for fun and learning
(and procrastinating studying math...)

Main repo: https://sr.ht/~ruan_p/ministdlib  
Official github mirror: https://github.com/Ruan-pysoft/ministdlib

Built with `-ffreestanding` and `-nostdlib` and 100% c89 code
(though the inline assembly parts might be a gcc extension?).
It is entirely self-contained and written from scratch by me.

Currently I'm only targeting my own machine
(Lenovo laptop running Arch Linux, compiling with gcc).
At some point in the future I might start targeting other Linux machines
(eg. my desktop running Mint)
or even other operating systems
(probably Windows 10).
But that's in the future.
For now, if it doesn't run on your computer,
good luck.
I might come back if you contact me or I might not,
but you're probably better off trying to fix it yourself.

This is NOT COMPLETE,
and the api is NOT STABLE,
DO NOT USE except for recreational code
that may BREAK AT ANY MOMENT.

(also the performance probably sucks)

For info on what I want to tackle next, see todo.txt.
Items with an exclamation point next to them
are considered "important" and I'll likely tackle those first.

## Demos

[![asciicast](https://asciinema.org/a/724079.svg)](https://asciinema.org/a/724079)

A quick demo of the `ministd_term.h` header.

[![asciicast](https://asciinema.org/a/724687.svg)](https://asciinema.org/a/724687)

A demo of `examples/sphere.c`.

## Usage

To compile the stdlib simply run `make`.

To compile a program run `./cc $programname`
(you must first compile the stdlib with `make MODE=release`)
or `./cc_dbg $programname` for a debuggable stdlib.
Note that the `cc` script is just a thin wrapper over `gcc`,
so you can pass any flags you would pass to `gcc` to `cc`.

Also note that the library is statically linked,
so you **must** recompile the library with `make`
after making any changes
and **before** compiling any c file using it.

You can run the tests with the command `make test`.
All of them *should* theoretically pass,
except for `should-fail.c`, which should fail (duh).
Also, my test suite is pretty useless for now,
I gotta work on that...
(It did catch bugs a few times
when I forgot to initialise an `err` variable to `ERR_OK`,
so I guess it's not too useless)

Lastly, you can compile the example programs with `make examples`,
and the compiled binaries can be found under
`build/debug/examples/bin` or `build/release/examples/bin`.

TODO :: Actually add some way to list/run examples from the makefile

## Acknowledgements

Many of the sources I've used are linked in `sources.txt`.
Honestly, I'm not sure how active I've been in maintaining that,
but whatever.

A big inspiration was the plan9 manpages, especially `string.h`.

I think this whole project started from [this](https://www.reddit.com/r/C_Programming/comments/1fzdwv9) reddit post,
because I wanted to do something similar,
in c89 for some reason,
and without using the stdlib for some further stupid reason...
I think ditching the stdlib came later? And by later I mean like an hour or two in.
As I recall, I was having trouble with the ANSI escape codes not working properly,
so I thought to make a library for that,
and while I'm at it I might as well make a library for IO as well,
and while I'm doing *that* I might as well write my own stdlib...
And then I only got a working sphere visualiser like a week later, lol.

I also took some inspiration for my design from [this](https://old.reddit.com/r/C_Programming/comments/feduq2) reddit post,
from which [this](https://old.reddit.com/r/C_Programming/comments/feduq2/comment/fjoj1ia) comment and [this](https://old.reddit.com/r/C_Programming/comments/feduq2/comment/fjoqv02) reply were huge inspirations for the IO model,
and I believe [this](https://old.reddit.com/r/C_Programming/comments/feduq2/comment/fjp3gj0) comment pointed me to plan9's c stdlib.

Lastly, I also used ChatGPT, mostly for idea generation and debugging.
I wrote every line of code with my own ten fingers
(though I might've used some search-and-replace or copy-and-paste);
none of it is AI-generated,
though ChatGPT has generated some code in the chat,
and I have looked through a few if its examples,
so I might have used some of that code.

The `Makefile` is mostly ChatGPT though,
this is a C program not a Make program,
so I don't mind using AI generation for that.

Regardless, you can view the chat [here](https://chatgpt.com/share/685b0e46-503c-8012-919d-b427104c1e9b).
I'll probably update that link as time goes on
to more recent versions of the chat.
Also, near the start I said I was working on it for two days,
but I believe that to be false,
I started the chat on a Friday or Saturday iirc,
and I've been working on the project since *at least* that Wednesday,
so I was closer to three or four days into the project.

## Contributing

In the end, this is a personal project
for my own enjoyment and learning,
so I want to do most of the programming myself.

However, if you do have a bugfix or suggestion,
feel free to contact me
and I'll take a look at it eventually.

You can create an issue at the [official github mirror](https://github.com/Ruan-pysoft/ministdlib/issues),
or you can create an issue on the issue tracker at [~ruan_p/ministdlib](https://todo.sr.ht/~ruan_p/ministdlib).
You can also discuss the project or send a patch on the [mailing list](https://lists.sr.ht/~ruan_p/ministdlib).

And yes, atm this is rather poorly documented,
the testcases are a mess,
and there are files all over the place
(though most of the random ones should be in my `.gitignore`...),
I'll get around to fixing all that eventually...

## Copying

This library is set free under the [Unlicense](https://unlicense.org/).
This means that it is released into the public domain
for all to use as they see fit.
Use it in a way I'll approve of,
use it in a way I won't,
doesn't make much of a difference to me.

I only ask (as a request in the name of common courtesy,
**not** as a legal requirement of any sort)
that you do not claim this work as your own
but credit me as appropriate.

The full terms are as follows:

```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>
```
