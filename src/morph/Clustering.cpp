// Author: Tim Diels <timdiels.m@gmail.com>

#include "Clustering.h"
#include "util.h"
#include <boost/spirit/include/qi.hpp>

using namespace std;

Clustering::Clustering(string path, GeneExpression& gene_expression_)
:	name(path), gene_expression(gene_expression_)
{
	std::unordered_set<size_type> genes;

	// Load
	read_file(path, [this, &genes](const char* begin, const char* end) {
		using namespace boost::spirit::qi;
		using namespace boost::fusion;

		std::map<std::string, Cluster*> cluster_map;
		auto on_cluster_item = [this, &cluster_map, &genes](const boost::fusion::vector<std::string, std::string>& item) {
			auto gene_name = at_c<0>(item);
			if (!gene_expression.has_gene(gene_name)) {
				// Note: in case clusterings are not generated by us, they might contain genes that we don't know
#ifndef NDEBUG
				cerr << "Warning: gene missing from expression matrix: " << gene_name << endl;
#endif
				return;
			}
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
		phrase_parse(begin, end, (as_string[lexeme[+(char_-space)]] > as_string[lexeme[+(char_-eol)]])[on_cluster_item] % eol, blank);
		return begin;
	});

	// Group together unclustered genes
	clusters.emplace_back("unclustered");
	auto& cluster = clusters.back();
	for (auto& p : gene_expression.get_genes()) {
		auto gene = p.second;
		if (!contains(genes, gene)) {
			cluster.add(gene);
		}
	}
	if (cluster.empty()) {
		clusters.pop_back();
	}
}

const std::vector<Cluster>& Clustering::get_clusters() const {
	return clusters;
}

GeneExpression& Clustering::get_source() const {
	return gene_expression;
}
