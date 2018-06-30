#pragma once

#include "PlayerCharacter.hpp"

// �� TH08 ������ҹ�� �еǳ�������
namespace Th08 {

	// ��ɡ������AԺ�����ʥ�
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

	// ���⡡����
	class YagokoroEirin :public PlayerCharacter {
	public:
		YagokoroEirin() :PlayerCharacter() {
			pref.Element = 1.0;  // ������Ҫ����Ϊ����˼���ԭ��
			pref.Phantom = 1.0;  // ��Ҫ�ͻ�ҹ���ֶ���������������
			pref.ElementMoon = 1.0;
			pref.Phantom = 1.0;
			majors.push_back(PureElementMoon);
			majors.push_back(PhantomDivine);
		}
	};

	// ����ɽ���xҹ
	class HouraisanKaguya :public PlayerCharacter {
	public:
		HouraisanKaguya() :PlayerCharacter() {
			pref.Element = 1.0;      // ��ҹ�ķ�������ȫ�����ã��������Ͳ���
			pref.ElementMoon = 1.0;
			majors.push_back(PureElementMoon);
		}
	};

}
