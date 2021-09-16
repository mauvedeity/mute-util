# mute-util

A program to allow temporary muting of programs, so that you have an
alternative to editing crontab.

## Introduction

The idea is that rather than having to edit my `crontab` manually all of
the time, I can just have a program that will cause the job to not run
until a specific date. Thus if I'm away on holiday, for instance, useful
"at home" jobs won't run until I'm back.

I built this originally as a helper for
[traffic-robot](https://github.com/mauvedeity/traffic-robot), because I
kept forgetting to re-enable it when I got back from holiday. This is why
it works the way it does.

## Mechanism

The idea is that the mute command will check a date value in a file, and if
that date value is >= today, then it will ignore the rest of its command
line, and return `EXIT_SUCCESS`. Alternatively, if the mute date has
passed, then it will execute the rest of the command line, and return the
return value of that command line.

## Usage

``` % mute ``` Displays current mute date and whether actions are currently
muted or not. This will change - when I implement [Issue 20](https://github.com/mauvedeity/mute-util/issues/20) then `mute` with no parameters will show only whether actions are currently muted or not, and `mute -v` will show whether actions are muted or not, what the mute date is if in the future, and compile flag status.

``` % mute cmd ```

If the current system date is before the mute date (found in YYYYMMDD
format in ~/.muterc) then it will ignore the command line and return
`EXIT_SUCCESS`.

If the current system date is the mute date or after, then it will execute
the command line and then return. The return value will be the return value
of the command that it was asked to execute.

``` % mute -s YYYYMMDD ``` Sets the 'mute-until' date. This is being
tracked as [Issue 3](https://github.com/mauvedeity/mute-util/issues/3) and
[Issue 12](https://github.com/mauvedeity/mute-util/issues/12).

There are other issues that will change the usage of the command, listed on
[the issues tab](https://github.com/mauvedeity/mute-util/issues).

## TODO

I'm now tracking enhancements under [the issues
tab](https://github.com/mauvedeity/mute-util/issues). This gives me a way
to track the enhancements and perhaps get them under control.
