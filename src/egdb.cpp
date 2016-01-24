#include <tuple> // for tie
#include <algorithm> // for for_each
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "EGDB.h"

namespace egdb {

using rapidjson::GetValueByPointer;

std::string rapidjson_to_string(const rapidjson::Value& doc) {
	rapidjson::StringBuffer strBuf;
	rapidjson::Writer<rapidjson::StringBuffer> write(strBuf);
	doc.Accept(write);
	return strBuf.GetString();
}

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

bool EGDB::insert(rapidjson::Document& d) {
	bool sucess = true;
	Vertex fd = this->vertexRegister(d); // d become empty
	//boost::format fmt("[%d] %s");
	//std::cout << (fmt % fd % rapidjson_to_string(this->graph[fd])).str() << std::endl;
	this->vertices_.insert(fd);
	//auto paths = GetDocumentPath(this->content_.back());
	auto paths = GetDocumentPath(this->graph[fd]);
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
				if (index != "/@id" && boost::ends_with(index, "/@id")) {
					// is a link
					//std::cerr << index << " " << "LINK" << std::endl;
					assert(value.type() == typeid(std::string));
					Vertex target = this->vertexRegister(value.get_refer<std::string>());
					//std::cerr << "FROM " << fd << " TO " << target << std::endl;
					Edge e;
					std::tie(e, std::ignore) = boost::add_edge(fd, target, this->graph);
					EdgeNameMap edge2name = boost::get(boost::edge_name, this->graph); // 这里是引用吗?
					std::string newIndex = boost::replace_last_copy(index, "/@id", "");
					boost::regex_replace(newIndex, boost::regex("/\\d+"), "");
					edge2name[e] = newIndex;
					//std::cerr << edge2name[e] << std::endl;
				} else if (boost::ends_with(index, "/@value")) {
					// is a value mix link
					std::string newIndex = boost::replace_last_copy(index, "/@value", "");
					sucess &= this->addTermIndex(newIndex, value, fd);
					sucess &= this->addTermIndex(index, value, fd);
				} else {
					sucess &= this->addTermIndex(index, value, fd);
				}
			}
		);
	}
	return sucess;
}

bool EGDB::addHashIndex(const std::string& path, const any& value, const size_t& fd) {
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

EGDB::Vertex EGDB::vertexRegister(rapidjson::Document& doc) {
	//std::cerr << rapidjson_to_string(doc) << std::endl;
	auto type = GetValueByPointer(doc, "/@type");
	auto id = GetValueByPointer(doc, "/@id");
	assert(type != nullptr);
	assert(id != nullptr);
	Vertex v = this->vertexRegister(id->GetString());
	std::swap(this->graph[v], doc);
	//std::cerr << v << " ID " << id->GetString() << std::endl;
	return v;
}

EGDB::Vertex EGDB::vertexRegister(const std::string& id) {
	assert(!id.empty());
	Vertex v;
	auto it = this->total_.find(id);
	if (it == this->total_.end()) {
		//std::cerr << "BEFORE " << boost::num_vertices(this->graph) << std::endl;
		v = boost::add_vertex(this->graph);
		//std::cerr << v << " AFTER " << boost::num_vertices(this->graph) << std::endl;
		std::tie(it, std::ignore) = this->total_.insert(std::make_pair(id, v));
	} else {
		//std::cerr << "id " << id << " is In TOTAL " << this->total_.size() << std::endl;
		//for(auto& x: this->total_) std::cerr << x.first.get_refer<std::string>() << std::endl;
		v = it->second;
	}
	return v;
}

bool EGDB::addTermIndex(const std::string& path, const any& value, const size_t& fd) {
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

std::set<EGDB::Vertex> 
EGDB::getInV(const EGDB::Vertex v) const {
	std::set<Vertex> response;
	auto range = boost::in_edges(v, this->graph);
	for (auto it = range.first; it != range.second; ++it) {
		//const EdgeNameMap& map = boost::get(boost::edge_name, this->graph);
		response.insert(boost::source(*it, this->graph));
	}
	return response;
}

std::set<EGDB::Vertex> 
EGDB::getOutV(const EGDB::Vertex v) const {
	std::set<Vertex> response;
	auto range = boost::out_edges(v, this->graph);
	for (auto it = range.first; it != range.second; ++it) {
		response.insert(boost::target(*it, this->graph));
	}
	return response;
}

std::set<EGDB::Vertex> 
EGDB::getInV(const EGDB::Vertex v, const std::string& name) const {
	std::set<Vertex> response;
	auto range = boost::in_edges(v, this->graph);
	for (auto it = range.first; it != range.second; ++it) {
		EdgeNameMap map = boost::get(boost::edge_name, const_cast<Graph&>(this->graph));
		if (map[*it] != name) continue;
		response.insert(boost::source(*it, this->graph));
	}
	return response;
}

std::set<EGDB::Vertex> 
EGDB::getOutV(const EGDB::Vertex v, const std::string& name) const {
	std::set<Vertex> response;
	auto range = boost::out_edges(v, this->graph);
	for (auto it = range.first; it != range.second; ++it) {
		EdgeNameMap map = boost::get(boost::edge_name, const_cast<Graph&>(this->graph));
		if (map[*it] != name) continue;
		response.insert(boost::target(*it, this->graph));
	}
	return response;
}

size_t EGDB::size() const {
	//return this->content_.size();
	return boost::num_vertices(this->graph);
}

size_t EGDB::indexSize(const std::string& path) const {
	auto it = this->term_.find(path);
	if (it == this->term_.end()) {
		return 0;
	}
	return it->second.size();
}

GraphHandle EGDB::query() const {
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
		return this->graph_.vertices_.begin();
	} else {
		return this->buffer_.begin();
	}
}

GraphHandle::Iterator GraphHandle::end() const {
	if (this->status_ == REFER) {
		return this->graph_.vertices_.end();
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
		for (auto& x: coll.second) merge.insert(x);
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

GraphHandle& GraphHandle::outV(const std::string& name) {
	std::set<EGDB::Vertex> merge;
	for (auto it = this->begin(); it != this->end(); ++it) {
		auto outV = this->graph_.getOutV(*it, name);
		for (auto& x: outV) merge.insert(x);
	}
	std::swap(this->buffer_, merge);
	this->setHolder(this->buffer_);
	return *this;
}

GraphHandle& GraphHandle::inV(const std::string& name) {
	std::set<EGDB::Vertex> merge;
	for (auto it = this->begin(); it != this->end(); ++it) {
		auto outV = this->graph_.getInV(*it, name);
		for (auto& x: outV) merge.insert(x);
	}
	std::swap(this->buffer_, merge);
	this->setHolder(this->buffer_);
	return *this;
}
}
