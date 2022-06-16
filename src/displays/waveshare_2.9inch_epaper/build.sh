#!/bin/bash
#
TIMEFORMAT=%3lR
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
pushd $SCRIPTPATH > /dev/null

g++ -g -O -ffunction-sections -fdata-sections -Wall -c  examples/main.c -o bin/main.o -I ./lib/Config -I ./lib/GUI -I ./lib/e-Paper -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -D USE_BCM2835_LIB -D RPI -c  ./lib/Config/dev_hardware_SPI.c -o ./bin/dev_hardware_SPI.o -Wl,--gc-sections -lbcm2835 -lm -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -D USE_BCM2835_LIB -D RPI -c  ./lib/Config/RPI_sysfs_gpio.c -o ./bin/RPI_sysfs_gpio.o -Wl,--gc-sections -lbcm2835 -lm -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -D USE_BCM2835_LIB -D RPI -c  ./lib/Config/DEV_Config.c -o ./bin/DEV_Config.o -Wl,--gc-sections -lbcm2835 -lm -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/e-Paper/EPD_2in9_V2.c -o bin/EPD_2in9_V2.o -I ./lib/Config -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/GUI/GUI_BMPfile.c -o bin/GUI_BMPfile.o -I ./lib/Config -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/GUI/GUI_Paint.c -o bin/GUI_Paint.o -I ./lib/Config -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  examples/EPD_2in9_V2_test.c -o bin/EPD_2in9_V2_test.o -I ./lib/Config -I ./lib/GUI -I ./lib/e-Paper -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  examples/ImageData2.c -o bin/ImageData2.o -I ./lib/Config -I ./lib/GUI -I ./lib/e-Paper -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  examples/ImageData.c -o bin/ImageData.o -I ./lib/Config -I ./lib/GUI -I ./lib/e-Paper -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/Fonts/font12.c -o bin/font12.o -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/Fonts/font12CN.c -o bin/font12CN.o -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/Fonts/font16.c -o bin/font16.o -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/Fonts/font20.c -o bin/font20.o -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/Fonts/font24.c -o bin/font24.o -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/Fonts/font24CN.c -o bin/font24CN.o -D DEBUG
g++ -g -O -ffunction-sections -fdata-sections -Wall -c  lib/Fonts/font8.c -o bin/font8.o -D DEBUG
# gcc -g -O -ffunction-sections -fdata-sections -Wall -D RPI ./bin/EPD_2in9_V2.o ./bin/GUI_BMPfile.o ./bin/GUI_Paint.o ./bin/EPD_2in9_V2_test.o ./bin/ImageData2.o ./bin/ImageData.o ./bin/main.o ./bin/font12.o ./bin/font12CN.o ./bin/font16.o ./bin/font20.o ./bin/font24.o ./bin/font24CN.o ./bin/font8.o ./bin/dev_hardware_SPI.o ./bin/RPI_sysfs_gpio.o ./bin/DEV_Config.o -o epd -Wl,--gc-sections -lbcm2835 -lm -D DEBUG

popd
