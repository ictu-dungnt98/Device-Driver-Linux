.PHONY : clean

SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)

TARGET = test2

CC := /home/dungnt98/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc


all: $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)
$(OBJECTS):
	$(CC) -c $(SOURCES)

clean: 
	rm -f $(OBJECTS) $(TARGET)
