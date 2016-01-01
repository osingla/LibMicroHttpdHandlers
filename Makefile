
.ONESHELL:

CXX=${CROSS_COMPILE}g++
LD=${CROSS_COMPILE}ld

vpath %.c   examples/:$(VPATH)
vpath %.cpp examples/:$(VPATH)
vpath %.h   examples/:$(VPATH)
vpath %.o   examples/:$(VPATH)

VPATH = ./

CXXFLAGS = -Wall -g -O0 -std=gnu++0x -I../ -I./
LDFLAGS = -l microhttpd 

SOLIB = libmicrohttpdhandlers.so

EXAMPLE1 = example1
OBJEX1 = examples/example1.o

.PHONY: clean mrproper dojo

all: $(SOLIB) $(EXAMPLE1) dojo

$(EXAMPLE1): $(OBJEX1) $(SOLIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(SOLIB) -l microhttpd 

clean:
	rm -f *.o *.so *~ $(EXAMPLE1)

mrproper:
	make clean
	rm -rf examples/dojo*

# Pattern rules:

%.o: %.cpp LibMicroHttpdHandlers.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@
        
$(SOLIB): LibMicroHttpdHandlers.o
	$(LD) -shared -soname $@ -o $@ $< $(LDFLAGS)

example1.o: example1.cpp LibMicroHttpdHandlers.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


dojo: examples/dojo

examples/dojo:
	rm -f dojo*
	cd examples
	wget http://download.dojotoolkit.org/release-1.10.4/dojo-release-1.10.4.tar.gz
	tar -f dojo-release-1.10.4.tar.gz -x
	ln -s dojo-release-1.10.4 dojo
