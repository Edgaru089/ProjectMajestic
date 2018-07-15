#pragma once

#include "RpgLauncherItem.hpp"
#include "RpgEntity.hpp"


////////////////////////////////////////
void RpgLauncherItem::_onShoot() {
	mlog << "OnShoot" << dlog;
	RpgEntity::shoot();
}

