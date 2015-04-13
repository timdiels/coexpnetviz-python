// Author: Tim Diels <timdiels.m@gmail.com>

#pragma once

#include <string>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/range/adaptors.hpp>
#include <memory>
#include <deep_blue_genome/common/Serialization.h>
#include <deep_blue_genome/common/types.h>
#include <deep_blue_genome/common/ublas.h> // TODO would compile speed up much without this in many headers?

namespace DEEP_BLUE_GENOME {

class GeneCollection;
class Gene;
class DataFileImport;

// TODO each row label was originally a probe id, does that map to a gene in general, or rather a specific gene variant? What does it measure specifically? Currently we assume each row is a .1 gene
/**
 * Gene expression matrix
 *
 * A matrix can contain genes of multiple gene collections
 *
 * matrix(i,j) = expression of gene i under condition j
 *
 * Note: these row indices are often used as gene ids in deep blue genome apps
 */
class GeneExpressionMatrix : public boost::noncopyable
{
	friend class DataFileImport;

public:
	/**
	 * Construct invalid instance
	 */
	GeneExpressionMatrix();

	/**
	 * Get index of row corresponding to given gene
	 */
	GeneExpressionMatrixRow get_gene_row(Gene&) const;
	Gene& get_gene(GeneExpressionMatrixRow) const;
	bool has_gene(const Gene&) const;

	/**
	 * Get range of genes in matrix
	 *
	 * @return concept Range<Gene*>
	 */
	auto get_genes() const {
		return gene_to_row | boost::adaptors::map_keys;
	}

	std::string get_name() const;
	std::string get_species_name() const;

	void dispose_expression_data();

	/**
	 * Get inner matrix representation
	 */
	const matrix& get() const;

public: // treat as private (failed to friend boost::serialization)
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);

private:
	std::string name; // name of dataset
	matrix expression_matrix; // row_major
	std::unordered_map<GeneExpressionMatrixRow, Gene*> row_to_gene; // TODO Bimap
	std::unordered_map<Gene*, GeneExpressionMatrixRow> gene_to_row; // inverse of gene_row_to_id
};

}


/////////////////////////
// hpp

namespace DEEP_BLUE_GENOME {

template<class Archive>
void GeneExpressionMatrix::serialize(Archive& ar, const unsigned int version) {
	ar & name;
	ar & expression_matrix; // TODO might want to lazy load this, or the class instance itself
	ar & row_to_gene;

	if (Archive::is_loading::value) {
		for (auto& p : row_to_gene) {
			gene_to_row.emplace(p.second, p.first);
		}
	}
}

}
