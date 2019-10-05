/*
 * Copyright 2010-2018 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include "RuleSoldierTransformation.h"

namespace OpenXcom
{

/**
 * Constructor for a soldier transformation project (necromancy, cloning, ascending!)
 * @param name The unique project name id
 */
RuleSoldierTransformation::RuleSoldierTransformation(const std::string &name) :
	_name(name),
	_keepSoldierArmor(false), _createsClone(false), _needsCorpseRecovered(true),
	_allowsDeadSoldiers(false), _allowsLiveSoldiers(false), _allowsWoundedSoldiers(false),
	_listOrder(0), _cost(0), _transferTime(0), _recoveryTime(0), _minRank(0),
	_useRandomStats(false), _lowerBoundAtMinStats(true), _upperBoundAtMaxStats(false), _upperBoundAtStatCaps(false),
	_reset(false)
{
}

/**
 * Loads the transformation project from a YAML file.
 * @param node YAML node.
 * @param listOrder The list weight for this transformation project.
 */
void RuleSoldierTransformation::load(const YAML::Node &node, int listOrder)
{
	if (const YAML::Node &parent = node["refNode"])
	{
		load(parent, listOrder);
	}
	_listOrder = node["listOrder"].as<int>(_listOrder);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}

	_requires = node["requires"].as<std::vector<std::string > >(_requires);
	_requiresBaseFunc = node["requiresBaseFunc"].as<std::vector<std::string > >(_requiresBaseFunc);
	_producedSoldierType = node["producedSoldierType"].as<std::string >(_producedSoldierType);
	_producedSoldierArmor = node["producedSoldierArmor"].as<std::string >(_producedSoldierArmor);
	_keepSoldierArmor = node["keepSoldierArmor"].as<bool >(_keepSoldierArmor);
	_createsClone = node["createsClone"].as<bool >(_createsClone);
	_needsCorpseRecovered = node["needsCorpseRecovered"].as<bool >(_needsCorpseRecovered);
	_allowsDeadSoldiers = node["allowsDeadSoldiers"].as<bool >(_allowsDeadSoldiers);
	_allowsLiveSoldiers = node["allowsLiveSoldiers"].as<bool >(_allowsLiveSoldiers);
	_allowsWoundedSoldiers = node["allowsWoundedSoldiers"].as<bool >(_allowsWoundedSoldiers);
	_allowedSoldierTypes = node["allowedSoldierTypes"].as<std::vector<std::string > >(_allowedSoldierTypes);
	_requiredPreviousTransformations = node["requiredPreviousTransformations"].as<std::vector<std::string > >(_requiredPreviousTransformations);
	_forbiddenPreviousTransformations = node["forbiddenPreviousTransformations"].as<std::vector<std::string > >(_forbiddenPreviousTransformations);
	_requiredMinStats = node["requiredMinStats"].as<UnitStats >(_requiredMinStats);
	_requiredItems = node["requiredItems"].as< std::map<std::string, int> >(_requiredItems);
	_cost = node["cost"].as<int>(_cost);
	_transferTime = node["transferTime"].as<int>(_transferTime);
	_recoveryTime = node["recoveryTime"].as<int>(_recoveryTime);
	_minRank = node["minRank"].as<int>(_minRank);
	_flatOverallStatChange = node["flatOverallStatChange"].as<UnitStats >(_flatOverallStatChange);
	_percentOverallStatChange = node["percentOverallStatChange"].as<UnitStats >(_percentOverallStatChange);
	_percentGainedStatChange = node["percentGainedStatChange"].as<UnitStats >(_percentGainedStatChange);
	_useRandomStats = node["useRandomStats"].as<bool >(_useRandomStats);
	_lowerBoundAtMinStats = node["lowerBoundAtMinStats"].as<bool >(_lowerBoundAtMinStats);
	_upperBoundAtMaxStats = node["upperBoundAtMaxStats"].as<bool >(_upperBoundAtMaxStats);
	_upperBoundAtStatCaps = node["upperBoundAtStatCaps"].as<bool >(_upperBoundAtStatCaps);
	_reset = node["reset"].as<bool >(_reset);
	_soldierBonusType = node["soldierBonusType"].as<std::string >(_soldierBonusType);

	std::sort(_requiresBaseFunc.begin(), _requiresBaseFunc.end());
}

/**
 * Gets the unique name id of the project
 * @return The name of the project
 */
const std::string &RuleSoldierTransformation::getName() const
{
	return _name;
}

/**
 * Gets the list weight of the project
 * @return The list order
 */
int RuleSoldierTransformation::getListOrder() const
{
	return _listOrder;
}

/**
 * Gets the list of research this project requires
 * @return The list of required research
 */
const std::vector<std::string > &RuleSoldierTransformation::getRequiredResearch() const
{
	return _requires;
}

/**
 * Gets the list of required base functions for this project
 * @return The list of required base functions
 */
const std::vector<std::string > &RuleSoldierTransformation::getRequiredBaseFuncs() const
{
	return _requiresBaseFunc;
}

/**
 * Gets the type of soldier produced by this project
 * @return The soldier type
 */
const std::string &RuleSoldierTransformation::getProducedSoldierType() const
{
	return _producedSoldierType;
}

/**
 * Gets the armor that the produced soldier should be wearing
 * @return The armor type
 */
const std::string &RuleSoldierTransformation::getProducedSoldierArmor() const
{
	return _producedSoldierArmor;
}

/**
 * Gets whether or not the project should have the soldier keep their current armor
 * @return Keep the current armor?
 */
bool RuleSoldierTransformation::isKeepingSoldierArmor() const
{
	return _keepSoldierArmor;
}

/**
 * Gets whether or not the project should produce a clone (new id) of the input soldier
 * @return Create a clone?
 */
bool RuleSoldierTransformation::isCreatingClone() const
{
	return _createsClone;
}

/**
 * Gets whether or not the project needs the body of the soldier to have been recovered
 * @return Needs the body?
 */
bool RuleSoldierTransformation::needsCorpseRecovered() const
{
	return _needsCorpseRecovered;
}

/**
 * Gets whether or not the project allows input of dead soldiers
 * @return Allow dead soldiers?
 */
bool RuleSoldierTransformation::isAllowingDeadSoldiers() const
{
	return _allowsDeadSoldiers;
}

/**
 * Gets whether or not the project allows input of alive soldiers
 * @return Allow live soldiers?
 */
bool RuleSoldierTransformation::isAllowingAliveSoldiers() const
{
	return _allowsLiveSoldiers;
}

/**
 * Gets whether or not the project allows input of wounded soldiers
 * @return Allow wounded soldiers?
 */
bool RuleSoldierTransformation::isAllowingWoundedSoldiers() const
{
	return _allowsWoundedSoldiers;
}

/**
 * Gets the list of soldier types eligible for this project
 * @return The list of allowed soldier types
 */
const std::vector<std::string > &RuleSoldierTransformation::getAllowedSoldierTypes() const
{
	return _allowedSoldierTypes;
}

/**
 * Gets the list of previous soldier transformations a soldier needs for this project
 * @return The list of required previous transformations.
 */
const std::vector<std::string > &RuleSoldierTransformation::getRequiredPreviousTransformations() const
{
	return _requiredPreviousTransformations;
}

/**
 * Gets the list of previous soldier transformations that make a soldier ineligible for this project
 * @return The list of forbidden previous 
 */
const std::vector<std::string > &RuleSoldierTransformation::getForbiddenPreviousTransformations() const
{
	return _forbiddenPreviousTransformations;
}

/**
 * Gets the minimum stats a soldier needs to be eligible for this project
 * @return The stat requirements
 */
const UnitStats &RuleSoldierTransformation::getRequiredMinStats() const
{
	return _requiredMinStats;
}

/**
 * Gets the list of items necessary to complete this project
 * @return The list of required items
 */
const std::map<std::string, int> &RuleSoldierTransformation::getRequiredItems() const
{
	return _requiredItems;
}

/**
 * Gets the cash cost of the project
 * @return The cost
 */
int RuleSoldierTransformation::getCost() const
{
	return _cost;
}

/**
 * Gets how long the transformed soldier should be in transit to the base after completion
 * @return The transfer time in hours
 */
int RuleSoldierTransformation::getTransferTime() const
{
	return _transferTime;
}

/**
 * Gets how long the transformed soldier should take to recover after completion
 * @return The recovery time in days
 */
int RuleSoldierTransformation::getRecoveryTime() const
{
	return _recoveryTime;
}

/**
 * Gets the minimum rank a soldier needs to be eligible for this project
 * @return The rank requirement
 */
int RuleSoldierTransformation::getMinRank() const
{
	return _minRank;
}

/**
 * Gets the flat change to a soldier's overall stats when undergoing this project
 * @return The flat overall stat changes
 */
const UnitStats &RuleSoldierTransformation::getFlatOverallStatChange() const
{
	return _flatOverallStatChange;
}

/**
 * Gets the percent change to a soldier's overall stats when undergoing this project
 * @return The percent overall stat changes
 */
const UnitStats &RuleSoldierTransformation::getPercentOverallStatChange() const
{
	return _percentOverallStatChange;
}

/**
 * Gets the percent change to a soldier's gained stats when undergoing this project
 * @return The percent gained stat changes
 */
const UnitStats &RuleSoldierTransformation::getPercentGainedStatChange() const
{
	return _percentGainedStatChange;
}

/**
 * Gets whether or not this project should use randomized stats from the produced RuleSoldier or the input soldier's stats
 * @return Use random stat generation?
 */
bool RuleSoldierTransformation::isUsingRandomStats() const
{
	return _useRandomStats;
}

/**
 * Gets whether or not this project should bound stat penalties at the produced RuleSoldier's minStats
 * @return Lower bound at minStats?
 */
bool RuleSoldierTransformation::hasLowerBoundAtMinStats() const
{
	return _lowerBoundAtMinStats;
}

/**
 * Gets whether or not this project should cap stats at the produced RuleSoldier's maxStats
 * @return Upper bound at maxStats?
 */
bool RuleSoldierTransformation::hasUpperBoundAtMaxStats() const
{
	return _upperBoundAtMaxStats;
}

/**
 * Gets whether or not this project should cap stats at the produced RuleSoldier's statCaps
 * @return Upper bound at statCaps?
 */
bool RuleSoldierTransformation::hasUpperBoundAtStatCaps() const
{
	return _upperBoundAtStatCaps;
}

/**
 * Gets whether or not this project should reset info about all previous transformations and all previously assigned soldier bonuses
 * @return Reset previous transformation info and soldier bonuses?
 */
bool RuleSoldierTransformation::getReset() const
{
	return _reset;
}

/**
 * Gets the type of soldier bonus assigned by this project
 * @return The soldier bonus type
 */
const std::string &RuleSoldierTransformation::getSoldierBonusType() const
{
	return _soldierBonusType;
}

}
