#ifndef TMK_PAGES_STATIST_H
#define TMK_PAGES_STATIST_H

#include <utility>
#include <vector>
#include <string>

#include "WebCrawler.h"

class PagesStatict {
public:
	typedef double PR;

	void PagesStatict(const std::string& pagesDataFilePath);
	void CalculatePageRank();
	void GetTopPages(std::vector<std::string>& oTopPages);

protected:
	std::vector<Url> urls_;
	std::vector<PageDetails> pageDetails;
	std::vector<std::pair<PageDetails::PageId, PR> > idsSortedbByPr;
};

#endif // TMK_PAGES_STATIST_H