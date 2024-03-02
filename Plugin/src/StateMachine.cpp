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
			if (itr->second == STAGE::kRemove)
				unlisted.push_back(itr->first);
		}
		for (auto member : unlisted) {
			UnList(member);
		}
	}

	void UnList(RE::TESObjectREFR* member)
	{
		if(LootStageMap.contains(member))
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

	void SetStage(RE::TESObjectREFR* member, STAGE stage, std::string debugMsg)
	{
		if (Utility::IsWrongForm(member, "SetStage"))
			return;
		Debug(fmt::format("SetStage: member:{}({}) from:{} to:{}: debugMsg:{}", member->GetFormEditorID(), Utility::num2hex(member->formID), static_cast<int>(GetStage(member)), static_cast<int>(stage), debugMsg));
		if (stage == STAGE::kLooted) {
			StrippingArmorCommon::AddKeyword(member, "SAStateLooted");
		} else if (stage == STAGE::kCorpsed) {
			StrippingArmorCommon::AddKeyword(member, "SAStateCorpsed");
		}
		LootStageMap[member] = stage;
		Debug(fmt::format("SetStage: Done. member:{}({}) from:{} to:{}", member->GetFormEditorID(), Utility::num2hex(member->formID), static_cast<int>(GetStage(member)), static_cast<int>(stage)));
	}

	STAGE GetStageFromKeyword(RE::TESObjectREFR* member)
	{
		if (Utility::IsWrongForm(member, "GetStageFromKeyword"))
			return STAGE::kDefault;
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
		if (Utility::IsWrongForm(member, "GetStageFromArmor"))
			return STAGE::kDefault;
		if (StrippingArmorCommon::HasDummySuits(member, true))
			return STAGE::kCorpsed;
		if (StrippingArmorCommon::HasDummySuits(member, false))
			return STAGE::kLooted;
		return STAGE::kDefault;
	}

	std::vector<RE::TESObjectREFR*> GetTargetBodiesAll()
	{
		std::vector<RE::TESObjectREFR*> list = {};
		std::vector<RE::TESObjectREFR*> delete_list = {};
		for (auto itr = LootStageMap.begin(); itr != LootStageMap.end(); ++itr) {
			auto member = itr->first;
			if (Utility::IsWrongForm(member, "GetTargetBodiesAll")) {
				delete_list.push_back(member);
				continue;
			}
			list.push_back(itr->first);
		}
		for (auto obj : delete_list) {
			UnList(obj);
		}

		return list;
	}

	std::vector<RE::TESObjectREFR*> GetTargetBodies(STAGE stage)
	{
		std::vector<RE::TESObjectREFR*> list = {};
		for (auto itr = LootStageMap.begin(); itr != LootStageMap.end(); ++itr) {
			if (Utility::IsWrongForm(itr->first, "GetTargetBodies"))
				continue;
			if (itr->second == stage)
				list.push_back(itr->first);
		}
		return list;
	}

	STAGE GetStageFromDetails(RE::TESObjectREFR* member)
	{
		if (Utility::IsWrongForm(member, "GetStageFromDetails"))
			return STAGE::kDefault;
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
		if (Utility::IsWrongForm(member, "CheckCandidateCondition"))
			return false;
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
		if (Utility::IsWrongForm(member, "AddToListForPickpocketTarget"))
			return;
		PickpocketMap[member] = true;
	}

	void RemoveFromListForPickpocketTarget(RE::TESObjectREFR* member)
	{
		if (!member)
			return;
		if (PickpocketMap.contains(member))
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

	void AddToListForKeytap(RE::TESObjectREFR* member, bool inDialog)
	{
		if (!member)
			return;
		if (inDialog) {
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

	bool IsListedInForKeytap(RE::TESObjectREFR* member, bool inDialog)
	{
		if (inDialog) {
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

	std::vector<RE::TESObjectREFR*> GetForKeytapList(bool inDialog)
	{
		std::vector<RE::TESObjectREFR*> list = {};
		if (inDialog) {
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

	void ForDebugGetMember(bool inDialog)
	{
		std::vector<RE::TESObjectREFR*> list = {};
		if (inDialog) {
			for (auto itr = DialogMap.begin(); itr != DialogMap.end(); ++itr) {
				list.push_back(itr->first);
			}
		} else {
			for (auto itr = KeytappedMap.begin(); itr != KeytappedMap.end(); ++itr) {
				list.push_back(itr->first);
			}
		}

		std::string msg = inDialog ? "DialogMap:" : "KeytappedMap:";
		Debug(fmt::format("{}", msg));
		for (auto member : list) {
			Debug(fmt::format("\t{}({})", member->GetFormEditorID(), Utility::num2hex(member->formID)));
		}
	}
}