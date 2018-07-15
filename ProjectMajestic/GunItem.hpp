#pragma once

#include "Item.hpp"

// 600 RPM
const Time minigunShootInterval = milliseconds(100);
const Time minigunReloadTime = seconds(3.5f);
const int minigunRoundsPerDrum = 150;

class GunItem :public Item {
public:
	GunItem(Dataset& data) :Item(data) {}

	virtual const string getItemId() = 0;

	virtual Time shootInterval() = 0;
	virtual double bulletDamage() = 0;
	virtual Time reloadTime() = 0;
	virtual int roundsPerMagazine() = 0;
	virtual string magazineItemName() = 0; // "item_xxx"

	int getMaxItemsPerSlotCount() override { return 1; }

	void updateLogic() override;

	bool _onRightPressed() override;
	void _onRightReleased() override;

	void _pushExtraImguiItemsToDashboard() override;

private:

	int& lastShootTimeMill() { return slotDataset["last_shoot_time"].getDataInt(); }
	bool isShooting() { /*return slotDataset["is_shooting"].getDataBool();*/ return lastShootTimeMill() != 0; };
	int& roundsLeft() { return slotDataset["rounds_left"].getDataInt(); }
	int& startReloadTimeMill() { return slotDataset["load_time"].getDataInt(); }
	bool isReloading() { return startReloadTimeMill() != 0; }

};

