#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "WebCrawler.h"


// using namespace curlpp::options;
using namespace htmlcxx;

void parse1()
{
  std::string html = "<html><body>hey<a HREF='http://www.tut.by' >dsad</a> test <A href='http://example.com' > </body></html>";
  WebCrawler crawler(1000);
  crawler.parse(html);
}

typedef std::unordered_set<std::string> UrlHashTable;

int main(int argc, char** argv) {
  
  curlpp::initialize(CURL_GLOBAL_ALL);
  
    try
    {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;
//         curlpp::Easy::verbose(true);
         std::ostringstream os;
         os << "Page:\n" << curlpp::options::Url("http://www.wikipedia.org/") << std::endl;

         std::string asAskedInQuestion = os.str();
        std::cout << "From STR:\n" << asAskedInQuestion.substr(0,20) << std::endl;
    }

    catch(curlpp::RuntimeError & e)
    {
    std::cout << "Error: " << e.what() << std::endl;
    }

    catch(curlpp::LogicError & e)
    {
    std::cout << " Logic Error: " << e.what() << std::endl;
    }

  //Parse some html code
 parse1();
  
  return 0;
}
