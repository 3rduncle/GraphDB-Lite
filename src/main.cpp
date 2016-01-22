#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "EmbeddedGraph.h"

int main() {
	rapidjson::Document d,d1,d2;
	d.Parse("{\"name\":\"cm\",\"alias\":[\"厘米\",\"公尺\"], \"factor\":100, \"domain\":[{\"@value\":\"单位\",\"@id\":\"Unit\"}]}");
	auto flate = GetDocumentPath(d);
	for (auto x:flate) {
		std::cout << x.first << std::endl;
	}
	d1.Parse("124124");
	flate = GetDocumentPath(d1);
	for (auto x: flate) {
		std::cout << x.first << std::endl;
	}
	d2.Parse("[12,123,124]");
	flate = GetDocumentPath(d2);
	for (auto x: flate) {
		std::cout << x.first << std::endl;
	}
	std::vector<rapidjson::Document> vec;
	for (size_t i = 0; i < 1000; ++i) {
		vec.push_back(rapidjson::Document());
		vec.rbegin()->Parse("[12,123,234]");
	}
	std::cout << vec.size() << std::endl;
	std::cout << std::boolalpha << vec.begin()->IsArray() << std::endl;
	std::cout << std::boolalpha << vec.rbegin()->IsArray() << std::endl;

	any a = "12345";
	any b = 12345;
	std::vector<any> veca;
	veca.push_back(a);
	std::cout << "Vec Push Sucess! " << veca.size() << std::endl;

	std::multimap<any, any> ma;
	ma.insert(std::make_pair(a, b));
	std::cout << "Map Push Sucess! " << ma.size() << std::endl;

	std::unordered_map<any, any> um;
	um.insert(std::make_pair(a, b));
	std::cout << "hash Map Push Sucess! " << um.size() << std::endl;

	EmbeddedGraph eg;
	rapidjson::Document dd1, dd2, dd3;
	dd1.Parse("{\"name\":\"cm\",\"alias\":[\"厘米\",\"公尺\"], \"factor\":100, \"domain\":[{\"@value\":\"单位\",\"@id\":\"Unit\"}],\"coordinator\":[1,2,3]}");
	dd2.Parse("{\"name\":\"m\",\"alias\":[\"米\",\"公尺\"], \"factor\":100, \"domain\":[{\"@value\":\"长度\",\"@id\":\"Unit\"}],\"coordinator\":[2,3]}");
	dd3.Parse("{\"name\":\"m\",\"@type\":\"Unit\",\"alias\":[\"米\",\"公尺\"], \"factor\":100, \"domain\":[{\"@value\":\"长度\",\"@id\":\"Unit\"}]}");

	eg.insert(dd1);
	eg.insert(dd2);
	eg.insert(dd3);
	std::cout << eg.size() << std::endl;
	std::cout << "alias index size " << eg.indexSize("/alias") << std::endl;
	auto res = eg.query().has("/alias", "米").has("/@type","Unit").toArray();
	std::cout << "Name of M size is " << res.size() << std::endl;
	const rapidjson::Value* v1 = rapidjson::GetValueByPointer(*res.back(), "/name");
	const rapidjson::Value* v2 = rapidjson::GetValueByPointer(*res.back(), "/@type");
	std::cout << "My name is " << v1->GetString() << std::endl;


	std::cout << "coorder index size " << eg.query().has("/coordinator").size() << std::endl;

	auto xx = GenerateMore("/alias/3/abcdage");
	for (auto x: xx) {
		std::cout << x << std::endl;
	}

	std::fstream fin("./test.conf");
	std::string line = "";
	EmbeddedGraph graph;
	while (std::getline(fin, line)) {
		rapidjson::Document doc;
		doc.Parse(line.c_str());
		graph.insert(doc);
	}
	auto handle = graph.query().has("/name", "长度");
	std::cout << handle.size() << std::endl;
	std::cout << handle.has("/coordinator").size() << std::endl;
}
