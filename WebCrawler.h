#include "htmlcxx/html/ParserDom.h"

#include <iostream>
#include <unordered_set>
#include <queue>

class WebCrawler: public htmlcxx::HTML::ParserSax
{
public:
    WebCrawler(std::size_t numberOfUrls) 
    : ParserSax()
    , urls_(numberOfUrls) {}
    
    virtual ~WebCrawler() {}

    typedef std::unordered_set<std::string> UrlHashTable;

protected:
    virtual void foundTag(htmlcxx::HTML::Node node, bool isEnd);
        
    UrlHashTable urls_;    
    std::queue<std::string> urlQueue_;
};
