#ifndef TMK_PAGES_STATIST_H
#define TMK_PAGES_STATIST_H

#include <utility>
#include <vector>
#include <string>

#include "WebCrawler.h"

typedef double PR;
    
class PagesStatist {
public:

	PagesStatist(const char* pagesDataFilePath);

	// Get n top pages by PR
	void getTopPages(size_t n, std::vector<std::pair<Url, PR> >& oTopPages) const;
    
//     const std::vector<std::pair<PageDetails::PageId, PR> > get
    
    // Get PR
    const std::vector<PR>& getPageRank() const;
    
	/*
	 * Vector with sizes of the pages in bytes
	 */	
	const std::vector<size_t>& getPageSizesInBytes() const;
    
	/*
	 * Vector with count of outgoing link for pages
	 */	
	const std::vector<size_t>& getPagesOutgoingLinksCount() const;
    
    /*
     * Vector with count of incoming link for pages
     */ 
    const std::vector<size_t>& getPagesIncomingLinksCount() const;
    
    const std::string& GetUrlById(PageDetails::PageId id) const;
    
protected:
    void calculatePageRank() const;
    
    bool greater(const std::pair<PageDetails::PageId, PR>& a,
                 const std::pair<PageDetails::PageId, PR>& b) const;
        
	std::vector<Url> urls_;
	std::vector<PageDetails> pageDetails_;
	mutable std::vector<PR> pageRank_;
    mutable std::vector<size_t> pagesSizes_; //sizes in bytes
	mutable std::vector<size_t> pagesOutgoingLinksCount_;
    mutable std::vector<size_t> pagesIncomingLinksCount_;
	mutable std::vector<std::pair<PageDetails::PageId, PR> > idsSortedbByPr_;
};

#endif // TMK_PAGES_STATIST_H
