#pragma once

#include "PlayerCharacter.hpp"

// 在 TH07 东方妖妖梦 中登场的人物
namespace Th07 {

	// 爱丽丝 
	class AliceMargatroid :public PlayerCharacter {
	public:
		AliceMargatroid() :PlayerCharacter() {
			pref.Element = 0.7;       // 七曜属性比帕琪都低一些（爱丽丝的符卡帕琪随便用？？双七组预定？？个人不站双七啊）
			pref.Phantom = 0.4;       // 不不不不妖灵属性高一些
			pref.ElementMetal = 0.7;
			pref.ElementWood = 0.7;
			pref.ElementWater = 0.7;
			pref.ElementFire = 0.65;  // 确保阿空的符卡（火0.8，日1.0）不能用
			pref.ElementSoil = 0.7;
			pref.ElementSun = 0.6;    // 确保阿空的符卡（火0.8，日1.0）不能用
			pref.ElementMoon = 0.6;   // 月都角色月属性普遍高于0.7
			pref.PhantomYoukai = 0.4; // 毕竟是魔法使

			majors.push_back(SevenElements); // 七色的人偶使
			majors.push_back(PhantomYoukai); // 魔法使是妖怪
		}
	};

	// 妖梦
	class KonpakuYoumu :public PlayerCharacter {
	public:
		KonpakuYoumu() :PlayerCharacter() {
			pref.Element = 0.5;
			pref.Phantom = 0.6;
			pref.ElementMetal = 0.5;  // 白楼剑楼观剑->金属性？
			pref.PhantomHuman = 0.4;  // 半人半灵
			pref.PhantomYoukai = 0.4;
			majors.push_back(ElementMetal);
			majors.push_back(PhantomHuman);
			majors.push_back(PhantomYoukai);
		}
	};

	// 幽幽子
	class SaigyoujiYuyuko :public PlayerCharacter {
	public:
		SaigyoujiYuyuko() :PlayerCharacter() {
			pref.Phantom = 1.0;
			pref.PhantomYoukai = 1.0;
			majors.push_back(PurePhantomYoukai);
		}
	};

	// 八云紫
	class YakumoYukari :public PlayerCharacter {
	public:
		YakumoYukari() :PlayerCharacter() {
			pref.Phantom = 1.0;
			pref.PhantomYoukai = 1.0;
			majors.push_back(PurePhantomYoukai);
		}
	};
}
