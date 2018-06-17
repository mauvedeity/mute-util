# mute-util
A program to allow temporary muting of programs, so that you have
an alternative to editing crontab.

## Introduction

The idea is that rather than having to edit my `crontab` manually all of the time,
I can just have a program that will cause the job to not run until a specific
date. Thus if I'm away on holiday, for instance, useful "at home" jobs won't
run until I'm back.

## Mechanism

The main idea is to take advantage of lazy evaluation in the shell. Thus we can
have one command line that looks like this:
```
% mute || command
```
This expression evaluates to the first result `OR` the second one. If the first
is true, the shell doesn't bother to run the second command, because the result
of the whole command line must be `true`. If the first command evaluates to
`false`, then the shell runs the second command, and the result is the result of
that command. This is the lazy evaluation.

This works for `AND` as well - so you can use the command to run things *only*
during the muted date by using the `AND` operator instead:
```
% mute && command
```
This will run the command only when mute returns `true`, again because of lazy
evaluation. This means we don't need an 'inverse' option.

## Time

The idea is that the mute command will check a date value in a file, and if that
date value is >= today, then it will return `true` (non-zero). This will have
the effect of making the lazy evaluation ignore the second command. Otherwise,
it returns `false`, so that the shell will run the second command.

## Usage

```
% mute
```
Returns either true or false.

```
% mute YYYYMMDD
```
Sets the 'mute-until' date.

## TODO
Oh, *lots* of stuff.

Ideally it should support multiple entries, with some kind of ID to tell them
apart, so you could have different mute schedules for different apps. 

Ideally there should be an 'until further notice' option - but you can fake this
by setting the date to be `99990101` or similar.

You should be able to *dry-run* it, so it tells you what the current status is.
You should be able to ask what the mute date is (or if multiple ones, all of
them).

The command should expose full management functionality, so you can add, remove,
update mute dates for different IDs as you please.
