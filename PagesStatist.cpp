#include "PagesStatist.h"


void PagesStatist::PagesStatict(const std::string& pagesDataFilePath) {
	FILE *file = fopen(pagesDataFilePath.c_str(), "w");
	size_t n;
	fscanf(file, "%zu", &n);
	pageDetails_.reserve(n);
	for (size_t i = 0; i < n; ++i) {
		PageId id;
		ClickDistance distanceFromMain;
    	size_t sizeInBytes;
    	fscanf(file, "%zu %zu %zu", &id, &distanceFromMain, &sizeInBytes);
	}
}

void PagesStatist::CalculatePageRank() {
	TMK_LOG("Calculating page rank\n");
	pageRank_.assign(pageDetails_.size(), 1);
	std::vector<PR> tmpPageRank(pageRank_);
	std::vector<bool> flag(pageDetails_.size(), false);
	GetPagesOutgoingLinksCount();
	bool valueVisited = true;
	std::queue<PageId> q;
	q.push(0);
	while (!q.empty()) {
		PageId id = q.front();
		q.pop();
		
		if (flag[id] == valueVisited)
			continue;

		for (auto it = pageDetails_[id].outgoingLinks_.begin(); it != pageDetails_[id].outgoingLinks_.end() ++it) 
		{
			tmpPageRank[*it] += pageRank_[id] / pagesOutgoingLinksCount_[id];
			if (flag[*it] != valueVisited)
				q.push(*it);
		}
		flag[id] = valueVisited;
	}


}

void PagesStatist::GetTopPages(size_t n, std::vector<Url>& oTopPages) {
	oTopPages.clear();
	for (size_t i = 0; i < n; ++i) {
		oTopPages.push_back(urls_[idsSortedbByPr_[i].first]);
	}
}

void PagesStatist::GetPageSizesInBytes(std::vector<size_t>& oSizes) {
	oSizes.clear();
	oSize.reserve(pageDetails_.size());
	for (auto it = pageDetails_.begin; it != pageDetails_.end(); ++it) {
		oSize.push_back(it->sizeInBytes_);
	}
}

const std::vector<size_t>& PagesStatist::GetPagesOutgoingLinksCount() {
	if (pagesOutgoingLinksCount_.size() == 0) {
		pagesOutgoingLinksCount_.reserve(pageDetails_.size());
		for (auto it = pageDetails_.begin; it != pageDetails_.end(); ++it) {
			pagesOutgoingLinksCount_.push_back(it->outgoingLinks_.size());
		}
	}
	return pagesOutgoingLinksCount_;
}
