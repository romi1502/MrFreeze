
# compiler
CXX ?= g++

# flags
#  -I../Shared_files
CXXFLAGS += -O3 -ffast-math -Wall -fPIC -DPIC $(shell pkg-config --cflags fftw3f) -I. -std=c++11
LDFLAGS += -shared $(shell pkg-config --libs fftw3f) -larmadillo -lm -lfftw3f

ifneq ($(NOOPT),true)
CXXFLAGS += -mtune=generic -msse -msse2 -mfpmath=sse
endif

# remove command
RM = rm -f

# plugin name
PLUGIN = mod-$(shell basename $(shell pwd) | tr A-Z a-z)
PLUGIN_SO = $(PLUGIN).so

# effect path
EFFECT_PATH = $(PLUGIN).lv2

# installation path
ifndef INSTALL_PATH
INSTALL_PATH = /Library/Audio/Plug-Ins/LV2
endif
INSTALLATION_PATH = $(DESTDIR)$(INSTALL_PATH)/$(EFFECT_PATH)

# sources and objects
# $(wildcard ../Shared_files/*.cpp)
SRC = $(wildcard src/*.cpp) $(wildcard ../freeze_engine/*.cpp)
OBJ = $(SRC:.cpp=.o)

## rules
all: $(PLUGIN_SO)

$(PLUGIN_SO): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	$(RM) *.so src/*.o

install: all
	mkdir -p $(INSTALLATION_PATH)
	cp -r $(PLUGIN_SO) ttl/* $(INSTALLATION_PATH)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@
