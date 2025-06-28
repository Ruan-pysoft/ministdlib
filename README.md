# Ruan's Mini Stdlib

A toy stdlib for c89 made for fun and learning
(and procrastinating studying math...)

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

To compile the stdlib simply run `./clib`.

To compile a program run `./cc $programname`.
Note that the `cc` script is just a thin wrapper over `gcc`,
so you can pass any flags you would pass to `gcc` to `cc`.

Also note that the library is statically linked,
so you **must** recompile the library with `./clib`
after making any changes
and **before** compiling any c file using it.

You can run the tests with the command `./run-tests`.
All of them *should* theoretically pass,
except for `should-fail.c`, which should fail (duh).
Also, my test suite is pretty useless for now,
I gotta work on that...
(It did catch bugs a few times
when I forgot to initialise an `err` variable to `ERR_OK`,
so I guess it's not too useless)

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
I wrote every line of code with my own two fingers
(though I might've used some search-and-replace or copy-and-paste);
none of it is AI-generated,
though ChatGPT has generated some code in the chat,
and I have looked through a few if its examples,
so I might have used some of that code.

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
(I'll figure out how to get a bugtracker or something set up at some point)
and I'll take a look at it eventually.

And yes, atm this is rather poorly documented,
the testcases are a mess,
and there are files all over the place
(though most of the random ones should be in my `.gitignore`...),
I'll get around to fixing all that eventually...

## License

(Also, work out again how dual licensing works,
I know some countries don't play well
with dual licensing or public domain licenses)

I want to add the option for some public domain-equivalent license,
such as cc0 or bsd 0-clause.

But for now only MIT license, see the file `LICENSE`.
