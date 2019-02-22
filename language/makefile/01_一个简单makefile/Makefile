XX = gcc
CFLAGS = -g
CLIBS = -lpthread

INCLUDE = $(wildcard ./*h)
SOURCES = $(wildcard ./*.c)

TARGET = mainApp
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

$(TARGET) : $(OBJECTS)
	$(XX) $(CFLAGS) $^ -o $@ $(CLIBS)

$(OBJECTS) : %.o : %.c
	$(XX) -c $(CFLAGS) $< -o $@

.PYHONY : clean
clean:
	rm -rf *.o $(TARGET)
