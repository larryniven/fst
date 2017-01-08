CXXFLAGS += -I ../
AR = gcc-ar

.PHONY: all clean

all: libfst.a

clean:
	-rm libfst.a
	-rm *.o

libfst.a: fst.o
	$(AR) rcs $@ $^
