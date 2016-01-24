#include <map>
#include <boost/regex.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <rapidjson/document.h>
#include "struct/struct.h"

namespace egdb {

class GraphHandle;

class EGDB {
public:
	// graph typedef
	typedef boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		boost::undirectedS,
		rapidjson::Document,
		boost::property<boost::edge_name_t, std::string>
	> Graph;
	typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<Graph>::edge_descriptor Edge;
	typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexIndexMap; // VertexIndexMap[Vertex] = Index
	typedef boost::property_map<Graph, boost::edge_name_t>::type EdgeNameMap; // EdgeNameMap[Edge] = Name

	// index typedef 
	typedef std::unordered_map<any, Vertex>  HashIndex;
	typedef std::map<any, std::set<Vertex> > TermIndex;
	typedef std::unordered_map<std::string, HashIndex> HashIndexCollection;
	typedef std::unordered_map<std::string, TermIndex> TermIndexCollection;

	friend GraphHandle;

	bool insert(rapidjson::Document&);
	GraphHandle query() const;
	const rapidjson::Document* get(const size_t& fd) const {
		if (fd > this->size()) {
			return nullptr;
		}
		//return &(this->content_.at(fd));
		return &(this->graph[fd]);
	}
	std::set<Vertex> getInV(const Vertex) const;
	std::set<Vertex> getOutV(const Vertex) const;
	std::set<Vertex> getInV(const Vertex, const std::string&) const;
	std::set<Vertex> getOutV(const Vertex, const std::string&) const;
	

	size_t size() const;
	size_t indexSize(const std::string& path) const;

private:
	Graph graph;
	Vertex vertexRegister(rapidjson::Document&);
	Vertex vertexRegister(const std::string&);
	bool addHashIndex(const std::string& path, const any& value, const size_t& fd);
	bool addTermIndex(const std::string& path, const any& value, const size_t& fd);
	std::set<Vertex> vertices_;
	HashIndex total_;
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
	typedef std::set<EGDB::Vertex>::iterator Iterator;
	enum Status {
		REFER = 0,
		HOLDER,
	};
	GraphHandle();
	GraphHandle(const EGDB& g)
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
	GraphHandle& inV(const std::string&);
	GraphHandle& outV(const std::string&);
	std::vector<EGDB::Vertex> toVertices() const {
		std::vector<EGDB::Vertex> res;
		res.assign(this->begin(), this->end());
		return res;
	}
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
	const EGDB& graph_;
};

std::map<std::string, const rapidjson::Value*>
GetDocumentPath(const rapidjson::Value& document, const std::string& seq = "/");

std::map<std::string, const rapidjson::Value*>
GetObjectDocumentPath(const rapidjson::Value& document, const std::string& seq = "/");

std::map<std::string, const rapidjson::Value*>
GetArrayDocumentPath(const rapidjson::Value& document, const std::string& seq = "/");

std::vector<std::string> GenerateMore(const std::string& path);

std::string rapidjson_to_string(const rapidjson::Value&);

}
