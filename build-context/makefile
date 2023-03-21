
# -D _GNU_SOURCE for RTLD_NEXT
# -D WSL ( Clipboard WSL )

CC=g++

OBJS= \
../obj/clipboard.o \
../obj/commands.o \
../obj/exe.o \
../obj/fn_sigs.o \
../obj/game.o \
../obj/main.o \
../obj/serverlist.o \
../obj/util.o

CFLAGS=-m32 -fpic -fpermissive -w -D _GNU_SOURCE -I ../hdr
LDFLAGS= -m32 -shared -lX11 -o ../out/test.so

../out/test.so: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o ../out/test.so

../obj/clipboard.o : ../src/clipboard.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/commands.o : ../src/commands.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe.o : ../src/exe.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/fn_sigs.o : ../src/fn_sigs.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/game.o : ../src/game.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/main.o : ../src/main.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/serverlist.o : ../src/serverlist.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/util.o : ../src/util.cpp
	$(CC) $(CFLAGS) -c $^ -o $@


.PHONY: clean
clean:
	rm ../obj/* >/dev/null 2>&1 || true
	rm ../out/* >/dev/null 2>&1 || true