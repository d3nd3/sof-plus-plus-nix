
# -D _GNU_SOURCE for RTLD_NEXT
# -D WSL ( Clipboard WSL )
# -D DEBUG ( DEBUG PRINT CALLS )

CC=g++

INSTALL_DIR=$(HOME)/.loki/sof-runtime/

BINARY=test.so

OBJS= \
../obj/clipboard.o \
../obj/commands.o \
../obj/cvars.o \
../obj/exe_shared.o \
../obj/exe_server.o \
../obj/exe_client.o \
../obj/fn_sigs.o \
../obj/game.o \
../obj/main.o \
../obj/serverlist.o \
../obj/util.o \
../obj/httpdl.o

CFLAGS= -m32 -fpic -fpermissive -w -D DEBUG -D _GNU_SOURCE -D_inline=inline -I ../hdr -I /lib/
LDFLAGS= -m32 -shared -o ../out/test.so -L /minizip/build
LINKUS= -lcurl -lX11 -Wl,-Bstatic -lminizip -Wl,-Bdynamic

# run recipe if binary does not exist
$(INSTALL_DIR)$(BINARY): ../out/$(BINARY)
	cp $^ $@

../out/$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o ../out/test.so $(LINKUS)

../obj/clipboard.o : ../src/clipboard.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/commands.o : ../src/commands.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/cvars.o : ../src/cvars.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_shared.o : ../src/exe_shared.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_server.o : ../src/exe_server.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_client.o : ../src/exe_client.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/fn_sigs.o : ../src/fn_sigs.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/game.o : ../src/game.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/httpdl.o : ../src/httpdl.cpp
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
