DEPS = 
CXXFLAGS=
HTMLCXX_SOURCES_ROOT=htmlcxx/html

OBJ=main.o ParserDom.o ParserSax.o Node.o

CURLPP_LIB_ROOT=/usr/local/lib
CURL_LIB_INCLUDE=.

LD=g++ -g -W -Wall -Werror -I/usr/include -I$(CURL_LIB_INCLUDE)

#its imoportant that you link the libcurlpp.a!
LD_FLAGS=-L$(CURLPP_LIB_ROOT) $(CURLPP_LIB_ROOT)/libcurlpp.a -lstdc++ \
-L/usr/lib/x86_64-linux-gnu -lcurl -pthread

EXECUTABLE=webCrawler.out

SHA2_DIR = sha256

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJ) $(SHA2_DIR)/sha256.o;  $(LD) -o $@ $^  $(LD_FLAGS)

.PHONY: $(SHA2_DIR)/sha256.o
$(SHA2_DIR)/sha256.o:
	$(MAKE) -C $(SHA2_DIR)

ParserDom.o: $(DEPS)
	g++ $(CXXFLAGS) -c $(HTMLCXX_SOURCES_ROOT)/ParserDom.cc -o $@
ParserSax.o: $(DEPS)
	g++ $(CXXFLAGS) -c $(HTMLCXX_SOURCES_ROOT)/ParserSax.cc -o $@
Node.o: $(DEPS)
	g++ $(CXXFLAGS) -c $(HTMLCXX_SOURCES_ROOT)/Node.cc -o $@
	
%.o: %.cc $(DEPS)    ;  g++ $(CXXFLAGS) -c $< -o $@

clean: 
	$(MAKE) clean -C $(SHA2_DIR)	
	rm -f *.o
	rm -f *.out
