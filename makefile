SIM_ROOT = /media/fd538cb5-97ec-46b1-829f-6aaae01875e3/qi/trace_cache
SNIPER_TARGET_ARCH = intel64
PIN_HOME = ~/Documents/sniper_cycle/pin_kit

ifeq ($(SNIPER_TARGET_ARCH),ia32)
  # Add -march=i686 to enable some extra instructions that allow for implementation of 64-bit atomic adds
  CXXFLAGS += -m32 -march=i686 -DTARGET_IA32
  LD_FLAGS += -m32
endif
ifeq ($(SNIPER_TARGET_ARCH),intel64)
  CXXFLAGS += -fPIC -DTARGET_INTEL64
  LD_FLAGS +=
endif

CXXFLAGS += -DPIN_REV=$(shell $(SIM_ROOT)/tools/pinversion.py $(PIN_HOME) | cut -d. -f3)

include common/Makefile.common

ifneq ($(BOOST_INCLUDE),)
	CPPFLAGS += -I$(BOOST_INCLUDE)
endif

LD_LIBS += -lsift -lxed -L$(SIM_ROOT)/python_kit/$(SNIPER_TARGET_ARCH)/lib -lpython2.7 -lrt -lz -lsqlite3

LD_FLAGS += -L$(SIM_ROOT)/lib -L$(SIM_ROOT)/sift -L$(PIN_HOME)/extras/xed-$(SNIPER_TARGET_ARCH)/lib

ifneq ($(SQLITE_PATH),)
	CPPFLAGS += -I$(SQLITE_PATH)/include
	LD_FLAGS += -L$(SQLITE_PATH)/lib
endif

export LD_LIBRARY_PATH := $(SIM_ROOT)/lib:$(LD_LIBRARY_PATH)

DIRECTORIES := ${shell find $(SIM_ROOT)/common -type d -print} $(SIM_ROOT)/include

LIBCARBON_SOURCES = $(foreach dir,$(DIRECTORIES),$(wildcard $(dir)/*.cc)) $(wildcard $(SIM_ROOT)/common/config/*.cpp)

LIBCARBON_OBJECTS = $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(LIBCARBON_SOURCES) ))) 

OBJECTS = $(LIBCARBON_OBJECTS)
INCLUDE_DIRECTORIES = $(DIRECTORIES) $(PIN_HOME)/extras/xed-$(SNIPER_TARGET_ARCH)/include $(PIN_HOME)/extras/xed2-$(SNIPER_TARGET_ARCH)/include $(SIM_ROOT)/linux $(SIM_ROOT)/sift

CPPFLAGS+=$(foreach dir,$(INCLUDE_DIRECTORIES),-I$(dir)) -I$(SIM_ROOT)/python_kit/$(SNIPER_TARGET_ARCH)/include/python2.7 

CXXFLAGS+= -Wall -std=c++0x 

%.d: %.cc
	$(_MSG) '[DEP   ]' $(subst $(shell readlink -f $(SIM_ROOT))/,,$(shell readlink -f $@))
	$(_CMD) $(CXX) -MM -MG $(CPPFLAGS) $(CXXFLAGS) $< | sed -n "H;$$ {g;s@.*:\(.*\)@$*.o $@: \$$\(wildcard\1\)@;p}" >$@

%.o : %.cc
	$(_MSG) '[CXX   ]' $(subst $(shell readlink -f $(SIM_ROOT))/,,$(shell readlink -f $@))
	$(_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c  $< -o $@

obj:	$(OBJECTS)
	$(_MSG) '[AR    ]' $(subst $(shell readlink -f $(SIM_ROOT))/,,$(shell readlink -f $@))
	$(_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
lru: 
	g++ cache_lru.cc -o lru.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
plru: 
	g++ cache_plru.cc -o plru.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
sec: 
	g++ cache_sec.cc -o sector.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
sttsec: 
	g++ cache_sttsec.cc -o stt_sector.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
optimal: 
	g++ cache_optimal.cc -o optimal.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
para:
	g++ cache_para.cc -o para.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
optimal_sec:
	g++ cache_optimal_sec.cc -o optimal_sec.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
optimal_LW:
	g++ cache_optimal_LW.cc -o optimal_LW.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
optimal_rand:
	g++ cache_optimal_rand.cc -o optimal_rand.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
encoding:
	g++ cache_encoding.cc -o encoding.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
encoding_pusedo:
	g++ cache_encoding_PLRU.cc -o encoding_PLRU.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
encoding_NLRU:
	g++ cache_encoding_NLRU.cc -o encoding_NLRU.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)
encoding_duel:
	g++ cache_encoding_duel.cc -o encoding_duel.out $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS)

clean:
	@rm -f $(OBJECTS) $(OBJECTS:%.o=%.d)


#	g++ cache_main.cc -o trace.out $(CPPFLAGS) $(CXXFLAGS)
