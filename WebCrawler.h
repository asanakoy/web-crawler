#ifndef TMK_WEB_CRAWLER_H
#define TMK_WEB_CRAWLER_H

#include <boost/regex.hpp>
#include <iostream>
#include <unordered_map>
#include <set>
#include <queue>
#include <ctime>

#include "htmlcxx/html/ParserDom.h"

typedef std::string Url;

struct PageDetails {
    typedef size_t PageId;
    typedef size_t ClickDistance;
    typedef std::set<size_t> Links;

    PageDetails(size_t id = 0, size_t distanceFromMain = 0, size_t sizeInBytes = 0) 
    : id_(id)
    , distanceFromMain_(distanceFromMain)
    , sizeInBytes_(sizeInBytes) {}
    
    PageId id_;
    ClickDistance distanceFromMain_;
    size_t sizeInBytes_;
    Links outgoingLinks_;
};

class WebCrawler: public htmlcxx::HTML::ParserSax {
public:
    WebCrawler(std::size_t numberOfUrls);

    virtual ~WebCrawler();
    
    void setDownloadInterval(unsigned ms);
    
    // Set dir where to save downloaded pages
    void setPagesDir(const char* pagesDir);
    
    bool startCrawl(const std::string& mainPageUrl);
    
    /*
     * Resume crawl.
     * Load state from file. File format should be like in saveToDist method.
     * All pages with size 0 wiil be scheduled to download.
     */
    bool resumeCrawl(const char* pagesDataFilePath);
   
    /*
     * Saves data about pages in the following format:
     * Url id distance_from_main_page size_in_bytes outgoing_links_ids
     * where outgoing_links_ids is the list of ids of the pages where we can go by the link from the current page
     * elements are separated by the space or tabulation
     */
    void saveToDisk(const char* filename);
    
    typedef std::unordered_map<Url, PageDetails> UrlsHashTable;

protected:
    void clearAll();
    // throws curlpp::RuntimeError and curlpp::LogicError
    void downloadPage(const std::string& url, PageDetails::PageId id, std::string& oPage);
    
    void crawl();
    
    void initFromFile(const char* pagesDataFilePath);
    
    virtual void foundTag(htmlcxx::HTML::Node node, bool isEnd);
    
    bool processUrl(const std::string& url, std::string& outUrl);
    
    static bool less(const std::pair<Url, PageDetails::ClickDistance>& a,
                     const std::pair<Url, PageDetails::ClickDistance>& b);
                 
    std::string globalUrlPreffix_;
    boost::regex urlRegex_;
    UrlsHashTable urlsData_;
    size_t currentDistanceFromMain_; // current Distance From the Main page in clicks
    Url currentUrl_;
    std::queue<std::pair<Url, PageDetails::ClickDistance> > urlQueue_; // pairs (url, distance_from_main_page)
    size_t pagesDownloadedCounter_;
    std::time_t lastDownloadTime_;
    unsigned downloadInterval_;
    int minutesTimeStamp_;
    size_t downloadErrorsCount_;
    char pagesDir_[256];
};

#endif // TMK_WEB_CRAWLER_H
