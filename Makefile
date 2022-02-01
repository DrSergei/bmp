.PHONY: all clean

all: bmp

CFLAGS:=-Iinclude -g -Wall -Wextra -Werror

obj/main.o: src/main.c include/bmp.h | obj
	gcc $(CFLAGS) -c $< -o $@

obj/bmp.o: src/bmp.c include/bmp.h | obj
	gcc $(CFLAGS) -c $< -o $@

obj/stego.o: src/stego.c include/bmp.h include/stego.h | obj
	gcc $(CFLAGS) -c $< -o $@

obj:
	mkdir obj

bmp: obj/main.o obj/bmp.o obj/stego.o
	gcc $^ -o $@ -lm

clean:
	rm -rf obj/bmp