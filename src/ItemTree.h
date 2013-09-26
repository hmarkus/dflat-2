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

#include <memory>

#include "DAG.h"
#include "ItemTreeNode.h"

class ItemTree;
typedef std::unique_ptr<ItemTree> ItemTreePtr;

// The set of children is sorted ascendingly according to the following criterion:
// A TreePtr is smaller than another if
// (a) its item set is (lexicographically) smaller, or
// (b) its item set is equal to the other's and its set of children is (lexicographically) smaller.
struct ItemTreePtrComparator { bool operator()(const ItemTreePtr& lhs, const ItemTreePtr& rhs); };

class ItemTree : public DAG<ItemTreeNode, std::set<ItemTreePtr, ItemTreePtrComparator>>
{
public:
	friend ItemTreePtrComparator;
	using DAG::DAG; // inherit Constructor
};
