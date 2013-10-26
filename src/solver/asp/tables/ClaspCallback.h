/*
Copyright 2012-2013, Bernhard Bliem
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

#include "../ClaspCallback.h"
#include "GringoOutputProcessor.h"

namespace solver { namespace asp { namespace tables {

class ClaspCallback : public ::solver::asp::ClaspCallback
{
public:
	typedef AtomInfo<GringoOutputProcessor::ExtendAtomArguments>      ExtendAtomInfo;
	typedef AtomInfo<GringoOutputProcessor::ItemAtomArguments>        ItemAtomInfo;
	typedef AtomInfo<GringoOutputProcessor::CountAtomArguments>       CountAtomInfo;
	typedef AtomInfo<GringoOutputProcessor::CurrentCostAtomArguments> CurrentCostAtomInfo;
	typedef AtomInfo<GringoOutputProcessor::CostAtomArguments>        CostAtomInfo;

	typedef std::vector<ItemAtomInfo>        ItemAtomInfos;
	typedef std::vector<ExtendAtomInfo>      ExtendAtomInfos;
	typedef std::vector<CountAtomInfo>       CountAtomInfos;
	typedef std::vector<CurrentCostAtomInfo> CurrentCostAtomInfos;
	typedef std::vector<CostAtomInfo>        CostAtomInfos;

	ClaspCallback(const GringoOutputProcessor& gringoOutput, const ChildItemTrees& childItemTrees, bool printModels, bool prune);

	// Called on entering/exiting a state
#ifndef GCC46_COMPATIBILITY
	virtual void state(Clasp::ClaspFacade::Event, Clasp::ClaspFacade&) override;
#else
	virtual void state(Clasp::ClaspFacade::Event, Clasp::ClaspFacade&);
#endif

	// Called for important events, e.g. a model has been found
#ifndef GCC46_COMPATIBILITY
	virtual void event(const Clasp::Solver& s, Clasp::ClaspFacade::Event e, Clasp::ClaspFacade& f) override;
#else
	virtual void event(const Clasp::Solver& s, Clasp::ClaspFacade::Event e, Clasp::ClaspFacade& f);
#endif

protected:
	const GringoOutputProcessor& gringoOutput;

	ItemAtomInfos        itemAtomInfos;
	ExtendAtomInfos      extendAtomInfos;
	CountAtomInfos       countAtomInfos;
	CurrentCostAtomInfos currentCostAtomInfos;
	CostAtomInfos        costAtomInfos;
};

}}} // namespace solver::asp::tables
