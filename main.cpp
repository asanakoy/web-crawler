#include <iostream>
#include <sstream>
#include <algorithm>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "htmlcxx/html/ParserDom.h"
#include "sha256/sha256.h"

// using namespace curlpp::options;
using namespace htmlcxx;

void parse1()
{
  std::string html = "<html><body>hey<a href='http://www.tshit!' >dsad</a> </body></html>";
  HTML::ParserDom parser;
  tree<HTML::Node> dom = parser.parseTree(html);
  
  //Print whole DOM tree
  std::cout << dom << std::endl;
  
  //Dump all links in the tree
  tree<HTML::Node>::iterator it = dom.begin();
  tree<HTML::Node>::iterator end = dom.end();
  for (; it != end; ++it)
  {
    std::string tagName = it->tagName();
    std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
    if (tagName == "a")
    {
        it->parseAttributes();
        std::cout << it->attribute("href").second << std::endl;
    }
  }
  
  //Dump all text of the document
  it = dom.begin();
  end = dom.end();
  for (; it != end; ++it)
  {
    if ((!it->isTag()) && (!it->isComment()))
    {
      std::cout << it->text();
    }
   }
}

int main(int argc, char** argv) {
  std::cout << "main()" << std::endl;
  std::string url = "http://www.wikipedia.org/";
  std::string output = sha256(url);

  std::cout << "sha256('"<< url << "'):" << output << std::endl;
  return 0;

  curlpp::initialize(CURL_GLOBAL_ALL);
  
    try
    {

        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;
//         curlpp::Easy::verbose(true);

    

         std::ostringstream os;
         os << "Page:\n" << curlpp::options::Url("http://www.wikipedia.org/") << std::endl;

         std::string asAskedInQuestion = os.str();
        std::cout << "From STR:\n" << asAskedInQuestion << std::endl;
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
