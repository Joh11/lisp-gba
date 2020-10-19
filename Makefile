CC = arm-none-eabi-gcc
CFLAGS = -mthumb-interwork -mthumb
# CFLAGS += -O3 # optimization
CFLAGS += -g # debug
LDFLAGS = -mthumb-interwork -mthumb -specs=gba.specs

OBJECTS = main.o input.o keyboard.o image.o

all: main.gba

clean:
	rm -f *.o main.elf main.gba

exec: main.gba
	mgba-qt main.gba &

main.gba: main.elf
	arm-none-eabi-objcopy -v -O binary $< $@
	gbafix $@

main.elf: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: all clean exec
