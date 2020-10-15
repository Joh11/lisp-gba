CC = arm-none-eabi-gcc
CFLAGS = -mthumb-interwork -mthumb -O2
LDFLAGS = -mthumb-interwork -mthumb -specs=gba.specs

OBJECTS = main.o

all: main.gba

clean:
	rm -f *.o main.elf main.gba

exec: main.gba
	higan main.gba &

main.gba: main.elf
	arm-none-eabi-objcopy -v -O binary $< $@
	gbafix $@

main.elf: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: all clean exec
