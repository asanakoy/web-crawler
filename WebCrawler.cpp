#include "WebCrawler.h"

void WebCrawler::foundTag(htmlcxx::HTML::Node node, bool isEnd) {
   if (node.tagName() == "a" || node.tagName() == "A") {
       node.parseAttributes();
       std::map<std::string, std::string>::const_iterator it = node.attributes().find("href");
       if (it != node.attributes().end()) {
           const std::string& href = it->second;
           if (urls_.insert(href).second) {// means that urs is unique
               urlQueue_.push(href);
               std::cout << "href= " << href << std::endl;
           }
       }
   }
}