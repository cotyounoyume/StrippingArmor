#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"
#include "StateMachine.h"
#include "StrippingArmorCommon.h"
#include "StrippingArmorLoot.h"
#include "StrippingArmorPickpocket.h"
#include "StrippingArmorByKey.h"

namespace StrippingArmor
{
	void                                                        MainLoop();
	void                                                        ResetParameter();
	void                                                        StateSelector();

	inline bool                                                 atOnce = true;
	inline bool                                                 NeedReset = false;
	inline std::unordered_map<RE::TESObjectREFR*, bool>         Candidates;
}