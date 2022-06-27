DIR_BIN = ./bin

CC = g++

OBJ_C = $(wildcard src/*.cpp)
OBJ_O = $(patsubst %.cpp,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

HOST=$(shell cat /etc/hostname)

ifeq ($(HOST), rapi)
	defines = -D ON_RASPBERRY
	objs    = src/displays/waveshare_2.9inch_epaper/bin/*.o
	libs    = -lbcm2835 -lm
else
	defines = -g
    objs    =
    libs    =
endif

$(shell mkdir -p $(DIR_BIN))

${DIR_BIN}/%.o:src/%.cpp
	$(CC) $(defines) -c $< -o $@

yes:${OBJ_O}
	@echo $(HOST)
	$(CC) $(OBJ_O) $(defines) $(objs) -o tafel -lcurl $(libs)

clean:
	rm $(DIR_BIN)/*.*
	rm tafel
