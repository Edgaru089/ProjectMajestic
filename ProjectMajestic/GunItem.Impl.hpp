#pragma once

#include "GunItem.hpp"
#include "BulletEntity.hpp"
#include "TextSystem.hpp"


////////////////////////////////////////
void GunItem::_onShoot() {
	BulletEntity::shoot(bulletDamage(), bulletSpeed(), bulletKnockbackFactor(), localPlayer->getEyePosition(),
						gameIO.degreeAngle + (rand01() - 0.5)*bulletUnaccuracyDegree());
}


////////////////////////////////////////
void GunItem::updateLogic() {
	if (isShooting()) {
		int curtime = programRunTimeClock.getElapsedTime().asMilliseconds();
		if (curtime - lastShootTimeMill() > shootInterval().asMilliseconds()) {
			if (roundsLeft() > 0) {
				lastShootTimeMill() = curtime;
				roundsLeft()--;
				_onShoot();
			}
		}
	}
	if (!isReloading() && isInHand()) {
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
	if (isReloading()) {
		if (programRunTimeClock.getElapsedTime().asMilliseconds() - startReloadTimeMill() >= reloadTime().asMilliseconds()) {
			startReloadTimeMill() = 0;
			roundsLeft() = roundsPerMagazine();
		}
	}
}


////////////////////////////////////////
void GunItem::_pushExtraImguiItemsToDashboard() {
	if (isReloading()) {
		imgui::Text(text.get("gunhud.reloading"),
			(programRunTimeClock.getElapsedTime().asMilliseconds() - startReloadTimeMill()) * 100 / reloadTime().asMilliseconds());
		imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(240, 240, 240));
		imgui::ProgressBar((float)(programRunTimeClock.getElapsedTime().asMilliseconds() - startReloadTimeMill()) / reloadTime().asMilliseconds(),
						   ImVec2(-1, 12), "");
		imgui::PopStyleColor();
	}
	else {
		imgui::Text(text.get("gunhud.ammo"), roundsLeft(), roundsPerMagazine());
		imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(240 - 240 * (float)roundsLeft() / roundsPerMagazine(),
															0,
															240 * (float)roundsLeft() / roundsPerMagazine()));
		imgui::ProgressBar((float)roundsLeft() / roundsPerMagazine(), ImVec2(-1, 12), "");
		imgui::PopStyleColor();
	}
	//if (shootInterval() > milliseconds(800)) {  // Long shoot interval - bolt action
		// Display load progress bar
	imgui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, -1));
	imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(192, 192, 192));
	imgui::ProgressBar(min(1.0f, (float)(programRunTimeClock.getElapsedTime().asMilliseconds() -
										 lastShootTimeMill()) /
						   shootInterval().asMilliseconds()), ImVec2(-1, 4), "");
	imgui::PopStyleColor();
	imgui::PopStyleVar();
//}
}


////////////////////////////////////////
bool GunItem::_onRightPressed() {
	isShooting() = true;

	return true;
}


////////////////////////////////////////
void GunItem::_onRightReleased() {
	isShooting() = false;
}

