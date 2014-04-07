#ifndef TMK_PAGES_STATIST_H
#define TMK_PAGES_STATIST_H

#include <utility>
#include <vector>
#include <string>

#include "WebCrawler.h"

class PagesStatist {
public:
	typedef double PR;

	void PagesStatict(const std::string& pagesDataFilePath);

	void CalculatePageRank();

	// Get n top pages by PR
	void GetTopPages(size_t n, std::vector<Url>& oTopPages);

	/*
	 * Returns Unordered vector with sizes of the pages in bytes
	 */	
	void GetPageSizesInBytes(std::vector<size_t>& oSizes);

	/*
	 * Returns Unordered vector with count of outgoing link for pages
	 */	
	void GetPagesOutgoingLinksCount(std::vector<size_t>& oLinksCount)

protected:
	std::vector<Url> urls_;
	std::vector<PageDetails> pageDetails_;
	std::vector<PR> pageRank_;
	std::std::vector<size_t> pagesOutgoingLinksCount_;
	std::vector<std::pair<PageDetails::PageId, PR> > idsSortedbByPr_;
};

#endif // TMK_PAGES_STATIST_H