DEPS = 
CXXFLAGS=-std=c++11 -I$(CURLPP_INCLUDE_DIR)
HTMLCXX_SOURCES_ROOT=htmlcxx/html

OBJ=main.o ParserDom.o ParserSax.o Node.o

CURLPP_DIR=curlpp
CURLPP_INCLUDE_DIR=$(CURLPP_DIR)/include
CURLPP_LIB_ROOT=$(CURLPP_DIR)/src/curlpp/

LD=g++ -g -W -Wall -Werror
#-I$(CURL_LIB_INCLUDE)

#its imoportant that you link the libcurlpp.a!
# LD_FLAGS=-L$(CURLPP_LIB_ROOT) $(CURLPP_LIB_ROOT)/libcurlpp.a -lstdc++ -lcurl 
#-pthread

LD_FLAGS=-L$(CURLPP_LIB_ROOT) -lcurlpp -static -lstdc++ -lcurl -lssl -lcrypto

EXECUTABLE=webCrawler.out

SHA2_DIR = sha256

all: $(EXECUTABLE)
	
#$(EXECUTABLE): curlpp $(OBJ) $(SHA2_DIR)/sha256.o;  $(LD) -o $@ $^  $(LD_FLAGS)
$(EXECUTABLE): $(OBJ) $(SHA2_DIR)/sha256.o
	$(CURLPP_DIR)/libtool --silent --tag=CXX   --mode=link g++  -g  -W -Wall  -static  -o $@ $^  $(LD_FLAGS)

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

# .PHONY: curlpp
curlpp:
	cd $(CURLPP_DIR) && ./configure --enable-ewarning=no
	$(MAKE) -C $(CURLPP_DIR)

clean: 
	$(MAKE) clean -C $(SHA2_DIR)	
	rm -f *.o
	rm -f *.out
