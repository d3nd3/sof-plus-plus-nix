SHELL := /bin/bash

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
../obj/httpdl.o \
../obj/detect.o \
../obj/dm_always.o \
../obj/deathmatch.o \
../obj/dm_sofree.o \
../obj/dm_standard.o \
../obj/dm_ctf.o \
../obj/dm_realistic.o

CFLAGS= -m32 -fpic -fpermissive -w -D DEBUG -D _GNU_SOURCE -D_inline=inline -I ../hdr -I /lib/
LDFLAGS= -m32 -shared -o ../out/test.so -L /minizip/build
LINKUS= -lcurl -lX11 -Wl,-Bstatic -lminizip -Wl,-Bdynamic

# Name of text file containing build number.
BUILD_NUMBER_FILE=buildnum
BUILD_NUMBER_LDFLAGS  = -Wl,--defsym=__BUILD_YEAR=$$((`date +'%Y'`))
BUILD_NUMBER_LDFLAGS += -Wl,--defsym=__BUILD_MONTH=$$((10\#`date +'%m'` + 16))
BUILD_NUMBER_LDFLAGS += -Wl,--defsym=__BUILD_DAY=$$((10\#`date +'%d'` + 64))

BUILD_NUMBER_LDFLAGS += -Wl,--defsym=__BUILD_NUMBER=$$(cat $(BUILD_NUMBER_FILE))



# run recipe if ../out/binary does not exist or ../out/binaryDate >= installDirBinaryDate
$(INSTALL_DIR)$(BINARY): ../out/$(BINARY)
	@echo COPYING $(BINARY) TO $(INSTALL_DIR)
	cp $^ $@

../out/$(BINARY): $(OBJS)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)

	$(CC) $(LDFLAGS) $(BUILD_NUMBER_LDFLAGS) $(OBJS) -o ../out/test.so $(LINKUS)

../obj/dm_realistic.o : ./dm_realistic.cpp
	$(CC) $(CFLAGS) -c $^ -o $@


../obj/dm_ctf.o : ./dm_ctf.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

../obj/dm_standard.o : ./dm_standard.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

../obj/dm_sofree.o : ./dm_sofree.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

../obj/deathmatch.o : ./deathmatch.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/dm_always.o : ./dm_always.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/detect.o : ./detect.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/clipboard.o : ./clipboard.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/commands.o : ./commands.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/cvars.o : ./cvars.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_shared.o : ./exe_shared.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_server.o : ./exe_server.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_client.o : ./exe_client.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/fn_sigs.o : ./fn_sigs.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/game.o : ./game.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/httpdl.o : ./httpdl.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/main.o : ./main.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/serverlist.o : ./serverlist.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/util.o : ./util.cpp
	$(CC) $(CFLAGS) -c $^ -o $@


.PHONY: clean
clean:
	rm ../obj/* >/dev/null 2>&1 || true
	rm ../out/* >/dev/null 2>&1 || true
