#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"
#include "StateMachine.h"
#include "StrippingArmorCommon.h"

namespace StrippingArmorLoot
{

	void ResetParameter();

	void State_Loot();
	void SearchBodies();
	void SelectStatusForLoot(RE::TESObjectREFR* member);
	void MonitoringBodiesForLoot();
	void ProcessForStageDead(RE::TESObjectREFR* member);
	void ProcessForStageLootCandidate(RE::TESObjectREFR* member);
	void ProcessForStageLootWaiting(RE::TESObjectREFR* member);
	void ProcessForStageLooted(RE::TESObjectREFR* member);
	void ProcessForStageCorpseCandidate(RE::TESObjectREFR* member);
	void ProcessForStageCorpsed(RE::TESObjectREFR* member);
	void ProcessForStageCorpsedAndNaked(RE::TESObjectREFR* member);

	void AddArmorsForLoot(RE::TESObjectREFR* actor);
	bool RemoveArmorIfLooted(RE::TESObjectREFR* member);
	void AddKeywordForLoot(RE::TESObjectREFR* member);
	void AddKeywordForCorpses(RE::TESObjectREFR* member);

	void ChangingCorpse(RE::TESObjectREFR* member, int HighOrLow);
	void ChangingCorpseByPapyrus(RE::TESObjectREFR* targetActor, int HighOrLow);
	bool IsReadyForCorpse(RE::TESObjectREFR* member);
	int  GetEquipmentStackCount(const RE::BGSInventoryItem& item);
	std::vector<RE::TESObjectARMO*> GetLootedArmors(RE::TESObjectREFR* actor);
	RE::TESObjectARMO*              GetCorpsesuit(RE::TESObjectREFR* member);

	int  GetHighOrLow(RE::TESObjectREFR* member);
	void RemoveCorpseKeywords(RE::TESObjectREFR* member);


}