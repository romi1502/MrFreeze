# compiler
CXX ?= g++

# flags
#  -I../Shared_files
CXXFLAGS += -O3 -ffast-math -Wall -fPIC -DPIC $(shell pkg-config --cflags fftw3f) -std=c++11 -I. -I./Eigen
LDFLAGS += -shared $(shell pkg-config --libs fftw3f) -lm -lfftw3f

ifneq ($(NOOPT),true)
CXXFLAGS += -mtune=generic -msse -msse2 -mfpmath=sse
endif

# remove command
RM = rm -f

# plugin name
PLUGIN = mod-mrfreeze
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
SRC = $(wildcard src/*.cpp) $(wildcard freeze_engine/*.cpp)
OBJ = $(SRC:.cpp=.o)

## rules
all: $(PLUGIN_SO)

$(PLUGIN_SO): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	$(RM) *.so src/*.o freeze_engine/*.o

install: all
	mkdir -p $(INSTALLATION_PATH)
	cp -r $(PLUGIN_SO) ttl/* $(INSTALLATION_PATH)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@
