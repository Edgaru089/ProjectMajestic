#pragma once

#include "PlayerCharacter.hpp"

// 主角组
namespace Major {

	// 灵梦
	class HakureiReimu :public PlayerCharacter {
	public:
		HakureiReimu() :PlayerCharacter() {
			pref.Phantom = 1.0;       // 纯人灵
			pref.PhantomHuman = 1.0;
			majors.push_back(PurePhantomHuman);
		}
	};

	// 魔理沙
	class KirisameMarisa :public PlayerCharacter {
	public:
		KirisameMarisa() :PlayerCharacter() {
			pref.Element = 0.8;
			pref.Phantom = 0.6;
			pref.ElementFire = 0.8;
			pref.PhantomHuman = 0.6;
			majors.push_back(ElementFire);
			majors.push_back(PhantomHuman);
		}
	};

}
