/*{{{
Copyright 2012-2016, Bernhard Bliem
WWW: <http://dbai.tuwien.ac.at/research/project/dflat/>.

This file is part of D-FLAT.

D-FLAT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

D-FLAT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with D-FLAT.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
//}}}
#include "../Decomposer.h"
#include "../options/Choice.h"

namespace decomposer {

class TreeDecomposer : public Decomposer
{
public:
	TreeDecomposer(Application& app, bool newDefault = false);

	virtual DecompositionPtr decompose(const Instance& instance) const override;

private:
	static const std::string OPTION_SECTION;
	static const int DEFAULT_ITERATION_COUNT;

	options::Choice            optNormalization;
	options::Choice            optEliminationOrdering;
	options::Choice            optFitnessCriterion;
	options::SingleValueOption optIterationCount;
	options::Option            optNoEmptyRoot;
	options::Option            optNoEmptyLeaves;
	options::Option            optPostJoin;
	options::Option            optPathDecomposition;

	unsigned heurMode;
	std::string heurPred;
};

} // namespace decomposer
