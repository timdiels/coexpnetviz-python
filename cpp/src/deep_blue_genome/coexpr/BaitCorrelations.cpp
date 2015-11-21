/*
 * Copyright (C) 2015 VIB/BEG/UGent - Tim Diels <timdiels.m@gmail.com>
 *
 * This file is part of Deep Blue Genome.
 *
 * Deep Blue Genome is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Deep Blue Genome is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Deep Blue Genome.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <deep_blue_genome/coexpr/stdafx.h>
#include "BaitCorrelations.h"

using namespace std;
using namespace DEEP_BLUE_GENOME;

namespace DEEP_BLUE_GENOME {
namespace COEXPR {

BaitCorrelations::BaitCorrelations(const Gene& bait)
:	bait(bait)
{
}

const Gene& BaitCorrelations::get_bait() const {
	return bait;
}

double BaitCorrelations::get_max_correlation() const {
	double max_ = correlations.front().second;
	for (auto& p : correlations) {
		max_ = max(p.second, max_);
	}
	return max_;
}

void BaitCorrelations::add_correlation(const Gene& target, double correlation) {
	correlations.emplace_back(&target, correlation);
}

}} // end namespace