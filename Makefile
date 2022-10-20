GCCDIR = /usr/bin

CXX = $(GCCDIR)/g++
CXX = g++
# Flags for the C++ compiler: C++11 and all the warnings
CXXFLAGS = -std=c++11 -Wall -fno-rtti
# Workaround for an issue of -std=c++11 and the current GCC headers
CXXFLAGS += -Wno-literal-suffix

# Determine the plugin-dir and add it to the flags
PLUGINDIR=$(shell $(CXX) -print-file-name=plugin)
CXXFLAGS += -I$(PLUGINDIR)/include

LDFLAGS = -std=c++11

TESTFILEDIR = testfiles
TESTFILE = testfile

# top level goal: build our plugin as a shared library
all: cfg_plugin.so

cfg_plugin.so: cfg_plugin.o
	$(CXX) $(LDFLAGS) -shared -o $@ $<

cfg_plugin.o : cfg_plugin.cc
	$(CXX) $(CXXFLAGS) -fPIC -c -o $@ $<

clean:
	rm -f cfg_plugin.o cfg_plugin.so out err

run: cfg_plugin.so $(TESTFILEDIR)/$(TESTFILE).c
	$(CXX) -fplugin=./cfg_plugin.so -c $(TESTFILEDIR)/$(TESTFILE).c -o /dev/null 2>err 1>out
 
.PHONY: all clean check