#include "StrippingArmorCommon.h"

namespace StrippingArmorCommon
{
	bool MCHasKeyword(RE::TESObjectREFR* object, std::string keywordString)
	{
		auto keyword = GetKeyword(keywordString);
		if (!keyword)
			return false;
		return object->HasKeyword(keyword);
	}

	bool IsDummySuits(RE::TESObjectARMO* armor, bool forCorpse)
	{
		if (!armor)
			return false;
		if (forCorpse)
			return armor->ContainsKeywordString("SACorpseFrozen") || armor->ContainsKeywordString("SACorpseDusty");
		return armor->ContainsKeywordString("SADontStripThis");
	}

	bool HasDummySuits(RE::TESObjectREFR* member, bool forCorpse)
	{
		auto armors = Utility::CollectEquipArmors(member);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (IsDummySuits(item, forCorpse))
				return true;
		}
		return false;
	}

	bool IsAltSuits(RE::TESObjectARMO* armor)
	{
		if (!armor)
			return false;
		return armor->ContainsKeywordString("SAThisIsForToggle");
	}

	bool HasAltSuits(RE::TESObjectREFR* member)
	{
		auto armors = Utility::CollectEquipArmors(member);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (IsAltSuits(item))
				return true;
		}
		return false;
	}

	RE::TESObjectARMO* GetArmor(std::string editorID)
	{
		return Utility::GetArmorFromString(editorID);
	}

	RE::BGSKeyword* GetKeyword(std::string editorID)
	{
		if (!KeywordMap.contains(editorID))
			return nullptr;
		return KeywordMap[editorID];
	}

	void MakeKeywordMapIfNeeded()
	{
		Info(format("MakeKeywordMapIfNeeded: start: KeywordMap:{}", KeywordMap.size()));

		if (!KeywordMap.size() == 0)
			return;
		Info(format("MakeKeywordMapIfNeeded: process: list:{}", KeywordList.size()));
		for (std::string item : KeywordList) {
			auto keyword = Utility::GetKeywordFromString(item);
			if (keyword) {
				KeywordMap[item] = keyword;
			}
		}
		Info("MakeKeywordMapIfNeeded: process end:");
	}

	void SetPlayerIfNeeded()
	{
		if (Player != nullptr) {
			if (Player->formID != 0x14) {
				Debug("Unknown Error: reset StrippingArmorCommon:Player's formID");
				Debug(fmt::format("before: id:{}", Utility::num2hex(Player->formID))); 
				Player = Utility::GetPlayer();
				PlayerActor = Utility::GetPlayerActor();
				Debug(fmt::format("after: id:{}", Utility::num2hex(Player->formID))); 
			}
			return;
		}
		Player = Utility::GetPlayer();
		PlayerActor = Utility::GetPlayerActor();
	}

	RE::TESObjectCELL* GetCurrentCell()
	{
		SetPlayerIfNeeded();
		if (Player == nullptr) {
			Debug(format("ERROR: can't get player ref"));
			return nullptr;
		}

		auto cell = Player->parentCell;
		if (cell == nullptr)
			Debug(format("ERROR: can't get parentCell"));
		return cell;
	}

	std::unordered_map<RE::TESObjectREFR*, bool> CollectRefsInCell()
	{
		std::unordered_map<RE::TESObjectREFR*, bool> result;
		auto cell = GetCurrentCell();
		if (!cell)
			return result;
		bool  DebugOn = Config::GetDebugExecuteToAllActorsOn();
		float radius = DebugOn ? float(Config::GetCellSearchRadiusDebug()) : float(Config::GetCellSearchRadius());
		return Utility::CollectRefsInCellInRange(cell, StrippingArmorCommon::Player, radius);
	}


	bool IsMemorizedArmor(RE::TESObjectREFR* member)
	{
		return ArmorClothCombinationMap.contains(member);
	}

	void MemorizeArmorType(RE::TESObjectREFR* member)
	{
		if (!ArmorClothCombinationMap.contains(member))
			ArmorClothCombinationMap[member] = GetArmorClothCombination(member);
	}

	void UnmemorizeArmorType(RE::TESObjectREFR* member)
	{
		ArmorClothCombinationMap.erase(member);
	}

	int GetArmorClothCombination(RE::TESObjectREFR* member)
	{
		if (ArmorClothCombinationMap.contains(member))
			return ArmorClothCombinationMap[member];
		bool clothOn = false;
		bool suitOn = false;
		auto armors = Utility::CollectEquipArmorsWithoutCount(member);
		for (auto armor : armors) {
			if (Utility::GetArmorType(armor) == "Cloth")
				clothOn = true;
			else if (Utility::GetArmorType(armor) == "Spacesuit")
				suitOn = true;
		}

		int result = (clothOn) ? 2 : 0;
		result += (suitOn) ? 1 : 0;
		Info(format("GetArmorClothCombination: result:{}, clothOn:{}, suitOn:{}", result, clothOn, suitOn));
		return result;
	}

	void AddKeyword(RE::TESObjectREFR* member, std::string keyword)
	{
		SubKeyword(member, keyword, "SAAddKeywordFromSFSE");
	}

	void RemoveKeyword(RE::TESObjectREFR* member, std::string keyword)
	{
		SubKeyword(member, keyword, "SARemoveKeywordFromSFSE");
	}

	void SubKeyword(RE::TESObjectREFR* member, std::string keyword, std::string scriptname)
	{
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" \"{}\" \"{}\"",
			scriptname, member->formID, keyword));
	}

	bool ShouldShowSpaceSuit(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		if (StrippingArmorCommon::GetArmorClothCombination(member) == 1) {
			return true;
		}

		bool isReadyCandidate = member->HasKeyword(StrippingArmorCommon::GetKeyword("SACandidateCheckReady"));
		bool isReadyCorpse = member->HasKeyword(StrippingArmorCommon::GetKeyword("SACorpseCheckReady"));
		bool ShouldShowSpaceSuitOK = member->HasKeyword(StrippingArmorCommon::GetKeyword("SAConditionShouldShowSuit"));
		Debug(fmt::format("ShouldShowSpaceSuit: {}({}): isReadyCandidate={}, isReadyCorpse={}, ShouldShowSpaceSuitOK={}",
			member->GetFormEditorID(), Utility::num2hex(member->formID), isReadyCandidate, isReadyCorpse, ShouldShowSpaceSuitOK));
		if (isReadyCandidate || isReadyCorpse) {
			if (ShouldShowSpaceSuitOK)
				return true;
		}
		return false;
	}

	bool NeedDummysuit(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		if (ShouldShowSpacesuitMap[member]) {
			return true;
		} else {
			return false;
		}
	}

	void EquipDummysuit(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return;
		Info(format("EquipDummysuit: start: {}", member->GetFormEditorID()));

		std::string editorID = "";
		std::string editorIDsub = "";
		if (!NeedDummysuit(member)) {
			if (Config::GetAlternativeClothEnabled())
				editorID = "Dummy_NormalAlt_Clothes";
			else
				editorID = "Dummy_Normal_Clothes";
		} else if (NeedDummysuit(member)) {
			if (Config::GetAlternativeClothEnabled()) {
				editorID = "Dummy_NormalAlt_Spacesuit";
				editorIDsub = "Dummy_NormalAlt_Clothes";
			} else {
				editorID = "Dummy_Normal_Spacesuit";
				editorIDsub = "Dummy_Normal_Clothes";
			}
		}

		auto armor = StrippingArmorCommon::GetArmor(editorID);
		auto armorsub = armor;
		if (editorIDsub == "")
			armorsub = nullptr;
		else
			armorsub = StrippingArmorCommon::GetArmor(editorIDsub);
		if (armorsub)
			Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("EquipItem {}", Utility::num2hex(armorsub->formID)));
		Sleep(1);
		Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("EquipItem {}", Utility::num2hex(armor->formID)));

		Info(format("EquipDummysuit: finish: {}", member->GetFormEditorID()));
	}

	void EquipToggleSuit(RE::TESObjectREFR* member, bool isSuit)
	{
		if (member == nullptr)
			return;
		Info(format("EquipToggleSuit: start: {}", member->GetFormEditorID()));

		auto editorID = isSuit ? "Dummy_Toggle_Spacesuit" : "Dummy_Toggle_Clothes";
		auto armor = StrippingArmorCommon::GetArmor(editorID);
		Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("EquipItem {}", Utility::num2hex(armor->formID)));

		Info(format("EquipToggleSuit: finish: {}", member->GetFormEditorID()));
	}

	void DoEffectShader(RE::TESObjectREFR* member)
	{
		if (!Config::GetEffectEnabled())
			return;
		float timer = 0.5;
		//Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("pms {} 1", Config::GetEffectFormID()));
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.PlayEffect\" \"{}\" \"{}\" \"{}\"", member->formID, Config::GetEffectFormID(), timer));
	}

	void ToggleBodyRoute(RE::TESObjectREFR* member)
	{
		if (StrippingArmorCommon::HasAltSuits(member)) {
			Info(format("ToggleBodyRoute: {} has ToggleKeyword", member->GetFormEditorID()));
			bool isSuit = IsSuit(member);
			auto keywords = GetKeywordMapFromActor(member);
			auto armor = GetArmorFromKeywordMap(keywords, isSuit);
			Utility::EquipItem(member, armor->formID);
			for (auto itr = keywords.begin(); itr != keywords.end(); ++itr) {
				Debug(fmt::format("Debug:{} RemoveKeyword:{}", member->GetFormEditorID(), itr->first));
				StrippingArmorCommon::RemoveKeyword(member, itr->first);
			}
		} else if (StrippingArmorCommon::HasDummySuits(member)) {
			Info(format("ToggleBodyRoute: {} has DummySuit", member->GetFormEditorID()));
			bool isSuit = IsSuit(member);
			auto keywords = GetKeywordMapFromArmor(member);
			StrippingArmorCommon::EquipToggleSuit(member, isSuit);
			for (auto itr = keywords.begin(); itr != keywords.end(); ++itr) {
				Debug(fmt::format("Debug:{} AddKeyword:{}", member->GetFormEditorID(), itr->first));
				StrippingArmorCommon::AddKeyword(member, itr->first);
			}
		} else {
			Info(format("ToggleBodyRoute: {} doesn't have keyword and suit", member->GetFormEditorID()));
		}
	}

	RE::TESObjectARMO* GetArmorFromKeywordMap(std::unordered_map<std::string, bool> keywords, bool isSuit)
	{
		std::string armorName = "";
		if (keywords.contains("SACorpseFrozen")) {
			armorName = "Dummy_CorpseFrozen";
		} else if (keywords.contains("SACorpseDusty")) {
			armorName = "Dummy_CorpseDusty";
		} else {
			armorName = "Dummy_Normal";
		}
		if (keywords.contains("SAAltClothes")) {
			armorName += "Alt";
		}
		armorName += isSuit ? "_Spacesuit" : "_Clothes";
		return StrippingArmorCommon::GetArmor(armorName);
	}

	std::unordered_map<std::string, bool> GetKeywordMapFromArmor(RE::TESObjectREFR* member)
	{
		auto result = std::unordered_map<std::string, bool>();
		auto armors = Utility::CollectEquipArmors(member);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			auto item = itr->first;
			if (Utility::HasKeyword(item, "ArmorTypeSpacesuitBody") || Utility::HasKeyword(item, "ArmorTypeApparelOrNakedBody")) {
				auto name = std::string(item->GetFormEditorID());
				if (name.contains("Alt"))
					result["SAAltClothes"] = true;
				if (name.contains("Frozen"))
					result["SACorpseFrozen"] = true;
				if (name.contains("Dusty"))
					result["SACorpseDusty"] = true;
			}
		}
		return result;
	}

	std::unordered_map<std::string, bool> GetKeywordMapFromActor(RE::TESObjectREFR* member)
	{
		auto result = std::unordered_map<std::string, bool>();

		if (Utility::HasKeyword(member, "SAAltClothes"))
			result["SAAltClothes"] = true;
		if (Utility::HasKeyword(member, "SACorpseFrozen"))
			result["SACorpseFrozen"] = true;
		if (Utility::HasKeyword(member, "SACorpseDusty"))
			result["SACorpseDusty"] = true;
		return result;
	}

	bool IsSuit(RE::TESObjectREFR* member)
	{
		auto armors = Utility::CollectEquipArmors(member);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			auto item = itr->first;
			if (Utility::HasKeyword(item, "ArmorTypeSpacesuitBody")) {
				return true;
			}
		}
		return false;
	}

	bool IsKeyPressed()
	{
		if (!Config::GetStrippingKeyOn())
			return false;
		if (WaitCount > 0) {
			WaitCount--;
			return false;
		}
		bool result = SFSE::WinAPI::GetKeyState(Config::GetStrippingKeyNumber()) & 0x8000;
		if (result) {
			Debug(fmt::format("IsKeyPressed"));
			WaitCount = WaitCountPlus;
		}
		return result;
	}

	bool IsToggleKeyPressed()
	{
		if (!Config::GetToggleNormalOrOthersOn())
			return false;
		if (WaitCount > 0) {
			WaitCount--;
			return false;
		}
		bool result = SFSE::WinAPI::GetKeyState(Config::GetToggleKeyNumber()) & 0x8000;
		if (result)
			WaitCount = WaitCountPlus;
		return result;
	}

	bool HasArmorsForLoot(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return false;

		auto armorMap = Utility::CollectEquipArmors(actor);
		int  count = 0;
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			if (StrippingArmorCommon::IsDummySuits(itr->first))
				continue;
			count++;
		}
		return count != 0;
	}

	void RemoveCandidateKeywords(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return;
		auto scriptMethod = "RemoveCandidateKeywords";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" {}", scriptMethod, member->formID));
	}


}