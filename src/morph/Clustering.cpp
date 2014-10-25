// Author: Tim Diels <timdiels.m@gmail.com>

#include "Clustering.h"
#include "util.h"
#include <boost/spirit/include/qi.hpp>

using namespace std;

Clustering::Clustering(string path, GeneExpression& gene_expression_)
:	name(path), gene_expression(gene_expression_)
{
	// Load
	read_file(path, [this](const char* begin, const char* end) {
		using namespace boost::spirit::qi;
		using namespace boost::fusion;

		std::map<std::string, Cluster*> cluster_map;
		auto on_cluster_item = [this, &cluster_map](const boost::fusion::vector<std::string, std::string>& item) {
			auto gene_name = at_c<0>(item);
			auto cluster_id = at_c<1>(item);
			auto it = cluster_map.find(cluster_id);
			if (it == cluster_map.end()) {
				clusters.emplace_back(cluster_id);
				it = cluster_map.emplace(cluster_id, &clusters.back()).first;
			}
			auto index = gene_expression.get_gene_index(gene_name);
			it->second->add(index);
			genes.emplace(index);
		};
		phrase_parse(begin, end, (as_string[lexeme[+(char_-space)]] > as_string[lexeme[+(char_-space)]])[on_cluster_item] % eol, blank);
		return begin;
	});
}

const std::vector<Cluster>& Clustering::get_clusters() const {
	return clusters;
}

GeneExpression& Clustering::get_source() const {
	return gene_expression;
}

const std::unordered_set<size_type>& Clustering::get_genes() const {
	return genes;
}
