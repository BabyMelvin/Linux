XX = g++
AR = ar
ARFLAG = -rcs
CFLAGS = -g

CLIBS = -L./lib -lSender -lReponse -lpthread
SUBDIRS = ./resonser ./sender
INCLUDES = $(wildcard ./include/*.h ./sender/*.h ./responser/*.h)
INCLUDE_DIRS = -I./inc -I./sender -I./resonser

TARGET = mainApp
OBJECTS = main.o

export XX CFLAGS AR ARFLAG

$(TARGET) : $(OBJECTS) $(SUBDIRS)
	$(XX) $< -o $@ $(CLIBS)
$(SUBDIRS) : ECHO
	+$(MAKE) -C $@
ECHO:   #仅仅用来打印
	@echo $(SOBDIRS)
	@echo begin compile

.PHONY : clean
clean:
		for dir in $(SUBDIRS);\
		dor $(MAKE) -C $$dir clean||exit 1;\
		done
		rm -rf $(TARGET) $(OBJECTS) ./lib/*.a

