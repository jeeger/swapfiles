# swap – Swap two files atomically

There's no shell utility for swapping two files or folders on Linux, even though it's possible with
the `renameat2()` syscall. This is a simple utility that uses the syscall to (almost) atomically
swap two files.

I'm saying "almost" because there is a short period between getting the absolute path of the files
with `realpath()` and swapping. In this interval, the file might be rewritten by someone else. It's
still less complicated than using a third temporary file though.

Currently, functionality is intentionally limited, but it should be a simple matter to extend the
program with more functionality.

## Installation

`autoreconf -i && ./configure && make && make install` should put the command into your path as
`swap`.

## Use

Call with `swap file1 file2`. That's it.
