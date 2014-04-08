#include <iostream>
#include <sstream>
#include <algorithm>

#include "common.h"
#include "WebCrawler.h"
#include "PagesStatist.h"

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
    
    if (argc > 2) {
        printUsage(argv[0]);
        return 0;
    }

    if (argc == 2) {
//         std::cout << "Do you want to start crawling from page" <<  argv[1] << " ? (Y / N): ";
//         std::string c;
//         std::cin >> c;
//         if (c != "Y" && c != "y") {
//             return 0;
//         } 
        WebCrawler crawler(100000);
        crawler.setDownloadInterval(0);
        if (!crawler.startCrawl(argv[1]))
            return 0;
        crawler.saveToDisk();

    } else {

        TMK_LOG("reading pagesData.txt\n");
        PagesStatist statist(std::string("pagesData.txt"));
        statist.calculatePageRank();

        std::vector<Url> top20;
        std::vector<size_t> pagesSize;
        

        statist.getTopPages(20, top20);
        statist.getPageSizesInBytes(pagesSize);
        const std::vector<size_t>& pagesOutgoingLinksCount = statist.getPagesOutgoingLinksCount();
    }
    
    return 0;
}
