DEPS = 
CXXFLAGS=-std=c++11 -I$(CURLPP_INCLUDE_DIR)
HTMLCXX_SOURCES_ROOT=htmlcxx/html

OBJ=main.o ParserDom.o ParserSax.o Node.o

CURLPP_DIR=curlpp
CURLPP_INCLUDE_DIR=$(CURLPP_DIR)/include
CURLPP_LIB_ROOT=$(CURLPP_DIR)/src/curlpp/


LD_FLAGS_MAC=-L$(CURLPP_LIB_ROOT) -lcurlpp -static -lstdc++ -lcurl -lssl -lcrypto
LD_FLAGS_LINUX=-L$(CURLPP_LIB_ROOT) -lcurlpp -static -lstdc++ -lcurl -pthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LD_FLAGS = $(LD_FLAGS_LINUX)
	LIBTOOL = libtool
endif
ifeq ($(UNAME_S),Darwin)
	LD_FLAGS = $(LD_FLAGS_MAC)
	LIBTOOL = $(CURLPP_DIR)/libtool
endif

EXECUTABLE=webCrawler.out

SHA2_DIR = sha256

all: curlpp $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJ) $(SHA2_DIR)/sha256.o
	$(LIBTOOL) --silent --tag=CXX   --mode=link g++  -g  -W -Wall  -static  -o $@ $^  $(LD_FLAGS)

.PHONY: $(SHA2_DIR)/sha256.o
$(SHA2_DIR)/sha256.o:
	$(MAKE) -C $(SHA2_DIR)

ParserDom.o:
	g++ $(CXXFLAGS) -c $(HTMLCXX_SOURCES_ROOT)/ParserDom.cc -o $@
ParserSax.o:
	g++ $(CXXFLAGS) -c $(HTMLCXX_SOURCES_ROOT)/ParserSax.cc -o $@
Node.o:
	g++ $(CXXFLAGS) -c $(HTMLCXX_SOURCES_ROOT)/Node.cc -o $@
	
%.o: %.cc $(DEPS)    ;  g++ $(CXXFLAGS) -c $< -o $@

.PHONY: curlpp
curlpp:
	cd $(CURLPP_DIR) && ./configure --enable-ewarning=no
	$(MAKE) -C $(CURLPP_DIR)

clean: 
	$(MAKE) clean -C $(SHA2_DIR)	
	rm -f *.o
	rm -f *.out
