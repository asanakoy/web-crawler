#ifndef TMK_WEB_CRAWLER_H
#define TMK_WEB_CRAWLER_H

#include <boost/regex.hpp>
#include <iostream>
#include <unordered_map>
#include <queue>

#include "htmlcxx/html/ParserDom.h"

typedef std::string Url;

struct PageDetails {
    typedef size_t PageId;
    typedef size_t ClickDistance;
    typedef std::vector<size_t> Links;

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
    
    bool startCrawl(const std::string& mainPageUrl);
    
    
    
    void saveToDisk();
    
    
    typedef std::unordered_map<Url, PageDetails> UrlsHashTable;

protected:
    void clearAll();
    // throws curlpp::RuntimeError and curlpp::LogicError
    void downloadPage(const std::string& url, std::string& oPage);
    
    virtual void foundTag(htmlcxx::HTML::Node node, bool isEnd);
    
    bool processUrl(const std::string& url, std::string& outUrl);
    
    std::string globalUrlPreffix_;
    boost::regex urlRegex_;
    UrlsHashTable urls_;
    size_t currentDistanceFromMain_; // current Distance From the Main page in clicks
    Url currentUrl_;
    std::queue<std::pair<Url, PageDetails::ClickDistance> > urlQueue_; // pairs (url, distance_from_main_page)
    size_t pagesDownloadedCounter_;
    std::time_t lastDownloadTime_;
    unsigned downloadInterval_;
};

#endif // TMK_WEB_CRAWLER_H