LIB_DIR = ./../lib
TOP_DIR = ./..

SOURCES = $(wildcard ./*c)
INCLUDE_DIRS = -I$(TOP_DIR)/inc -I$(TOP_DIR)/response/ -I./

TARGET = libSendor.a
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

$(TARGET):$(OBJECTS)
	$(AR) $(ARFLAG) $@ $^
	cp $@ @(LIB_DIR)
$(OBJECTSA) : %.o : %.c
	$(XX) -c $(CFLAGS) $< -o $@ $(INCLUDE_DIRS)

.PHONY : clean
clean:
	rm -rf $(TARGET) $(OBJECTS)

