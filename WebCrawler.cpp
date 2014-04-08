#include <boost/regex.hpp>
#include <fstream>
#include <ctime>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "common.h"
#include "WebCrawler.h"

void WebCrawler::setDownloadInterval(unsigned ms) {
    downloadInterval_ = ms;
}

void WebCrawler::setPagesDir(const char* pagesDir) {
    strcpy(pagesDir_, pagesDir);
}

void WebCrawler::downloadPage(const std::string& url, std::string& oPage) {
        // That's all that is needed to do cleanup of used resources (RAII style).
        std::ostringstream os;
        TMK_LOG_ALL("Downloading url %zu: %s\n", pagesDownloadedCounter_, url.c_str());
        
        unsigned delay = difftime(time(NULL), lastDownloadTime_) * 1000;
        if (delay < downloadInterval_ ) {
            TMK_LOG_ALL("sleeping %d ms\n", downloadInterval_ - delay);
            usleep((downloadInterval_ - delay)*1000);
        }
       
        time(&lastDownloadTime_);
        pagesDownloadedCounter_++;
        os << curlpp::options::Url(url) << std::endl;
        
        oPage = os.str();
        
        char fileName[256];
        sprintf(fileName, "%s/%zu.html", pagesDir_, pagesDownloadedCounter_);
        FILE *pFile = fopen(fileName, "w");
        if (!pFile) {
            TMK_LOG_ALL("Error: failed to open file for writing %s\n", fileName);
            return;
        }
        fprintf (pFile, "%s", oPage.c_str());
        fclose (pFile);
}


WebCrawler::WebCrawler(std::size_t numberOfUrls)
    : ParserSax()
    , globalUrlPreffix_("")
    , urlRegex_("(.*simple\\.wikipedia\\.org)?(/wiki/.*?)(#.*)?")
    , urls_(numberOfUrls)
    , currentDistanceFromMain_(0)
    , currentUrl_("")
    , pagesDownloadedCounter_(0)
    , lastDownloadTime_(0)
    , downloadInterval_(0)
    , minutesTimeStamp_(0)
    , downloadErrorsCount_(0)
    {
        curlpp::initialize(CURL_GLOBAL_ALL);    
        setPagesDir("data/pages");
    }

WebCrawler::~WebCrawler() {
    cURLpp::terminate();   
}

void WebCrawler::clearAll() {
    globalUrlPreffix_ = "";
    currentUrl_ = "";
    currentDistanceFromMain_ = 0;
    pagesDownloadedCounter_ = 0;
    urls_.clear();
    urlQueue_ = std::queue<std::pair<std::string, size_t> >();
    lastDownloadTime_ = 0;
    minutesTimeStamp_ = 0;
    downloadErrorsCount_ = 0;
}

bool WebCrawler::startCrawl(const std::string& mainPageUrl) {
    TMK_LOG_ALL("Starting crawling...\n");    
    clearAll();
    time_t t = time(NULL);
    struct tm *tmp = gmtime(&t);
    minutesTimeStamp_ = tmp->tm_min;
    boost::smatch result;
    if (!boost::regex_match(mainPageUrl, result, 
        boost::regex("(.*simple\\.wikipedia\\.org)(/wiki/.*?)(#.*)?"))) {
        std::cout << "Incorrect start page url :(\nI have ability to parse only articles of http://simple.wikipedia.org/\n";
        return false;
    }
    globalUrlPreffix_ = result[1].str();
    std::string startUrl  = result[1].str() + result[2].str();
    urls_.insert({startUrl, {urls_.size(), currentDistanceFromMain_, 0}});
    urlQueue_.push({startUrl, 0});
    
    std::string url;
    std::string page;
    while (!urlQueue_.empty() /*&& pagesDownloadedCounter_ < 30*/) {
        currentUrl_ = urlQueue_.front().first;
        currentDistanceFromMain_ = urlQueue_.front().second;
        urlQueue_.pop();

        try {
            downloadPage(currentUrl_, page);
            urls_[currentUrl_].sizeInBytes_ = strlen(page.c_str());
            parse(page);
        }
        catch (curlpp::RuntimeError &e) {
            TMK_LOG_ALL("RuntimeError: %s\n", e.what());
            downloadErrorsCount_++;
        }
        catch (curlpp::LogicError &e) {
            TMK_LOG_ALL("LogicError: %s\n", e.what());
            downloadErrorsCount_++;
        }
        
        if (pagesDownloadedCounter_ % 10000 == 0) {
            // If something goes wrong we will have some piece of data.
            char filename[256];
            sprintf(filename, "data/pagesData_%zu", pagesDownloadedCounter_ / 10000);
            saveToDisk(filename);
        }
        
    }
    TMK_LOG_ALL("Crawling successfully finished!\n"); 
    TMK_LOG_ALL("Pages downloaded: %zu\n", pagesDownloadedCounter_); 
    TMK_LOG_ALL("Download errors occured: %zu\n", downloadErrorsCount_);
    return true;
}

/*
 * returns false if the url is not an article, true otherwise
 */
bool WebCrawler::processUrl(const std::string& url, std::string& outUrl) {
    boost::smatch result;
    if (boost::regex_match(url, result, urlRegex_)) {   
        if (result[2].str().find(":") != std::string::npos) {
            return false;
        }
        outUrl = globalUrlPreffix_ +  result[2].str();
        return true;
    } else {
        return false;
    }
}

void WebCrawler::foundTag(htmlcxx::HTML::Node node, bool isEnd) {

    if (node.tagName() == "a" || node.tagName() == "A") {
        node.parseAttributes();
        std::map<std::string, std::string>::const_iterator it = node.attributes().find("href");
        if (it != node.attributes().end()) {
            const std::string &href = it->second;
            std::string processedHref;
            if (!processUrl(href, processedHref)) {
                TMK_FLOG("Url %s skipped\n", href.c_str());
                return;
            }
            
            auto pair = urls_.insert({processedHref, {urls_.size(), currentDistanceFromMain_, 0}});
            if (pair.second) {// means that urs is unique
                TMK_FLOG("Url %s added\n", processedHref.c_str());
                urlQueue_.push({processedHref, currentDistanceFromMain_ + 1});
                
            }
            urls_[currentUrl_].outgoingLinks_.push_back(pair.first->second.id_);   
        }
    }
}

void WebCrawler::saveToDisk(const char* filePath) {
    TMK_LOG_ALL("Saving page data to file %s\n", filePath); 
    FILE* urlsMapFile = fopen(filePath, "w");
    if (!urlsMapFile) {
        TMK_LOG_ALL("Error: failed to open file for writing %s\n", filePath);
        return;
    }
    
    fprintf(urlsMapFile, "%zu\n", urls_.size());   
    for (auto it = urls_.begin(); it!=urls_.end(); ++it)
    {
        fprintf(urlsMapFile, "%s \t %zu \t %zu \t %zu \t", 
                it->first.c_str(), 
                it->second.id_,
                it->second.distanceFromMain_,
                it->second.sizeInBytes_
               );
      
        for (auto linkIt = it->second.outgoingLinks_.begin(); linkIt != it->second.outgoingLinks_.end(); ++linkIt) 
        {
             fprintf(urlsMapFile, "%zu \t",  *linkIt);
        }
        fprintf(urlsMapFile, "\n");
    }
    fclose(urlsMapFile);
    TMK_LOG_ALL("Page data have been successfully saved!\n"); 
}
