all:    mute

mute:   mute.o

mute.o: mute.c

clean:
	rm -f mute.o mute

