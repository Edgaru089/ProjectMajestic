#pragma once

#include "GunItem.hpp"
#include "BulletEntity.hpp"


////////////////////////////////////////
void GunItem::updateLogic() {
	if (isShooting()) {
		int curtime = programRunTimeClock.getElapsedTime().asMilliseconds();
		if (curtime - lastShootTimeMill() > shootInterval().asMilliseconds()) {
			lastShootTimeMill() = curtime;
			if (roundsLeft() > 0) {
				BulletEntity::shoot(bulletDamage());
				roundsLeft()--;
			}
		}
	}
	if (!isReloading()) {
		if (logicIO.keyboardState[Keyboard::R] == LogicIO::JustPressed) {
			bool ok = false;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 9; j++) {
					auto& slot = playerInventory.slots[i][j];
					if (slot["item_name"].getDataString() == magazineItemName()) {
						slot["count"].getDataInt()--;
						if (slot["count"].getDataInt() <= 0)
							slot["item_name"].getDataString() = "";
						ok = true;
						break;
					}
				}
			if (ok) {
				roundsLeft() = 0;
				startReloadTimeMill() = programRunTimeClock.getElapsedTime().asMilliseconds();
			}
		}
	}
	else {
		if (programRunTimeClock.getElapsedTime().asMilliseconds() - startReloadTimeMill() >= reloadTime().asMilliseconds()) {
			startReloadTimeMill() = 0;
			roundsLeft() = roundsPerMagazine();
		}
	}
}


////////////////////////////////////////
void GunItem::_pushExtraImguiItemsToDashboard() {
	if (isReloading()) {
		imgui::Text("Reloading %d%%",
			(programRunTimeClock.getElapsedTime().asMilliseconds() - startReloadTimeMill()) * 100 / reloadTime().asMilliseconds());
		imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(240, 240, 240));
		imgui::ProgressBar((float)(programRunTimeClock.getElapsedTime().asMilliseconds() - startReloadTimeMill()) / reloadTime().asMilliseconds(),
						   ImVec2(-1, 15));
		imgui::PopStyleColor();
	}
	else {
		imgui::Text("Ammo %d / %d", roundsLeft(), roundsPerMagazine());
		imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(240 - 240 * (float)roundsLeft() / roundsPerMagazine(),
															0,
															240 * (float)roundsLeft() / roundsPerMagazine()));
		imgui::ProgressBar((float)roundsLeft() / roundsPerMagazine(), ImVec2(-1, 15));
		imgui::PopStyleColor();
	}
}


////////////////////////////////////////
bool GunItem::_onRightPressed() {
	lastShootTimeMill() = programRunTimeClock.getElapsedTime().asMilliseconds();

	return true;
}


////////////////////////////////////////
void GunItem::_onRightReleased() {
	lastShootTimeMill() = 0;
}

