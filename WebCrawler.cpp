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

void WebCrawler::downloadPage(const std::string& url, PageDetails::PageId id, std::string& oPage) {
        // That's all that is needed to do cleanup of used resources (RAII style).
        std::ostringstream os;
        TMK_LOG_ALL("Downloading url %zu: %s\n", id, url.c_str());
        
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
        sprintf(fileName, "%s/%zu.html", pagesDir_, id);
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
    , urlsData_(numberOfUrls)
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
    urlsData_.clear();
    urlQueue_ = std::queue<std::pair<std::string, size_t> >();
    lastDownloadTime_ = 0;
    downloadErrorsCount_ = 0;
}

void WebCrawler::crawl() {
    std::string url;
    std::string page;
    while (!urlQueue_.empty() /*&& pagesDownloadedCounter_ < 30*/) {
        currentUrl_ = urlQueue_.front().first;
        currentDistanceFromMain_ = urlQueue_.front().second;
        urlQueue_.pop();

        try {
            downloadPage(currentUrl_, urlsData_[currentUrl_].id_, page);
            urlsData_[currentUrl_].sizeInBytes_ = strlen(page.c_str());
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
        
        if (pagesDownloadedCounter_ % 2000 == 0) {
            // If something goes wrong we will have some piece of data.
            char filename[256];
            sprintf(filename, "data/pagesData_%zu", pagesDownloadedCounter_ / 2000);
            saveToDisk(filename);
        }
    }
}

bool WebCrawler::startCrawl(const std::string& mainPageUrl) {
    TMK_LOG_ALL("Starting crawling...\n");    
    clearAll();
    boost::smatch result;
    if (!boost::regex_match(mainPageUrl, result, 
        boost::regex("(.*simple\\.wikipedia\\.org)(/wiki/.*?)(#.*)?"))) {
        std::cout << "Incorrect start page url :(\nI have ability to parse only articles of http://simple.wikipedia.org/\n";
        return false;
    }
    globalUrlPreffix_ = result[1].str();
    std::string startUrl  = result[1].str() + result[2].str();
    urlsData_.insert({startUrl, {urlsData_.size(), currentDistanceFromMain_, 0}});
    urlQueue_.push({startUrl, 0});
    
    crawl();
    
    TMK_LOG_ALL("Crawling successfully finished!\n"); 
    TMK_LOG_ALL("Pages downloaded: %zu\n", pagesDownloadedCounter_); 
    TMK_LOG_ALL("Download errors occured: %zu\n", downloadErrorsCount_);
    return true;
}

bool WebCrawler::resumeCrawl(const char* pagesDataFilePath) {
    TMK_LOG_ALL("Resuming crawling...\n"); 
    clearAll();
    initFromFile(pagesDataFilePath);
    
    crawl();
    
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
            
            auto pair = urlsData_.insert({processedHref, {urlsData_.size(), currentDistanceFromMain_, 0}});
            if (pair.second) {// means that urs is unique
                TMK_FLOG("Url %s added\n", processedHref.c_str());
                urlQueue_.push({processedHref, currentDistanceFromMain_ + 1});
                
            }
            if (urlsData_[currentUrl_].id_ != pair.first->second.id_)
                urlsData_[currentUrl_].outgoingLinks_.insert(pair.first->second.id_);   
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
    
    fprintf(urlsMapFile, "%zu\n", urlsData_.size());   
    for (auto it = urlsData_.begin(); it!=urlsData_.end(); ++it)
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

void WebCrawler::initFromFile(const char* pagesDataFilePath) {
    TMK_LOG_ALL("Initializing crawler from file...\n");
    TMK_LOG_ALL("Reading data from %s\n", pagesDataFilePath);
    std::ifstream fin(pagesDataFilePath);
    if (!fin) {
        TMK_LOG_ALL("Error opening '%s' for reading\n", pagesDataFilePath);
        return;
    }
    size_t n;
    fin >> n;
    fin.get();
    urlsData_.reserve(n);

    Url url;
    // Items for UrlQueue
    std::vector<std::pair<Url, PageDetails::ClickDistance> > items;
    
    PageDetails::PageId id;
    
    std::string line;
    
    while (std::getline(fin, line)) {
        std::istringstream sin(line);
        sin >> url;
        
        PageDetails details;
        sin >> details.id_ >> details.distanceFromMain_ >> details.sizeInBytes_;
        
        if (details.sizeInBytes_ == 0) {
            items.push_back({url, details.distanceFromMain_});
        }        
        PageDetails::Links& links =  urlsData_.insert({url, details}).first->second.outgoingLinks_;
        PageDetails::PageId linkedPageId;
        while (sin >> linkedPageId) {
            assert(linkedPageId < n && "ooops");
        }
    }
    
    assert(n == urlsData_.size() && "oops! The input data is malformed!");
    
    boost::smatch result;
    if (!boost::regex_match(urlsData_.begin()->first, result, 
        boost::regex("(.*simple\\.wikipedia\\.org)(/wiki/.*?)"))) {
        TMK_LOG_ALL("Incorrect page url %s\nI have ability to parse only articles of http://simple.wikipedia.org/\n",
                    urlsData_.begin()->first.c_str());
        TMK_LOG_ALL("The input data is malformed!\n");
        return;
    }
    globalUrlPreffix_ = result[1].str();
    
    std::sort(items.begin(), items.end(), WebCrawler::less);
    for (auto it = items.begin(); it != items.end(); ++it) {
        urlQueue_.push(*it);
    }
    pagesDownloadedCounter_ = urlsData_.size() - items.size();
    
    TMK_LOG_ALL("Data have been read!\n");
}

bool WebCrawler::less(const std::pair<Url, PageDetails::ClickDistance>& a,
                             const std::pair<Url, PageDetails::ClickDistance>& b)
{
    return a.second < b.second;
}
