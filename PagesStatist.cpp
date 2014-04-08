#include "PagesStatist.h"
#include "common.h"

#include <sstream>
#include <fstream>

PagesStatist::PagesStatist(const std::string& pagesDataFilePath) {
	TMK_LOG_ALL("Reading data from %s\n", pagesDataFilePath.c_str());
    std::ifstream fin(pagesDataFilePath.c_str());
    if (!fin) {
        TMK_LOG_ALL("Error opening '%s'\n", pagesDataFilePath.c_str());
        return;
    }
	size_t n;
	fin >> n;
	pageDetails_.reserve(n);
	for (size_t i = 0; i < n; ++i) {
		PageDetails::PageId id;
		PageDetails::ClickDistance distanceFromMain;
    	size_t sizeInBytes;
        fin >> id >> distanceFromMain >> sizeInBytes;
        std::vector<PageDetails::PageId> links;
        PageDetails::PageId linkedPageId;
        pageDetails_.push_back({id, distanceFromMain, sizeInBytes});
        while (!fin.eof()) {
            std::string line;
            std::getline(fin, line);
            std::istringstream sin(line);
            while (!sin.eof()) {
                sin >> linkedPageId;
                pageDetails_.back().outgoingLinks_.push_back(linkedPageId);
            }
         }
	}
	TMK_LOG_ALL("Data have been read!\n");
}

void PagesStatist::calculatePageRank() {
	TMK_LOG("Calculating page rank\n");
	pageRank_.assign(pageDetails_.size(), 10);
	std::vector<PR> tmpPageRank(pageRank_);
	std::vector<bool> flag(pageDetails_.size(), false);
	getPagesOutgoingLinksCount();
	bool valueVisited = true;
	std::queue<PageDetails::PageId> q;
	q.push(0);
    flag[0] = valueVisited;
	while (!q.empty()) {
		PageDetails::PageId id = q.front();
		q.pop();

		for (auto it = pageDetails_[id].outgoingLinks_.begin(); it != pageDetails_[id].outgoingLinks_.end(); ++it) 
		{
			tmpPageRank[*it] += pageRank_[id] / pagesOutgoingLinksCount_[id];
			if (flag[*it] != valueVisited) {
				q.push(*it);
                flag[*it] = valueVisited;
            }
		}
	}


}

void PagesStatist::getTopPages(size_t n, std::vector<Url>& oTopPages) {
	oTopPages.clear();
	for (size_t i = 0; i < n; ++i) {
		oTopPages.push_back(urls_[idsSortedbByPr_[i].first]);
	}
}

void PagesStatist::getPageSizesInBytes(std::vector<size_t>& oSizes) {
	oSizes.clear();
	oSizes.reserve(pageDetails_.size());
	for (auto it = pageDetails_.begin(); it != pageDetails_.end(); ++it) {
		oSizes.push_back(it->sizeInBytes_);
	}
}

const std::vector<size_t>& PagesStatist::getPagesOutgoingLinksCount() {
	if (pagesOutgoingLinksCount_.size() == 0) {
		pagesOutgoingLinksCount_.reserve(pageDetails_.size());
		for (auto it = pageDetails_.begin(); it != pageDetails_.end(); ++it) {
			pagesOutgoingLinksCount_.push_back(it->outgoingLinks_.size());
		}
	}
	return pagesOutgoingLinksCount_;
}
