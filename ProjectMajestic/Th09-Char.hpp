#pragma once

#include "PlayerCharacter.hpp"

// �� TH09 ������ӳ�V �еǳ�������
namespace Th09 {

	// �LҊ ����
	class KazamiYuuka :public PlayerCharacter {
	public:
		KazamiYuuka() :PlayerCharacter() {
			pref.Element = 1.0;       // ��������ǿ֮һ��
			pref.Phantom = 1.0;
			pref.ElementWood = 1.0;   // ��Ū����
			pref.PhantomYoukai = 1.0;
			majors.push_back(ElementWood);
			majors.push_back(PhantomYoukai);
		}
	};

}
