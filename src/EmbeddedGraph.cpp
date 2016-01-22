#include <tuple> // for tie
#include <algorithm> // for for_each
#include <boost/algorithm/string.hpp>
#include "EmbeddedGraph.h"

any ConvertToAny(const rapidjson::Value& j) {
	if (j.IsString()) {
		return j.GetString();
	} else if (j.IsDouble()) {
		return j.GetDouble();
	} else if (j.IsInt()) {
		return j.GetInt();
	} else if (j.IsBool()) {
		return j.GetBool();
	} else {
		return any(0);
	}
}

std::vector<std::string> GenerateMore(const std::string& path) {
	std::vector<std::string> res;
	res.push_back(path);
	boost::regex array_r("/\\d");
	if (boost::regex_search(path, array_r)) {
		res.push_back(boost::regex_replace(path, array_r, ""));
	}
	return res;
}

std::map<std::string, const rapidjson::Value*>
GetDocumentPath(const rapidjson::Value& document, const std::string& seq) {
	if (document.IsObject()) {
		return GetObjectDocumentPath(document, seq);
	} else if (document.IsArray()) {
		return GetArrayDocumentPath(document, seq);
	} else {
		std::map<std::string, const rapidjson::Value*> res;
		res.insert(std::make_pair("/0", &document));
		return res;
	}
}

std::map<std::string, const rapidjson::Value*>
GetArrayDocumentPath(const rapidjson::Value& document, const std::string& seq) {
	assert(document.IsArray());
	std::map<std::string, const rapidjson::Value*> response;
	size_t i = 0;
	for (auto it = document.Begin(); it != document.End(); ++it, ++i) {
		std::string key = seq + boost::lexical_cast<std::string>(i);
		if (it->IsObject()) {
			auto sub = GetObjectDocumentPath(*it, seq);
			for (auto& psub: sub) {
				auto p = std::make_pair(key + psub.first, psub.second);
				response.insert(p);
			}
		} else if (it->IsArray()) {
			auto sub = GetArrayDocumentPath(*it, seq);
			for (auto& psub: sub) {
				auto p = std::make_pair(key + psub.first, psub.second);
				response.insert(p);
			}
		} else {
			response.insert(std::make_pair(key, &(*it)));
		}
	}
	return response;
}

std::map<std::string, const rapidjson::Value*>
GetObjectDocumentPath(const rapidjson::Value& document, const std::string& seq) {
	assert(document.IsObject());
	std::map<std::string, const rapidjson::Value*> response;
	for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it) {
		std::string key = seq + it->name.GetString();
		if (it->value.IsObject()) {
			auto sub = GetObjectDocumentPath(it->value, seq);
			for (auto& psub: sub) {
				auto p = std::make_pair(key + psub.first, psub.second);
				response.insert(p);
			}
		} else if (it->value.IsArray()) {
			auto sub = GetArrayDocumentPath(it->value, seq);
			for (auto& psub: sub) {
				auto p = std::make_pair(key + psub.first, psub.second);
				response.insert(p);
			}
		} else {
			response.insert(std::make_pair(key, &(it->value)));
		}
	}
	return response;
}

bool EmbeddedGraph::insert(rapidjson::Document& d) {
	bool sucess = true;
	this->content_.push_back(rapidjson::Document());
	std::swap(this->content_.back(), d); // d become empty
	size_t fd = this->content_.size() - 1;  // 索引为长度-1
	this->collection_.insert(fd);
	auto paths = GetDocumentPath(this->content_.back());
	for (auto& p: paths) {
		std::string path = p.first;
		if (p.second->IsNull()) {
			continue;
		}
		any value = ConvertToAny(*p.second);
		auto indexes = GenerateMore(path);
		std::for_each(indexes.begin(), indexes.end(), 
			[&value, &sucess, &fd, this](const std::string& index) {
				//std::cerr << "Insert " << index << " " << fd << std::endl;
				sucess &= this->addTermIndex(index, value, fd);
			}
		);
	}
	return sucess;
}

bool EmbeddedGraph::addHashIndex(const std::string& path, const any& value, const size_t& fd) {
	bool found = true;
	auto it = this->hash_.find(path);
	if (it == this->hash_.end()) {
		std::tie(it, found) = this->hash_.insert(std::make_pair(path, HashIndex()));
	}
	if (!found) {
		return false;
	}	
	auto status = it->second.insert(std::make_pair(value, fd));
	return status.second;
}

bool EmbeddedGraph::addTermIndex(const std::string& path, const any& value, const size_t& fd) {
	bool found = true;
	auto it = this->term_.find(path);
	if (it == this->term_.end()) {
		std::tie(it, found) = this->term_.insert(std::make_pair(path, TermIndex()));
	}
	if (!found) return false;
	auto coll = it->second.find(value);
	if (coll == it->second.end()) {
		std::tie(coll, found) = it->second.insert(std::make_pair(value, std::set<size_t>()));
	}
	if (!found) return false;
	auto status = coll->second.insert(fd);
	return status.second;
}

size_t EmbeddedGraph::size() const {
	return this->content_.size();
}

size_t EmbeddedGraph::indexSize(const std::string& path) const {
	auto it = this->term_.find(path);
	if (it == this->term_.end()) {
		return 0;
	}
	return it->second.size();
}

GraphHandle EmbeddedGraph::query() const {
	GraphHandle handle(*this);
	//handle.setRange(this->collection_);
	return handle;
}

void GraphHandle::setHolder(const std::set<size_t>& source) {
	//std::cerr << "Range " << (this->begin() == this->buffer_.begin()) << std::endl;
	this->changeStatus(HOLDER);
	std::copy(source.begin(), source.end(),
			std::inserter(this->buffer_, this->buffer_.begin()));
	//std::cerr << "setRange " << this->size_ << std::endl;
	//std::cerr << "Range " << (this->begin() == this->buffer_.begin()) << std::endl;
}

void GraphHandle::setRefer() {
	this->changeStatus(REFER);
}

void GraphHandle::clearHolder() {
	this->changeStatus(HOLDER);
	this->buffer_.clear();
}

size_t GraphHandle::size() const {
	if (this->status_ == REFER) {
		return this->graph_.size();
	} else {
		return this->buffer_.size();
	}
}

bool GraphHandle::empty() const {
	return this->size() == 0;
}

GraphHandle::Iterator GraphHandle::begin() const {
	if (this->status_ == REFER) {
		return this->graph_.collection_.begin();
	} else {
		return this->buffer_.begin();
	}
}

GraphHandle::Iterator GraphHandle::end() const {
	if (this->status_ == REFER) {
		return this->graph_.collection_.end();
	} else {
		return this->buffer_.end();
	}
}

GraphHandle& GraphHandle::has(const std::string& path) {
	auto found = this->graph_.term_.find(path);
	if (found == this->graph_.term_.end()) {
		this->clearHolder();
		//std::cerr << "Not Path" << std::endl;
		return *this;
	}
	std::set<size_t> merge;
	for (const auto& coll: found->second) {
		if (merge.empty()) {
			std::copy(coll.second.begin(), 
					coll.second.end(),
					std::inserter(merge, merge.begin()));
		} else {
			std::set<size_t> tmp;
			std::set_union(merge.begin(),
					merge.end(),
					coll.second.begin(),
					coll.second.end(),
					std::inserter(tmp, tmp.begin()));
			std::swap(merge, tmp);
		}
		if (merge.size() >= this->graph_.size()) {
			break;
		}
	}
	//std::cout << "EQUAL? " << std::boolalpha << (this->begin() == this->buffer_.begin()) << std::endl;
	//for (auto it = this->begin(); it != this->end(); ++it) std::cout << "THIS " << *it << std::endl;
	//for (auto& x: merge) std::cout << "MERGE " << x << std::endl;
	//for (auto& x: this->buffer_) std::cout << "BUFFER1 " << x << std::endl;
	std::set<size_t> tmp;
	//for (auto& x: tmp) std::cout << "TMP1 " << x << std::endl;
	std::set_intersection(this->begin(),
			this->end(),
			merge.begin(),
			merge.end(),
			std::inserter(tmp, tmp.begin()));
	//for (auto& x: this->buffer_) std::cout << "BUFFER2 " << x << std::endl;
	//for (auto& x: tmp) std::cout << "TMP2 " << x << std::endl;
	std::swap(this->buffer_, tmp);
	this->setHolder(this->buffer_);
	//for (auto& x: this->buffer_) std::cout << "BUFFER3 " << x << std::endl;
	//for (auto& x: tmp) std::cout << "TMP3 " << x << std::endl;
	return *this;
}

GraphHandle& GraphHandle::has(const std::string& path, const any& value) {
	auto found = this->graph_.term_.find(path);
	if (found == this->graph_.term_.end()) {
		this->clearHolder();
		//std::cerr << "Not Path" << std::endl;
		return *this;
	}
	auto coll = found->second.find(value);
	if (coll == found->second.end() || coll->second.empty()) {
		this->clearHolder();
		//std::cerr << "Not Value" << std::endl;
		return *this;
	}
	std::set<size_t> tmp;
	std::set_intersection(this->begin(),
			this->end(),
			coll->second.begin(),
			coll->second.end(),
			std::inserter(tmp, tmp.begin()));
	//std::cerr << "SWAP " << (this->begin() == this->buffer_.begin()) << std::endl;
	std::swap(this->buffer_, tmp);
	this->setHolder(this->buffer_);
	//std::cerr << "SWAP " << (this->begin() == this->buffer_.begin()) << std::endl;
	return *this;
}
