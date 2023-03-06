#                       +-------------------------------------------------------------------- +
#                       |                                                                     |
#                       | If you're just using the templates, there's nothing to look at here |
#                       |                                                                     |
#                       +---------------------------------------------------------------------+

# Yes, this is hacky and doesn't allow anything to be cached. I'm not gonna do the research to figure that one out for programs of this scale.
# Merge requests welcome though

build:
	-mkdir debug
	-clear
	-cls
	arm-none-eabi-gcc -c src/*.c -g -O0 -w -mthumb -march=armv7-m -mno-unaligned-access -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -std=gnu17 -I. -I./libmd407
	arm-none-eabi-g++ -o debug/MOP -L. *.o  -lgcc -lc_nano  -Tmd407-ram.x  -nostartfiles 
	arm-none-eabi-objcopy -S -O srec  debug/MOP debug/MOP.s19
	arm-none-eabi-objdump -D -S debug/MOP > debug/MOP.dass
	-rm *.o
	-del *.o
	echo Success
