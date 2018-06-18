#pragma once

#include "BowItem.hpp"


////////////////////////////////////////
bool BowItem::_onRightPressed() {
	slotDataset["bow_start_time"].setData(programRunTimeClock.getElapsedTime().asMilliseconds());
	return true;
}


////////////////////////////////////////
void BowItem::_onRightReleased() {
	double stage = min(1.0, 0.2 +
		(double)(programRunTimeClock.getElapsedTime().asMilliseconds() - slotDataset["bow_start_time"].getDataInt()) / 1200.0 * 0.8);
	slotDataset["bow_start_time"].setData(0);
	// TODO Check inventory and arrows before shooting
	// TODO Force and damage change
	ArrowEntity::shoot(stage*maxArrowDamage);
}
