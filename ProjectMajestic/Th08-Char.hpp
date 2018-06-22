#pragma once

#include "PlayerCharacter.hpp"

// 在 TH08 东方永夜抄 中登场的人物
namespace Th08 {

	// 鈴仙　優曇華院　イナバ
	class ReisenUdongeinInaba :public PlayerCharacter {
	public:
		ReisenUdongeinInaba() :PlayerCharacter() {
			pref.Element = 0.7;
			pref.PhantomYoukai = 0.5;
			pref.ElementMoon = 0.7;
			pref.PhantomYoukai = 0.5;
			majors.push_back(ElementMoon);
			majors.push_back(PhantomYoukai);
		}
	};

	// 八意　永琳
	class YagokoroEirin :public PlayerCharacter {
	public:
		YagokoroEirin() :PlayerCharacter() {
			pref.Element = 1.0;  // 这里主要是因为八意思兼的原型
			pref.Phantom = 1.0;  // 和要和辉夜区分而加上了神灵属性
			pref.ElementMoon = 1.0;
			pref.Phantom = 1.0;
			majors.push_back(PureElementMoon);
			majors.push_back(PhantomDivine);
		}
	};

	// 蓬莱山　輝夜
	class HouraisanKaguya :public PlayerCharacter {
	public:
		HouraisanKaguya() :PlayerCharacter() {
			pref.Element = 1.0;      // 辉夜的符卡永琳全都能用，反过来就不行
			pref.ElementMoon = 1.0;
			majors.push_back(PureElementMoon);
		}
	};

}
