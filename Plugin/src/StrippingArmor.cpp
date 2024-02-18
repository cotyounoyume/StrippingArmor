#include "StrippingArmor.h"

namespace StrippingArmor
{
	void MainLoop()
	{
		Config::ReadIni();

		while (true) {
			Sleep(Config::GetTimePerFrame());
			if (!Utility::InGameScene())
				continue;

			if (atOnce) {
				Debug("atOnce Start");
				atOnce = false;
				ResetParameter();
				Config::EsmNotLoadCheck();
				Debug("atOnce finish");
			}

			if (Events::NeedReset) {
				ResetParameter();
				continue;
			}

			StateSelector();
		}
	}

	void ResetParameter()
	{
		Debug(fmt::format("ResetParameter"));
		Events::NeedReset = false;
		StrippingArmorCommon::WaitCount = 0;
		
		StrippingArmorLoot::ResetParameter();
		StateMachine::ClearListForPickpocketTarget();
		StateMachine::ClearListForForKeytap();
	}

	void StateSelector()
	{
		if (Utility::IsMenuOpen("DialogueMenu")) {
			StrippingArmorByKey::State_Dialogue();
		} else if (Utility::IsMenuOpen("PickpocketMenu")) {
			StrippingArmorPickpocket::State_Pickpocket();
		} else {
			StrippingArmorLoot::State_Loot();
			StrippingArmorByKey::State_StrippingByKey();
			StrippingArmorByKey::State_ToggleByKey();
		}

		if (!Utility::IsMenuOpen("PickpocketMenu") && !StateMachine::PickpocketMap.empty()) 
			StrippingArmorPickpocket::HouseKeepPickpocket();

		if (!Utility::IsMenuOpen("DialogueMenu")) 
			StrippingArmorByKey::HouseKeepByKey();
	}
}
