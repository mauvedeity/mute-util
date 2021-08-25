# mute-util

A program to allow temporary muting of programs, so that you have
an alternative to editing crontab.

## Introduction

The idea is that rather than having to edit my `crontab` manually all
of the time, I can just have a program that will cause the job to not
run until a specific date. Thus if I'm away on holiday, for instance,
useful "at home" jobs won't run until I'm back.

## Mechanism

The idea is that the mute command will check a date value in a
file, and if that date value is >= today, then it will 
ignore the rest of its command line, and return
`EXIT_SUCCESS`. Alternatively, if the mute date has passed, then it
will execute the rest of the command line, and return the return value
of that command line.

## Usage

``` % mute ``` Displays current mute date and whether actions are 
currently muted or not.

``` % mute cmd ```

If the current system date is before the mute date (found in YYYYMMDD
format in ~/.muterc) then it will ignore the command line and return
`EXIT_SUCCESS`.

If the current system date is the mute date or after, then it will execute
the command line and then return. The return value will be the return value
of the command that it was asked to execute.

``` % mute -s YYYYMMDD ``` Sets the 'mute-until' date. This is being tracked as
(Issue 3)[https://github.com/mauvedeity/mute-util/issues/3] and
(Issue 12)[https://github.com/mauvedeity/mute-util/issues/12].

## TODO

I'm now tracking enhancements under (the issues tab)[https://github.com/mauvedeity/mute-util/issues].
This gives me a way to track the enhancements and perhaps get them under control.
