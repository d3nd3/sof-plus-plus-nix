SHELL := /bin/bash

# -D _GNU_SOURCE for RTLD_NEXT
# -D WSL ( Clipboard WSL )
# -D DEBUG ( DEBUG PRINT CALLS )

CC=g++

INSTALL_DIR:=$(HOME)/.loki/sof-runtime/

BINARY:=test.so

# ensure exists obj dir
$(info $(shell mkdir -p ../obj))

# DEFAULT VARIABLE VALUES
# CAN ENABLE FEATURES HERE
USE_PYTHON:=1
PYTHON_VERSION?=3.8
USE_DEMOSYSTEM:=1

OBJS:= \
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
../obj/httpdl.o \
../obj/detect.o \
../obj/video.o \
../obj/weapon_balance.o \
../obj/util.o \
../obj/strip.o \
../obj/dm_always.o \
../obj/deathmatch.o \
../obj/dm_sofree.o \
../obj/dm_standard.o \
../obj/dm_ctf.o \
../obj/dm_realistic.o

ifeq ($(USE_DEMOSYSTEM),1)
OBJS += ../obj/demos.o
CFLAGS_DEMO := -D DEMOSYSTEM
endif

CFLAGS:= -O2 -fomit-frame-pointer -std=c++17 -m32 -fpic -fpermissive -w -D _GNU_SOURCE -D_inline=inline -I ../hdr -I /lib/ $(CFLAGS_DEMO)

# Statically link libstdc++ and libgcc to avoid GLIBCXX symbol errors on target servers
LDFLAGS:= -m32 -shared -static-libstdc++ -static-libgcc -L /minizip/build
LINKUS:= -lcurl -lX11 -Wl,-Bstatic -lstdc++fs -lminizip -Wl,-Bdynamic -lcrypt -lpthread -ldl -lutil -lm

# Force 32-bit pkg-config path query for Python
PYTHON_CFLAGS := $(shell PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig pkg-config --cflags python$(PYTHON_VERSION) 2>/dev/null)
ifeq ($(PYTHON_CFLAGS),)
  PYTHON_CFLAGS := $(shell PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig pkg-config --cflags python3 2>/dev/null | sed 's/x86_64-linux-gnu/i386-linux-gnu/g')
endif

ifeq ($(USE_PYTHON),1)
  ifeq ($(PYTHON_CFLAGS),)
    ifneq (clean,$(MAKECMDGOALS))
      $(error USE_PYTHON=1 but 32-bit Python headers not found. Install libpython3-dev:i386 or build with USE_PYTHON=0)
    endif
  endif
  OBJS += ../obj/python.o ../obj/entity.o ../obj/ppnix.o ../obj/player.o ../obj/decorators.o ../obj/memory.o ../obj/pages.o ../obj/killfeed.o
  CFLAGS += $(PYTHON_CFLAGS) -DUSE_PYTHON
  LDFLAGS += -L../build_python
  
  # Dynamically static link libpython3.9.a or libpython3.8.a based on PYTHON_VERSION flag
  LINKUS += -Wl,-Bstatic -lpython$(PYTHON_VERSION) -Wl,-Bdynamic
endif


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


#PYTHON DEPENDENDANT PARTS
ifeq ($(USE_PYTHON),1)

../obj/player.o : ./python_api/player.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/entity.o : ./python_api/entity.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/ppnix.o : ./python_api/ppnix.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/decorators.o : ./python_api/decorators.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/memory.o : ./python_api/memory.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

../obj/pages.o : ./features/pages.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/killfeed.o : ./features/killfeed.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/python.o : ./features/python.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

endif

# game_modes
../obj/deathmatch.o : ./game_modes/deathmatch.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/dm_realistic.o : ./game_modes/dm_realistic.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/dm_ctf.o : ./game_modes/dm_ctf.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/dm_standard.o : ./game_modes/dm_standard.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/dm_sofree.o : ./game_modes/dm_sofree.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/dm_always.o : ./game_modes/dm_always.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

# features
../obj/detect.o : ./features/detect.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/clipboard.o : ./features/clipboard.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/serverlist.o : ./features/serverlist.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/httpdl.o : ./features/httpdl.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/weapon_balance.o : ./features/weapon_balance.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/demos.o : ./features/demos.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

# core
../obj/exe_shared.o : ./core/exe_shared.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_server.o : ./core/exe_server.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/exe_client.o : ./core/exe_client.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/game.o : ./core/game.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/video.o : ./core/video.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

# normal
../obj/commands.o : ./commands.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/cvars.o : ./cvars.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/fn_sigs.o : ./fn_sigs.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/main.o : ./main.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/util.o : ./util.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
../obj/strip.o : ./strip.cpp
	$(CC) $(CFLAGS) -c $^ -o $@


.PHONY: clean
clean:
	rm -f ../obj/* >/dev/null 2>&1 || true
	rm -f ../out/* >/dev/null 2>&1 || true
