DEPS = 
CXXFLAGS=-std=c++11 -I$(CURLPP_INCLUDE_DIR)
HTMLCXX_SOURCES_ROOT=htmlcxx/html

OBJ=main.o ParserSax.o Node.o WebCrawler.o common.o PagesStatist.o

CURLPP_DIR=curlpp
CURLPP_INCLUDE_DIR=$(CURLPP_DIR)/include
CURLPP_LIB_ROOT=$(CURLPP_DIR)/src/curlpp/

LD_FLAGS_COMMON = -L$(CURLPP_LIB_ROOT) -lcurlpp -static -lstdc++ -lcurl -lboost_system -lboost_regex 
LD_FLAGS_MAC = -lssl -lcrypto
LD_FLAGS_LINUX = -pthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LD_FLAGS = $(LD_FLAGS_COMMON) $(LD_FLAGS_LINUX) 
	LIBTOOL = libtool
endif
ifeq ($(UNAME_S),Darwin)
	LD_FLAGS = $(LD_FLAGS_COMMON) $(LD_FLAGS_MAC)
	LIBTOOL = $(CURLPP_DIR)/libtool
endif

EXECUTABLE=webCrawler.out

# SHA2_DIR = sha256

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJ)
	$(LIBTOOL) --silent --tag=CXX   --mode=link g++  -g  -W -Wall  -static  -o $@ $^  $(LD_FLAGS)

# .PHONY: $(SHA2_DIR)/sha256.o
# $(SHA2_DIR)/sha256.o:
# 	$(MAKE) -C $(SHA2_DIR)

ParserDom.o: $(HTMLCXX_SOURCES_ROOT)/ParserDom.cc
	g++ $(CXXFLAGS) -c $< -o $@
ParserSax.o: $(HTMLCXX_SOURCES_ROOT)/ParserSax.cc
	g++ $(CXXFLAGS) -c $< -o $@
Node.o: $(HTMLCXX_SOURCES_ROOT)/Node.cc
	g++ $(CXXFLAGS) -c $< -o $@
	
WebCrawler.o: WebCrawler.cpp
	g++ $(CXXFLAGS) -c $< -o $@ $(LD_FLAGS)
	
%.o: %.cpp  ;  g++ $(CXXFLAGS) -c $< -o $@

.PHONY: curlpp
curlpp:
	cd $(CURLPP_DIR) && ./configure --enable-ewarning=no
	$(MAKE) -C $(CURLPP_DIR)

clean: 
# 	$(MAKE) clean -C $(SHA2_DIR)	
	rm -f *.o
	rm -f *.out
