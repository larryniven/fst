CXXFLAGS += -I ../
AR = gcc-ar

obj = fst.o ifst.o

.PHONY: all clean

all: libfst.a

clean:
	-rm libfst.a
	-rm *.o

libfst.a: $(obj)
	$(AR) rcs $@ $(obj)

fst.o: fst.h fst-impl.h
ifst.o: ifst.h fst.h fst-impl.h
