#include "PagesStatist.h"
#include "common.h"

#include <sstream>
#include <fstream>
#include <algorithm>
#include <boost/bind.hpp>
#include <set>

PagesStatist::PagesStatist(const char* pagesDataFilePath) {
    TMK_LOG_ALL("Initializing PagesStatist\n");
	TMK_LOG_ALL("Reading data from %s\n", pagesDataFilePath);
    std::ifstream fin(pagesDataFilePath);
    if (!fin) {
        TMK_LOG_ALL("Error opening '%s' for reading\n", pagesDataFilePath);
        return;
    }
	size_t n;
	fin >> n;
    fin.get();
	pageDetails_.assign(n, {0, 0, 0});
    urls_.assign(n, "");

    Url url;
    PageDetails::PageId id;
    
    std::string line;
    
    while (std::getline(fin, line)) {
        std::istringstream sin(line);
        sin >> url >> id;
        pageDetails_[id].id_ = id;
        
        sin >> pageDetails_[id].distanceFromMain_ >> pageDetails_[id].sizeInBytes_;
        urls_[id] = url;
        PageDetails::Links& links = pageDetails_[id].outgoingLinks_;
        PageDetails::PageId linkedPageId;
        while (sin >> linkedPageId) {
            assert(linkedPageId < n && "ooops");
            if (linkedPageId != id)
                links.insert(linkedPageId);
        }
    }
	
	assert(n == pageDetails_.size() && "oops! The input data is malformed!");
    assert(n == urls_.size() && "oops! The input data is malformed!");
    
    TMK_LOG_ALL("Data have been read!\n");
}

void PagesStatist::calculatePageRank() const {
	TMK_LOG("Calculating page rank\n");
	pageRank_.assign(pageDetails_.size(), 1);
	std::vector<PR> tmpPageRank(pageRank_);
	std::vector<bool> flag(pageDetails_.size(), false);
	getPagesOutgoingLinksCount();
	bool valueVisited = true;
	std::queue<PageDetails::PageId> q;
	q.push(0);
    flag[0] = valueVisited;
    const double EPS = 0.000001;
    double delta = 100000.0;
    size_t iteration = 0;
    while (delta > EPS) {
        while (!q.empty()) {
            PageDetails::PageId id = q.front();
            q.pop();
            
            
            if (pagesOutgoingLinksCount_[id] != 0) {
                for (auto it = pageDetails_[id].outgoingLinks_.begin(); it != pageDetails_[id].outgoingLinks_.end(); ++it) 
                {
                    pageRank_[*it] += tmpPageRank[id] / pagesOutgoingLinksCount_[id];
                    if (flag[*it] != valueVisited) {
                        q.push(*it);
                        flag[*it] = valueVisited;
                    }
                }
            }
        }

        delta = 0;
        for (int i = 0; i < pageRank_.size(); ++i) {
            delta = std::max(delta, pageRank_[i] - tmpPageRank[i]);
           
        }
        
        tmpPageRank = pageRank_;
        valueVisited = !valueVisited;
        iteration++;
    }
    TMK_LOG_ALL("PR have been calculated in %d iterations\n", iteration);
    TMK_LOG_ALL("Sorting pages by PR\n");
    idsSortedbByPr_.clear();
    idsSortedbByPr_.reserve(pageRank_.size());
    
    for(size_t i = 0; i < pageRank_.size(); ++i) {
        idsSortedbByPr_.push_back({i, pageRank_[i]});
    }
    std::sort(idsSortedbByPr_.begin(), idsSortedbByPr_.end(), boost::bind(&PagesStatist::greater, this, _1, _2));
}

void PagesStatist::getTopPages(size_t n, std::vector<std::pair<Url, PR> >& oTopPages) const {
    if (pageRank_.size() == 0)
        calculatePageRank();    
    
	oTopPages.clear();
	for (size_t i = 0; i < n; ++i) {
		oTopPages.push_back({urls_[idsSortedbByPr_[i].first], idsSortedbByPr_[i].second});
	}
}

const std::vector<PR>& PagesStatist::getPageRank() const {
    if (pageRank_.size() == 0)
        calculatePageRank();
    return pageRank_;
}

const std::vector<size_t>& PagesStatist::getPageSizesInBytes() const {
	if (pagesSizes_.size() == 0) {
        pagesSizes_.assign(pageDetails_.size(), 0);
        for (auto it = pageDetails_.begin(); it != pageDetails_.end(); ++it) {
            pagesSizes_[it->id_] = it->sizeInBytes_;
        }
    }
	return pagesSizes_;
}

const std::vector<size_t>& PagesStatist::getPagesOutgoingLinksCount() const {
    if (pagesOutgoingLinksCount_.size() == 0) {
        pagesOutgoingLinksCount_.assign(pageDetails_.size(), 0);
        for (auto it = pageDetails_.begin(); it != pageDetails_.end(); ++it) {
            pagesOutgoingLinksCount_[it->id_] = it->outgoingLinks_.size();
        }
    }
    return pagesOutgoingLinksCount_;
}

const std::vector<size_t>& PagesStatist::getPagesIncomingLinksCount() const {
    if (pagesIncomingLinksCount_.size() == 0) {
        pagesIncomingLinksCount_.assign(pageDetails_.size(), 0);
        for (auto it = pageDetails_.begin(); it != pageDetails_.end(); ++it) {
            for (auto linkIt = it->outgoingLinks_.begin(); linkIt != it->outgoingLinks_.end(); ++linkIt) {
                pagesIncomingLinksCount_[*linkIt]++;
            }
        }
    }
    return pagesIncomingLinksCount_;
}

const std::vector<size_t>& PagesStatist::getPageDistancesFromMain() const {
    if (pageDistancesFromMain_.size() == 0) {
        pageDistancesFromMain_.assign(pageDetails_.size(), 0);
        for (auto it = pageDetails_.begin(); it != pageDetails_.end(); ++it) {
            pageDistancesFromMain_[it->id_] = it->distanceFromMain_;
        }
    }
    return pageDistancesFromMain_;
}

const std::string& PagesStatist::GetUrlById(PageDetails::PageId id) const {
    return urls_[id];
}

bool PagesStatist::greater(const std::pair<PageDetails::PageId, PR>& a,
                const std::pair<PageDetails::PageId, PR>& b) const
{
    return a.second > b.second;
}
