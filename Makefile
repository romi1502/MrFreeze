# compiler
CXX ?= g++

# flags
#  -I../Shared_files
CXXFLAGS += -O3 -ffast-math -Wall -fPIC -DPIC $(shell pkg-config --cflags fftw3f) -std=c++11 -I./src -I./Eigen
LDFLAGS += -shared $(shell pkg-config --libs fftw3f)

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
SRC = $(wildcard src/*.cpp) $(wildcard src/freeze_engine/*.cpp)
OBJ = $(SRC:.cpp=.o)

# wisdom
WISDOM_FILE = mrfreeze.wisdom

## rules
all: $(PLUGIN_SO)

$(PLUGIN_SO): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@
	# make $(WISDOM_FILE)

clean:
	$(RM) *.so src/*.o src/freeze_engine/*.o
	$(RM) $(WISDOM_FILE)

install: all
	mkdir -p $(INSTALLATION_PATH)
	cp -r $(PLUGIN_SO) ttl/* $(INSTALLATION_PATH)
	cp $(WISDOM_FILE) $(INSTALLATION_PATH)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

# $(WISDOM_FILE):
# 	@echo "Generating harmonizer.wisdom file, this might take a while..."
# 	fftwf-wisdom -n -x -o $@ \
# 	rof1024 rob1024 rof1536 rob1536 rof2048 rob2048 rof2176 rob2176 rof2304 rob2304 rof2432 rob2432 rof2560 rob2560 rof3072 rob3072 rof4096 rob4096
