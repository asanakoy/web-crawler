#ifndef TMK_PAGES_STATIST_H
#define TMK_PAGES_STATIST_H

#include <utility>
#include <vector>
#include <string>

#include "WebCrawler.h"

class PagesStatist {
public:
	typedef double PR;

	PagesStatist(const std::string& pagesDataFilePath);

	void calculatePageRank();

	// Get n top pages by PR
	void getTopPages(size_t n, std::vector<Url>& oTopPages);

	/*
	 * Returns Unordered vector with sizes of the pages in bytes
	 */	
	void getPageSizesInBytes(std::vector<size_t>& oSizes);

	/*
	 * Returns Unordered vector with count of outgoing link for pages
	 */	
	const std::vector<size_t>& getPagesOutgoingLinksCount();

protected:
	std::vector<Url> urls_;
	std::vector<PageDetails> pageDetails_;
	std::vector<PR> pageRank_;
	std::vector<size_t> pagesOutgoingLinksCount_;
	std::vector<std::pair<PageDetails::PageId, PR> > idsSortedbByPr_;
};

#endif // TMK_PAGES_STATIST_H