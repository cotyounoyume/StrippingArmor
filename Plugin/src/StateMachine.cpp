#include "StateMachine.h"

namespace StateMachine
{
	void ResetLootStageMap()
	{
		LootStageMap.clear();
	}

	bool IsListed(RE::TESObjectREFR* member)
	{
		if (!LootStageMap.contains(member))
			return false;
		return true;
	}

	void RemoveUnnecessaryMember()
	{
		std::vector<RE::TESObjectREFR*> unlisted;
		for (auto itr = LootStageMap.begin(); itr != LootStageMap.end(); ++itr) {
			if(itr->second == STAGE::kRemove)
				unlisted.push_back(itr->first);
		}
		for (auto member : unlisted) {
			UnList(member);
		}
	}

	void UnList(RE::TESObjectREFR* member)
	{
		LootStageMap.erase(member);
	}

	void ResetStage(RE::TESObjectREFR* member)
	{
		if (!IsListed(member))
			LootStageMap[member] = STAGE::kDefault;
		LootStageMap[member] = GetStageFromDetails(member);
	}

	STAGE GetStage(RE::TESObjectREFR* member, bool fromMap)
	{
		if (fromMap == false || !IsListed(member)) {
			ResetStage(member);
		}
		return LootStageMap[member];
	}

	void SetStage(RE::TESObjectREFR* member, STAGE stage)
	{
		Debug(fmt::format("SetStage: member:{}({}) from:{} to:{}", member->GetFormEditorID(), Utility::num2hex(member->formID), static_cast<int>(GetStage(member)), static_cast<int>(stage)));
		if (stage == STAGE::kLooted) {
			StrippingArmorCommon::AddKeyword(member, "SAStateLooted");
		} else if (stage == STAGE::kCorpsed) {
			StrippingArmorCommon::AddKeyword(member, "SAStateCorpsed");
		}
		LootStageMap[member] = stage;
	}

	STAGE GetStageFromKeyword(RE::TESObjectREFR* member)
	{
		if (StrippingArmorCommon::MCHasKeyword(member, "SAStateCorpsed"))
			return STAGE::kCorpsed;
		if (StrippingArmorCommon::MCHasKeyword(member, "SAStateLooted"))
			return STAGE::kLooted;
		if (StrippingArmorCommon::MCHasKeyword(member, "SACandidateCheckReady"))
			return STAGE::kLootCandidate;
		if (StrippingArmorCommon::MCHasKeyword(member, "kCorpseCandidate"))
			return STAGE::kCorpseCandidate;
		return STAGE::kDefault;
	}

	STAGE GetStageFromArmor(RE::TESObjectREFR* member)
	{
		if (StrippingArmorCommon::HasDummySuits(member, true))
			return STAGE::kCorpsed;
		if (StrippingArmorCommon::HasDummySuits(member, false))
			return STAGE::kLooted;
		return STAGE::kDefault;
	}

	std::vector<RE::TESObjectREFR*> GetTargetBodiesAll()
	{
		std::vector<RE::TESObjectREFR*> list = {};
		for (auto itr = LootStageMap.begin(); itr != LootStageMap.end(); ++itr) {
			list.push_back(itr->first);
		}
		return list;
	}

	std::vector<RE::TESObjectREFR*> GetTargetBodies(STAGE stage)
	{
		std::vector<RE::TESObjectREFR*> list = {};
		for (auto itr = LootStageMap.begin(); itr != LootStageMap.end(); ++itr) {
			if (itr->second == stage)
				list.push_back(itr->first);
		}
		return list;
	}

	STAGE GetStageFromDetails(RE::TESObjectREFR* member)
	{
		auto stage = GetStageFromKeyword(member);
		if (stage == STAGE::kDefault)
			stage = GetStageFromArmor(member);
		if (stage == STAGE::kDefault) {
			if (member->IsDead(true))
				stage = STAGE::kDead;
		}
		return stage;
	}

	bool CheckCandidateCondition(RE::TESObjectREFR* member)
	{
		bool result = false;
		if (member->HasKeyword(StrippingArmorCommon::GetKeyword("SADetailEtc")))
			result = true;
		if (member->HasKeyword(StrippingArmorCommon::GetKeyword("SADetailSleeping")) && Config::GetConditionSleepingOn())
			result = true;
		if (member->HasKeyword(StrippingArmorCommon::GetKeyword("SADetailUnconscious")) && Config::GetConditionUnconsciousOn())
			result = true;
		if (member->HasKeyword(StrippingArmorCommon::GetKeyword("SADetailBleedingOut")) && Config::GetConditionBleedingOutOn())
			result = true;
		if (member->HasKeyword(StrippingArmorCommon::GetKeyword("SADetailCommanded")) && Config::GetConditionIsCommandedOn())
			result = true;
		return result;
	}


	void AddToListForPickpocketTarget(RE::TESObjectREFR* member)
	{
		if (!member)
			return;
		PickpocketMap[member] = true;
	}

	void RemoveFromListForPickpocketTarget(RE::TESObjectREFR* member)
	{
		if (!member)
			return;
		if(PickpocketMap.contains(member))
			PickpocketMap.erase(member);
	}

	bool IsListedInPickpocketTarget(RE::TESObjectREFR* member)
	{
		return PickpocketMap.contains(member);
	}

	std::vector<RE::TESObjectREFR*> GetPickpocketTargetList()
	{
		std::vector<RE::TESObjectREFR*> list;
		for (auto itr = PickpocketMap.begin(); itr != PickpocketMap.end(); ++itr) {
			list.push_back(itr->first);
		}
		return list;
	}

	void ClearListForPickpocketTarget()
	{
		Debug("ClearListForPickpocketTarget");
		PickpocketMap.clear();
	}

	void AddToListForKeytap(RE::TESObjectREFR* member)
	{
		if (!member)
			return;
		if (StrippingArmorCommon::MCHasKeyword(member, "SAIAMSpeaker")) {
			DialogMap[member] = true;
		} else {
			KeytappedMap[member] = true;
		}
	}

	void RemoveFromListForKeytap(RE::TESObjectREFR* member)
	{
		if (!member)
			return;
		if (StrippingArmorCommon::MCHasKeyword(member, "SAIAMSpeaker")) {
			if (DialogMap.contains(member))
				DialogMap.erase(member);
		} else {
			if (KeytappedMap.contains(member))
				KeytappedMap.erase(member);
		}
	}

	bool IsListedInForKeytap(RE::TESObjectREFR* member) 
	{
		if (StrippingArmorCommon::MCHasKeyword(member, "SAIAMSpeaker")) {
			return DialogMap.contains(member);
		} else {
			return KeytappedMap.contains(member);
		}
	}

	void ClearListForForKeytap()
	{
		Debug("ClearListForForKeytap Dialog");
		DialogMap.clear();
		KeytappedMap.clear();
	}

	std::vector<RE::TESObjectREFR*> GetForKeytapList()
	{

		std::vector<RE::TESObjectREFR*> list = {};
		if (Utility::IsMenuOpen("DialogueMenu")) {
			for (auto itr = DialogMap.begin(); itr != DialogMap.end(); ++itr) {
				list.push_back(itr->first);
			}
		} else {
			for (auto itr = KeytappedMap.begin(); itr != KeytappedMap.end(); ++itr) {
				list.push_back(itr->first);
			}
		}
		return list;
	}


}