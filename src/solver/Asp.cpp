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

#include <sstream>
#include <cassert>
#include <gringo/streams.h>
#include <clasp/clasp_facade.h>

#include "Asp.h"
#include "../Application.h"
#include "../ItemTree.h"
#include "../Decomposition.h"
#include "asp/tables/GringoOutputProcessor.h"
#include "asp/tables/ClaspCallback.h"
#include "asp/trees/GringoOutputProcessor.h"
#include "asp/trees/ClaspCallback.h"
#include "asp/ClaspInputReader.h"

using namespace solver::asp;
using ChildItemTrees = GringoOutputProcessor::ChildItemTrees;

namespace {

void declareDecomposition(const Decomposition& decomposition, std::ostream& out)
{
	out << "currentNode(" << decomposition.getRoot().getGlobalId() << ")." << std::endl;
	for(const auto& v : decomposition.getRoot().getBag()) {
		out << "bag(" << decomposition.getRoot().getGlobalId() << ',' << v << "). ";
		out << "current(" << v << ")." << std::endl;
	}

	for(const auto& child : decomposition.getChildren()) {
		out << "childNode(" << child->getRoot().getGlobalId() << ")." << std::endl;
		for(const auto& v : child->getRoot().getBag()) {
			out << "bag(" << child->getRoot().getGlobalId() << ',' << v << "). ";
			out << "-introduced(" << v << ")." << std::endl; // Redundant
		}
	}

	if(decomposition.getParents().empty())
		out << "final." << std::endl;
	else {
		for(const auto& parent : decomposition.getParents()) {
			out << "parentNode(" << parent->getRoot().getGlobalId() << ")." << std::endl;
			for(const auto& v : parent->getRoot().getBag())
				out << "bag(" << parent->getRoot().getGlobalId() << ',' << v << ")." << std::endl;
		}
	}

	// Redundant predicates for convenience...
	out << "introduced(X) :- current(X), not -introduced(X)." << std::endl;
	out << "removed(X) :- childNode(N), bag(N,X), not current(X)." << std::endl;
}

void declareChildItemTree(std::ostream& out, const ItemTreePtr& itemTree, bool tableMode, unsigned int nodeId, const std::string& itemSetName, const std::string& parent = "", unsigned int level = 0)
{
	if(!itemTree)
		return;

	// Declare this item set
	if(tableMode) {
		if(parent.empty() == false)
			out << "childRow(" << itemSetName << ',' << nodeId << ")." << std::endl;
	} else {
		out << "atLevel(" << itemSetName << ',' << level << ")." << std::endl;
		out << "atNode(" << itemSetName << ',' << nodeId << ")." << std::endl;
		if(parent.empty()) {
			out << "root(" << itemSetName << ")." << std::endl;
			out << "rootOf(" << itemSetName << ',' << nodeId << ")." << std::endl;
		} else {
			out << "sub(" << parent << ',' << itemSetName << ")." << std::endl;
			if(itemTree->getChildren().empty()) {
				out << "leaf(" << itemSetName << ")." << std::endl;
				out << "leafOf(" << itemSetName << ',' << nodeId << ")." << std::endl;
			}
		}
	}
	for(const auto& item : itemTree->getRoot()->getItems())
		out << "childItem(" << itemSetName << ',' << item << ")." << std::endl;

	// If this is a leaf, declare cost
	// TODO count etc.
	const ItemTree::Children& children = itemTree->getChildren();
	if(children.empty()) {
		out << "childCost(" << itemSetName << ',' << itemTree->getRoot()->getCost() << ")." << std::endl;
	}
	else {
		// Declare child item sets
		size_t i = 0;
		for(const auto& child : children) {
			std::ostringstream childName;
			childName << itemSetName << '_' << i++;
			declareChildItemTree(out, child, tableMode, nodeId, childName.str(), itemSetName, level+1);
		}
	}
}

std::unique_ptr<GringoOutputProcessor> newGringoOutputProcessor(const ChildItemTrees& childItemTrees, bool tableMode)
{
	if(tableMode)
		return std::unique_ptr<GringoOutputProcessor>(new tables::GringoOutputProcessor(childItemTrees));
	else
		return std::unique_ptr<GringoOutputProcessor>(new trees::GringoOutputProcessor(childItemTrees));
}

std::unique_ptr<ClaspCallback> newClaspCallback(bool tableMode, const GringoOutputProcessor& gringoOutputProcessor, const ChildItemTrees& childItemTrees, bool printModels)
{
	if(tableMode)
		return std::unique_ptr<ClaspCallback>(new tables::ClaspCallback(dynamic_cast<const tables::GringoOutputProcessor&>(gringoOutputProcessor), childItemTrees, printModels));
	else
		return std::unique_ptr<ClaspCallback>(new trees::ClaspCallback(dynamic_cast<const trees::GringoOutputProcessor&>(gringoOutputProcessor), childItemTrees, printModels));
}


} // anonymous namespace

namespace solver {

Asp::Asp(const Decomposition& decomposition, const Application& app, const std::string& encodingFile, bool tableMode)
	: Solver(decomposition, app)
	, encodingFile(encodingFile)
	, tableMode(tableMode)
{
}

ItemTreePtr Asp::compute()
{
	// Compute item trees of child nodes
	ChildItemTrees childItemTrees;
	for(const auto& child : decomposition.getChildren()) {
		ItemTreePtr itree = child->getSolver().compute();
		if(!itree)
			return itree;
		childItemTrees.emplace(child->getRoot().getGlobalId(), std::move(itree));
	}

	// Input: Child item trees
	std::unique_ptr<std::stringstream> childItemTreesInput(new std::stringstream);
	for(const auto& childItemTree : childItemTrees) {
		std::ostringstream rootItemSetName;
		rootItemSetName << 'n' << childItemTree.first;
		declareChildItemTree(*childItemTreesInput, childItemTree.second, tableMode, childItemTree.first, rootItemSetName.str());
	}

	// Input: Original problem instance
	std::unique_ptr<std::stringstream> instanceInput(new std::stringstream);
	*instanceInput << app.getInputString();

	// Input: Decomposition
	std::unique_ptr<std::stringstream> decompositionInput(new std::stringstream);
	declareDecomposition(decomposition, *decompositionInput);

	// Put these inputs together
	Streams inputStreams;
	inputStreams.addFile(encodingFile, false); // Second parameter: "relative" here means relative to the file added previously, which does not exist yet
	// Remember: "Streams" deletes the appended streams -_-
	inputStreams.appendStream(Streams::StreamPtr(instanceInput.release()), "<instance>");
	inputStreams.appendStream(Streams::StreamPtr(decompositionInput.release()), "<decomposition>");
	inputStreams.appendStream(Streams::StreamPtr(childItemTreesInput.release()), "<child_itrees>");

	// Call the ASP solver
	std::unique_ptr<GringoOutputProcessor> outputProcessor(newGringoOutputProcessor(childItemTrees, tableMode));
	ClaspInputReader inputReader(inputStreams, *outputProcessor);
	std::unique_ptr<ClaspCallback> cb(newClaspCallback(tableMode, *outputProcessor, childItemTrees, app.isDebugEnabled()));
	Clasp::ClaspConfig config;
	config.enumerate.numModels = 0;
	Clasp::ClaspFacade clasp;
	clasp.solve(inputReader, config, cb.get());

	ItemTreePtr result = cb->finalize();

	if(app.isDebugEnabled()) {
		if(result) {
			std::cerr << "Resulting item tree of node " << decomposition.getRoot().getGlobalId() << ':' << std::endl << *result << std::endl;
			// XXX
			std::cerr << "Extensions:\n";
			result->printExtensions(std::cerr);
		}
		else
			std::cerr << "Item tree of node " << decomposition.getRoot().getGlobalId() << " is empty." << std::endl;
	}

	return result;
}

} // namespace solver