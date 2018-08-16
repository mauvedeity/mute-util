# mute-util

A program to allow temporary muting of programs, so that you have
an alternative to editing crontab.

## Introduction

The idea is that rather than having to edit my `crontab` manually all
of the time, I can just have a program that will cause the job to not
run until a specific date. Thus if I'm away on holiday, for instance,
useful "at home" jobs won't run until I'm back.

## Mechanism

~~The main idea is to take advantage of lazy evaluation in the shell. Thus
we can have one command line that looks like this:~~

~~This expression evaluates to the first result `OR` the second one. If
the first is true, the shell doesn't bother to run the second command,
because the result of the whole command line must be `true`. If the first
command evaluates to `false`, then the shell runs the second command, and
the result is the result of that command. This is the lazy evaluation.~~

However, that didn't work in `crontab`. I don't know why, it just
wouldn't work.  So I'm looking at another approach, which I hope might
actually work this time.  This approach is to have `mute` work rather like
`time` - that is, you specify per Usage below, and the command either
runs or it doesn't, based on what the mute status is. This means that
the command to change the date is different - see also Usage below.

## Time

The idea is that the mute command will check a date value in a
file, and if that date value is >= today, then it will ~~return
`true` (non-zero)~~ ignore the rest of its command line, and return
`EXIT_SUCCESS`. Alternatively, if the mute date has passed, then it
will execute the rest of the command line, and return the return value
of that command line.

## Usage

``` % mute ``` Displays current mute date and exits.

``` % mute cmd ```

If the current system date is before the mute date (found in YYYYMMDD
format in ~/.muterc) then it will ignore the command line and return

``` % mute -s YYYYMMDD ``` Sets the 'mute-until' date. This is currently
on backlog.

## TODO Oh, *lots* of stuff.

Ideally it should support multiple entries, with some kind of ID to tell
them apart, so you could have different mute schedules for different apps.

Ideally there should be an 'until further notice' option - but you can
fake this by setting the date to be `99990101` or similar.

The command should expose full management functionality, so you can add,
remove, update mute dates for different IDs as you please.
