#pragma once

#include "PlayerCharacter.hpp"

// 在 TH09 东方花映塚 中登场的人物
namespace Th09 {

	// 風見 幽香
	class KazamiYuuka :public PlayerCharacter {
	public:
		KazamiYuuka() :PlayerCharacter() {
			pref.Element = 1.0;       // 幻想乡最强之一？
			pref.Phantom = 1.0;
			pref.ElementWood = 1.0;   // 玩弄花草
			pref.PhantomYoukai = 1.0;
			majors.push_back(ElementWood);
			majors.push_back(PhantomYoukai);
		}
	};

}
