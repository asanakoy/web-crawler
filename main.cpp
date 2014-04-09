#include <iostream>
#include <sstream>
#include <algorithm>

#include "common.h"
#include "WebCrawler.h"
#include "PagesStatist.h"

class Logger;

void printUsage(char* scriptName) {
    std::cout << "Usage: \n" 
    << "Start crawling:\n\t"
    << scriptName << "  --crawl  start_url" << std::endl
    << "Resume crawling. Init state from file:\n\t"
    << scriptName << "  --resume-crawl  pages_data_file" << std::endl
    << "Calculate PR and other stat:\n\t"
    << scriptName << "  --stat   pages_data_file" << std::endl;
}

int main(int argc, char **argv) {
    
    if (argc != 3) {
        printUsage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "--crawl") == 0) {
        Logger::setOutputFile("log-crawl_test.txt");

        WebCrawler crawler(120000);
        crawler.setDownloadInterval(5);
        crawler.setPagesDir("data/pages_test");
        if (!crawler.startCrawl(argv[2]))
            return 0;
        crawler.saveToDisk("data/pagesData_total.txt");

    } else  if (strcmp(argv[1], "--resume-crawl") == 0) {
        Logger::setOutputFile("log-crawl_test.txt");

        WebCrawler crawler(120000);
        crawler.setDownloadInterval(5);
        crawler.setPagesDir("pages");
        if (!crawler.resumeCrawl(argv[2]))
            return 0;
        crawler.saveToDisk("data/pagesData_total.txt");

    } else if (strcmp(argv[1], "--stat") == 0) {
        Logger::setOutputFile("log-stat_test.txt");
        PagesStatist statist(argv[2]);

        const std::vector<PR>& pageRank = statist.getPageRank();
        TMK_LOG_ALL("Saving PR to file \n");
        tmk::saveToDisk(pageRank, "data/PR.txt");
        
        std::vector<std::pair<Url, PR> > top20;
        statist.getTopPages(20, top20);
        TMK_LOG_ALL("Top 20 pages by PR:\n");
        for (int i = 0; i < top20.size(); ++i) {
           TMK_LOG_ALL("PR: %f\t%s\n", top20[i].second, top20[i].first.c_str()); 
        }
        
        const std::vector<size_t>& pagesSize = statist.getPageSizesInBytes();
        TMK_LOG_ALL("Saving pages Sizes to file \n");
        tmk::saveToDisk(pagesSize, "data/pageSize.txt");
        
        const std::vector<size_t>& pagesOutgoingLinksCount = statist.getPagesOutgoingLinksCount();
        TMK_LOG_ALL("Saving pages outgoing links count to file\n");
        tmk::saveToDisk(pagesOutgoingLinksCount, "data/pageOutLinks.txt");
        
        const std::vector<size_t>& pagesIncomingLinksCount = statist.getPagesIncomingLinksCount();
        TMK_LOG_ALL("Saving pages incoming links count to file\n");
        tmk::saveToDisk(pagesIncomingLinksCount, "data/pageInLinks.txt");
        
        const std::vector<size_t>& pageDistancesFromMain = statist.getPageDistancesFromMain();
        TMK_LOG_ALL("Saving pages distances from main page to file\n");
        tmk::saveToDisk(pageDistancesFromMain, "data/pageDistances.txt");
        size_t maxPageDist = 0;
        for (auto it = pageDistancesFromMain.begin(); it != pageDistancesFromMain.end(); ++it) {
            maxPageDist = std::max(maxPageDist, *it);
        }
        TMK_LOG_ALL("Maximal distance from main page: %zu\n", maxPageDist);
        
    } else {
        printUsage(argv[0]);
    }
    
    return 0;
}
