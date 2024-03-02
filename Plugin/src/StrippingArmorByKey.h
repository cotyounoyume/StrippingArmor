#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"
#include "StateMachine.h"
#include "StrippingArmorCommon.h"

namespace StrippingArmorByKey
{
	void State_Dialogue();
	void State_StrippingByKey();

	void SearchTargetsByKey(bool bForced);
	void CallAndCheckDialogTarget();
	void AddKeywordMonitoring(bool inDialog = false);
	void ProcessByKey(bool inDialog);

	void StealEquipArmors(RE::TESObjectREFR * member);
	void AddKeywordForCandidates(RE::TESObjectREFR * member, bool isForcedByDebugging = false);
	void AddKeywordForDialogTarget(RE::TESObjectREFR * member);
	void RemoveDialogTargetKeywords(RE::TESObjectREFR * member);

	void ToggleArmorsByKey();
	void State_ToggleByKey();
	void DropEquipItems(RE::TESObjectREFR * member);
	void HouseKeepByKey();

	inline std::unordered_map<RE::TESObjectREFR*, bool> AddKeywordWaitingList;
	inline RE::TESObjectREFR*                           DialogTarget;
}