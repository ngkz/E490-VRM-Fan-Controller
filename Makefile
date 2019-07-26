MCU             = attiny85
FREQ            = 1000000
TARGET          = vrmfanctl
SRCS            = vrmfanctl.c
CC              = avr-gcc
CFLAGS          = -g -Os \
                  -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
                  -Wall -Wstrict-prototypes -Wextra -Werror \
                  -std=gnu99 \
                  -mmcu=$(MCU)
CPPFLAGS        = -DF_CPU=$(FREQ)
LDFLAGS         = -Wl,-Map=$(MAP),--cref
OBJDUMP         = avr-objdump
SIZE            = avr-size
AVRDUDE         = avrdude
PROGRAMMER      = buspirate
PROGRAMMER_PORT = /dev/ttyUSB0
AVRDUDE_FLAGS   = -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -p $(MCU)

MAP             = $(TARGET).map
LIST            = $(TARGET).lss
OBJS            = $(SRCS:.c=.o)
DEPS            = $(SRCS:.c=.d)

all: $(TARGET) $(LIST) size

$(TARGET): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $(CPPFLAGS) $<

$(LIST): $(TARGET)
	$(OBJDUMP) -h -S $< > $@

size: $(TARGET)
	$(SIZE) --format=avr --mcu=$(MCU) $(TARGET)

program: all
	 $(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:$(TARGET) -U hfuse:w:$(TARGET) -U efuse:w:$(TARGET) -U flash:w:$(TARGET)

clean:
	$(RM) $(TARGET) $(MAP) $(LIST) $(OBJS) $(DEPS)

%.d: %.c
	@rm -f $@ && \
		$(CC) -MM $(CFLAGS) $(CPPFLAGS) $< > $@.$$$$ && \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		code=$$?; \
		rm -f $@.$$$$ && exit $$code;

-include $(DEPS)

.PHONY: all size program clean
