#include <map>
#include <boost/regex.hpp>
#include "rapidjson/document.h"
#include "struct/struct.h"

using ipython::any;

class GraphHandle;

class EmbeddedGraph {
public:
	typedef std::unordered_map<any, size_t>  HashIndex;
	typedef std::map<any, std::set<size_t> > TermIndex;
	typedef std::unordered_map<std::string, HashIndex> HashIndexCollection;
	typedef std::unordered_map<std::string, TermIndex> TermIndexCollection;
	bool insert(rapidjson::Document&);

	size_t size() const;
	size_t indexSize(const std::string& path) const;

	friend GraphHandle;
	GraphHandle query() const;

	const rapidjson::Document* get(const size_t& fd) const {
		if (fd > this->content_.size()) {
			return nullptr;
		}
		return &(this->content_.at(fd));
	}
private:
	bool addHashIndex(const std::string& path, const any& value, const size_t& fd);
	bool addTermIndex(const std::string& path, const any& value, const size_t& fd);
	std::vector<rapidjson::Document> content_;
	std::set<size_t> collection_;
	HashIndexCollection hash_;
	TermIndexCollection term_;
};

namespace Function {
struct MATCH {};
struct SEARCH {};
struct GT {};
struct LT {};
}

class GraphHandle {
public:
	typedef std::set<size_t>::iterator Iterator;
	enum Status {
		REFER = 0,
		HOLDER,
	};
	GraphHandle();
	GraphHandle(const EmbeddedGraph& g)
		: graph_(g)
	{
		this->status_ = REFER;
	}
	/*
	GraphHandle(const GraphHandle& handle) {
		this->status_ = handle.status_;
		this->graph_ = handle.graph_;
		this->setRange(handle.buffer_);
	}
	*/
	void setRefer();
	void setHolder(const std::set<size_t>& source);
	void clearHolder();
	std::pair<Iterator, Iterator> getRange() const;

	size_t size() const;
	bool empty() const;
	Iterator begin() const;
	Iterator end() const;
	//GraphHandle& operator=(const GraphHandle&);
	GraphHandle& operator|=(const GraphHandle&);
	GraphHandle& operator&=(const GraphHandle&);

	GraphHandle& has(const std::string& path);
	GraphHandle& has(const std::string&, const any& value);
	std::vector<const rapidjson::Document*> toArray() const {
		std::vector<const rapidjson::Document*> res;
		for (auto& x: this->buffer_) {
			//std::cerr << "Get " << x << std::endl;
			res.push_back(graph_.get(x));
		}
		return res;
	}
private:
	void changeStatus(const Status& s) {
		this->status_ = s;
	}
	Status status_;
	std::set<size_t> buffer_;
	const EmbeddedGraph& graph_;
};

std::map<std::string, const rapidjson::Value*>
GetDocumentPath(const rapidjson::Value& document, const std::string& seq = "/");

std::map<std::string, const rapidjson::Value*>
GetObjectDocumentPath(const rapidjson::Value& document, const std::string& seq = "/");

std::map<std::string, const rapidjson::Value*>
GetArrayDocumentPath(const rapidjson::Value& document, const std::string& seq = "/");

std::vector<std::string> GenerateMore(const std::string& path);
