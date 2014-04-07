#include <iostream>
#include <sstream>
#include <algorithm>

#include "common.h"
#include "WebCrawler.h"

class Logger;

// using namespace curlpp::options;
using namespace htmlcxx;

void parse1() {
    std::string html = "<html><body>hey<a HREF='http://www.tut.by' >dsad</a> test <A href='http://example.com' > </body></html>";
    WebCrawler crawler(1000);
    crawler.parse(html);
}

void printUsage(char* scriptName) {
    std::cout << "Usage: " << scriptName << " start_url" << std::endl;
}

int main(int argc, char **argv) {
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 0;
    }

    WebCrawler crawler(100000);
    crawler.setDownloadInterval(0);
    if (!crawler.startCrawl(argv[1]))
        return 0;
    crawler.saveToDisk();
    
    //Parse some html code
    //parse1();

    return 0;
}
