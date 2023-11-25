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

			UpdateCrosshairTarget();
			StateSelector();
			MemberChecker();
		}
	}

	int GetStrippingArmorIndex()
	{
		if (StrippingArmorIndex != 0)
			return StrippingArmorIndex;
		StrippingArmorIndex = Utility::GetModIndex(FormIDForModIndex, NameForModIndex);
		return StrippingArmorIndex;
	}

	RE::BGSKeyword* GetKeywordFromID(int formID)
	{
		if (StrippingArmorIndex == 0)
			GetStrippingArmorIndex();
		return Utility::GetKeywordFromID(formID, StrippingArmorIndex);
	}

	RE::TESBoundObject* GetArmorFromID(int formID)
	{
		if (StrippingArmorIndex == 0)
			GetStrippingArmorIndex();
		return Utility::GetArmorFromID(formID, StrippingArmorIndex);
	}

	void ResetParameter()
	{
		Events::NeedReset = false;
		target = nullptr;
		LastTarget = nullptr;
		crosshairrefOn = false;
		WaitCount = 0;
		ArmorTypesMap.clear();
		ReadyStateMap.clear();
		ArmorClothCombinationMap.clear();
		LootedCorpseMap.clear();
		StrippingKeyTappedMap.clear();
	}

	void UpdateCrosshairTarget()
	{
		auto tmpTarget = RE::PlayerCharacter::GetSingleton()->crosshairRef;
		if (tmpTarget != nullptr && tmpTarget->IsActor()) {
			target = tmpTarget;
			Events::DialogueTarget = target;
		} else {
			target = nullptr;
		}
	}

	void StateSelector()
	{
		if (Events::NeedReset) {
			ResetParameter();
			return;
		}
		if (Utility::IsMenuOpen("DialogueMenu")) {
			State_Dialogue();
		} else if (target == nullptr && !crosshairrefOn) {
			State_Common(false, false);
		} else if (target != nullptr && crosshairrefOn) {
			State_Common(true, true);
		} else if (target && !crosshairrefOn) {
			State_Common(true, false);
		} else {
			State_Common(false, true);
		}
	}

	bool IsTargetValid(bool isLastTarget)
	{
		if (isLastTarget) {
			if (LastTarget == nullptr || !LastTarget->IsActor()) {
				return false;
			} else {
				return true;
			}
		} else {
			if (target == nullptr || !target->IsActor()) {
				return false;
			} else {
				return true;
			}
		}
	}

	bool IsTargetDead(bool isLastTarget)
	{
		return (isLastTarget) ? LastTarget->IsDead(true) : target->IsDead(true);
	}

	void State_Common(bool targetOn, bool crosshairOn)
	{
		if (!targetOn && !crosshairOn)
			StateTargetOffCrosshairOff();

		bool isLastTarget = crosshairOn;
		if (!IsTargetValid(isLastTarget))
			return;

		if (IsTargetDead(isLastTarget)) {
			if (targetOn && crosshairOn) {
				StateTargetOnCrosshairOn_CorpseRoute();
			} else if (!targetOn && crosshairOn) {
				StateTargetOffCrosshairOn_CorpseRoute();
			} else if (targetOn && !crosshairOn) {
				StateTargetOnCrosshairOff_CorpseRoute();
			}
		} else {
			if (targetOn && crosshairOn) {
				StateTargetOnCrosshairOn_LivingRoute();
			} else if (!targetOn && crosshairOn) {
				StateTargetOffCrosshairOn_LivingRoute();
			} else if (targetOn && !crosshairOn) {
				StateTargetOnCrosshairOff_LivingRoute();
			}
		}
	}

	void State_Dialogue()
	{
		if (!Events::DialogueTarget)
			return;

		if (IsKeyPressed() && Events::DialogueTarget != nullptr && Events::DialogueTarget->IsActor()) {
		} else {
			return;
		}
		crosshairrefOn = true;
		LastTarget = Events::DialogueTarget;
		Info(format("State_Dialogue: DialogueRoute: LastTarget:{}", LastTarget->GetFormEditorID()));

		AddKeywordForCandidates(LastTarget, true);
	}

	void StateTargetOffCrosshairOff()
	{
		return;
	}

	void StateTargetOnCrosshairOff_CommonRoute()
	{
		crosshairrefOn = true;
		LastTarget = target;
		ArmorTypesMap = Utility::GetArmorTypes(LastTarget);
		if (!ArmorClothCombinationMap.contains(LastTarget))
			ArmorClothCombinationMap[LastTarget] = GetArmorClothCombination();
	}

	void StateTargetOnCrosshairOff_LivingRoute()
	{
		StateTargetOnCrosshairOff_CommonRoute();
	}

	void StateTargetOnCrosshairOn_LivingRoute()
	{
		if (IsKeyPressed()) {
			AddKeywordForCandidates(LastTarget, true);
		}
	}

	void StateTargetOffCrosshairOn_Common()
	{
		crosshairrefOn = false;
		ArmorClothCombinationMap.erase(LastTarget);
		LastTarget = nullptr;
		Events::DialogueTarget = nullptr;
	}

	void StateTargetOffCrosshairOn_LivingRoute()
	{
		StateTargetOffCrosshairOn_Common();
	}

	bool IsTapped() { return StrippingKeyTappedMap[LastTarget]; }
	bool IsReady() { return ReadyStateMap[LastTarget]; }
	bool IsLooted() { return LootedCorpseMap[LastTarget]; }

	void StateTargetOnCrosshairOff_CorpseRoute()
	{
		StateTargetOnCrosshairOff_CommonRoute();

		if (IsTapped())
			return;
		if (IsReady())
			return;

		//Utility::PrintArmorStacks2(target);
		Utility::ReadyForLoot2(LastTarget);
		LootedCorpseMap[LastTarget] = false;
		StrippingKeyTappedMap[LastTarget] = false;
		ReadyStateMap[LastTarget] = true;
		WaitCount = WaitCountPlus;
		//Utility::PrintArmorStacks2(target);
	}

	void StateTargetOnCrosshairOn_CorpseRoute()
	{
		if (!IsKeyPressed()) {
			if (IsTapped())
				return;
			LootAndRemove();
			return;
		}

		if (!Config::GetUseStrippingKeyToCorpse())
			return;

		if (!IsLooted()) {
			AddKeywordForCandidates(LastTarget, true);
			LootedCorpseMap[LastTarget] = true;
			StrippingKeyTappedMap[LastTarget] = true;
		} else {
			RemoveEquipItems(true);
			Sleep(Config::GetTimePerFrame());
			LootedCorpseMap[LastTarget] = true;
			StrippingKeyTappedMap[LastTarget] = true;
		}
	}

	void LootAndRemove()
	{
		auto armors = Utility::GetLootedArmors(LastTarget);
		for (auto armor : armors) {
			if (!ArmorTypesMap.contains(armor))
				continue;
			Info(fmt::format("  Looted:{}: {}: {}", Utility::num2hex(armor->formID), armor->GetFormEditorID(), ArmorTypesMap[armor]));

			if (ArmorTypesMap.contains(armor) && ArmorTypesMap[armor] == "Spacesuit") {
				if (LootedCorpseMap.contains(LastTarget))
					LootedCorpseMap[LastTarget] = true;
				AddKeywordForCandidates(LastTarget, false);
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("UnequipItem {}", Utility::num2hex(armor->formID)));
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} 99", Utility::num2hex(armor->formID)));
			} else {
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("UnequipItem {}", Utility::num2hex(armor->formID)));
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} 99", Utility::num2hex(armor->formID)));
			}
		}
	}

	void StateTargetOffCrosshairOn_CorpseRoute()
	{
		ReadyStateMap.erase(LastTarget);
		//if (LastTarget->GetCurrentLocation() == RE::PlayerCharacter::GetSingleton()->GetCurrentLocation() && Config::GetChangingAppearanceOfCorpseEnabled() && LootedCorpseMap.contains(LastTarget) && LootedCorpseMap[LastTarget] == true) {
		//	ChangingCorpse(LastTarget);
		//}
		StateTargetOffCrosshairOn_Common();
	}

	void MemberChecker()
	{
		MemberCheckerCandidate();
		MemberCheckerCorpse();
	}

	bool MCHasKeyword(RE::TESObjectREFR* object, std::string keywordString)
	{
		auto keyword = GetKeyword(keywordString);
		if (!keyword)
			return false;
		return object->HasKeyword(keyword);
	}

	void MemberCheckerCandidate()
	{
		if (Candidates.empty())
			return;
		std::vector<RE::TESObjectREFR*> unlisted;
		for (auto itr = Candidates.begin(); itr != Candidates.end(); ++itr) {
			auto member = itr->first;
			if (!MCHasKeyword(member, "SACandidateCheckReady")) {
				continue;
			}
			if (!MCHasKeyword(member, "SAConditionNG") && !MCHasKeyword(member, "SAConditionOK")) {
				Info(format("ERROR: MemberCheckerCandidate: member({}) has SACandidateCheckReady, but with no conditions.", member->GetFormEditorID()));
				unlisted.push_back(member);
				continue;
			}

			if (MCHasKeyword(member, "SAConditionOK")) {
				StrippingArmorWithKeyword(member);
				unlisted.push_back(member);
				continue;
			}
			if (MCHasKeyword(member, "SAConditionNG")) {
				RemoveCandidateKeywords(member);
				unlisted.push_back(member);
				continue;
			}
		}

		for (auto member : unlisted) {
			Candidates.erase(member);
		}
	}

	void MemberCheckerCorpse()
	{
		if (Corpses.empty())
			return;
		std::vector<RE::TESObjectREFR*> unlisted;
		for (auto itr = Corpses.begin(); itr != Corpses.end(); ++itr) {
			auto member = itr->first;
			if (!MCHasKeyword(member, "SACorpseCheckReady")) {
				continue;
			}

			if (MCHasKeyword(member, "SATemparetureLow") || MCHasKeyword(member, "SATemparetureHigh")) {
				ChangingCorpseWithKeyword(member);
				unlisted.push_back(member);
				continue;
			}
			if (MCHasKeyword(member, "SATemparetureNormal")) {
				RemoveCorpseKeywords(member);
				unlisted.push_back(member);
				continue;
			}
		}

		for (auto member : unlisted) {
			Corpses.erase(member);
		}
	}

	void AddKeywordForCandidates(RE::TESObjectREFR* member, bool byKey)
	{
		bool bForced = member->IsDead(true) 
			|| (Utility::IsMenuOpen("PickpocketMenu") && Config::GetConditionPickingPocketOn()) 
			|| (Utility::IsMenuOpen("DialogueMenu") && Config::GetConditionTalkingOn());

		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.AddKeywordForCandidates\" \"{}\" \"{}\" \"{}\"",
			member->formID, bForced, byKey));
		Candidates[member] = true;
	}

	void AddKeywordForCorpses(RE::TESObjectREFR* member)
	{
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.AddKeywordForCorpses\" \"{}\"",
			member->formID));
		Corpses[member] = true;
	}

	bool IsReady(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		return member->HasKeyword(GetKeyword("SACandidateCheckReady"));
	}

	bool IsReadyForCorpse(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		if (!member->IsDead(true))
			return false;
		return member->HasKeyword(GetKeyword("SACorpseCheckReady"));
	}

	bool CheckByKey(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		return member->HasKeyword(GetKeyword("SACandidateCheckByKey"));
	}

	bool IsBreathable(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		bool isReadyCandidate = member->HasKeyword(GetKeyword("SACandidateCheckReady"));
		bool isReadyCorpse = member->HasKeyword(GetKeyword("SACorpseCheckReady"));
		bool breathableOK = member->HasKeyword(GetKeyword("SABreathableOK"));

		if (isReadyCandidate || isReadyCorpse) {
			if (breathableOK)
				return true;
		} else {
			Info("ERROR: can't get breathableOK");
		}
		return false;
	}

	bool ShouldShowSpaceSuit(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		bool isReadyCandidate = member->HasKeyword(GetKeyword("SACandidateCheckReady"));
		bool isReadyCorpse = member->HasKeyword(GetKeyword("SACorpseCheckReady"));
		bool ShouldShowSpaceSuitOK = member->HasKeyword(GetKeyword("SAConditionShouldShowSuit"));

		if (isReadyCandidate || isReadyCorpse) {
			if (ShouldShowSpaceSuitOK)
				return true;
		}
		return false;
	}

	bool CheckConditionOK(RE::TESObjectREFR* member)
	{
		bool result = false;
		if (member->HasKeyword(GetKeyword("SADetailEtc")))
			result = true;
		if (member->HasKeyword(GetKeyword("SADetailSleeping")) && Config::GetConditionSleepingOn())
			result = true;
		if (member->HasKeyword(GetKeyword("SADetailUnconscious")) && Config::GetConditionUnconsciousOn())
			result = true;
		if (member->HasKeyword(GetKeyword("SADetailBleedingOut")) && Config::GetConditionBleedingOutOn())
			result = true;
		if (member->HasKeyword(GetKeyword("SADetailCommanded")) && Config::GetConditionIsCommandedOn())
			result = true;
		return result;
	}

	void StrippingArmorWithKeyword(RE::TESObjectREFR* member)
	{
		if (!IsReady(member))
			return;
		if (!CheckConditionOK(member)) {
			RemoveCandidateKeywords(member);
			return;
		}

		ArmorTypesMap = Utility::GetArmorTypes(member);
		if (!ArmorClothCombinationMap.contains(member))
			ArmorClothCombinationMap[member] = GetArmorClothCombination();

		if (!member->IsDead(true)) {
			DropEquipItems();
			EquipDummysuit(member);
		} else {
			if (CheckByKey(member)) {
				RemoveEquipItems(true);
				EquipDummysuit(member);
			} else {
				EquipDummysuit(member);
			}
			AddKeywordForCorpses(member);
		}
		DoEffectShader(member);
		RemoveCandidateKeywords(member);
	}

	int GetHighOrLow(RE::TESObjectREFR* member)
	{
		int HighOrLow = 0;
		if (!IsReadyForCorpse(member))
			return 0;

		if (MCHasKeyword(member, "SAConditionSealed")) 
			return 0;

		if (MCHasKeyword(member, "SATemparetureLow")) {
			HighOrLow = -1;
		} else if (MCHasKeyword(member, "SATemparetureHigh")) {
			HighOrLow = 1;
		}
		return HighOrLow;
	}

	void ChangingCorpseWithKeyword(RE::TESObjectREFR* member)
	{
		if (!IsReadyForCorpse(member))
			return;
		if (!Config::GetChangingAppearanceOfCorpseEnabled()) {
			RemoveCorpseKeywords(member);
			return;
		}

		int HighOrLow = GetHighOrLow(member);
		if (HighOrLow == 0) {
			RemoveCorpseKeywords(member);
			return;
		}
			
		ChangingCorpse2(member, HighOrLow);
		RemoveCorpseKeywords(member);
	}

	void RemoveCandidateKeywords(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return;
		auto scriptMethod = "RemoveCandidateKeywords";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" {}", scriptMethod, member->formID));
	}

	void RemoveCorpseKeywords(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return;
		auto scriptMethod = "RemoveCorpseKeywords";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" {}", scriptMethod, member->formID));
	}

	void DropEquipItems()
	{
		if (!LastTarget)
			return;
		Info(format("DropEquipItems: start: {}", LastTarget->GetFormEditorID()));
		auto armors = Utility::CollectEquipItems(LastTarget, "ARMOR");
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			auto name = std::string(item->GetFormEditorID());
			if (name.contains("Crowd")) {
				Info(fmt::format("  Skip. This item is for crowd race.: {}, {}", Utility::num2hex(item->GetFormID()), name));
				continue;
			}

			Info(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));

			std::string scriptName = "UnequipItem2";
			if (Config::GetEnableDroppingItemsOn()) {
				scriptName = "DropObject2";
			}
			Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
				scriptName, LastTarget->formID, item->formID, Config::GetRePickTimer(), Config::GetCanStealDroppedItemOn()));
		}
		Info(format("DropEquipItems: finish {}", LastTarget->GetFormEditorID()));
	}

	void RemoveEquipItems(bool leftOne)
	{
		Info(format("RemoveEquipItem: start"));
		auto armors = Utility::CollectEquipItems(LastTarget, "ARMOR");
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;

			Info(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));
			Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} {}", Utility::num2hex(item->formID), count - 1));
			if (leftOne) {
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("UnequipItem {}", Utility::num2hex(item->formID)));
			}
		}

		Info(format("RemoveEquipItem: finish"));
	}

	bool NeedDummysuit(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		if (!IsBreathable(member)) {
			return true;
		//}
		//if (ShouldShowSpaceSuit(member)) {
		//	return true;
		//} else {
		//	return false;
		}
		if (ArmorClothCombinationMap[member] >= 2) {
			return false;
		} else if(ArmorClothCombinationMap[member] == 1){
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
		if (NeedDummysuit(member) == false && Config::GetAlternativeClothEnabled() == false)
			return;

		std::string editorID = "Dummy_NormalAlt_Clothes";
		if (NeedDummysuit(member)) {
			if (Config::GetAlternativeClothEnabled())
				editorID = "Dummy_NormalAlt_Spacesuit";
			else
				editorID = "Dummy_Normal_Spacesuit";
		}

		auto armor = GetArmor(editorID);
		Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("EquipItem {}", Utility::num2hex(armor->formID)));
		Info(format("EquipDummysuit: finish: {}", member->GetFormEditorID()));
	}

	void DoEffectShader(RE::TESObjectREFR* member)
	{
		if (!Config::GetEffectEnabled())
			return;
		float timer = 0.5;
		//Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("pms {} 1", Config::GetEffectFormID())); 
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.PlayEffect\" \"{}\" \"{}\" \"{}\"", member->formID, Config::GetEffectFormID(), timer));
	}

	RE::TESBoundObject* GetCorpsesuit(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return nullptr;

		bool altCloth = Config::GetAlternativeClothEnabled();
		int  HighOrLow = GetHighOrLow(member);

		Info(format("EquipCorpsesuit: start: {}, altCloth:{}, HighOrLow:{}", member->GetFormEditorID(), altCloth, HighOrLow));
		if (HighOrLow == 0)
			return nullptr;

		std::string editorID = "Dummy_Corpse";
		editorID += (HighOrLow == 1) ? "Dusty" : "Frozen";
		editorID += (altCloth) ? "Alt_" : "_";
		editorID += (NeedDummysuit(member)) ? "Spacesuit" : "Clothes";

		auto armor = GetArmor(editorID);
		return armor;
	}


	int GetArmorClothCombination()
	{
		bool clothOn = false;
		bool suitOn = false;
		for (auto itr = ArmorTypesMap.begin(); itr != ArmorTypesMap.end(); ++itr) {
			if (itr->second == "Cloth")
				clothOn = true;
			if (itr->second == "Spacesuit")
				suitOn = true;
		}

		int result = (clothOn) ? 2 : 0;
		result += (suitOn) ? 1 : 0;
		Info(format("GetArmorClothCombination: result:{}, clothOn:{}, suitOn:{}", result, clothOn, suitOn));
		return result;
	}

	bool IsKeyPressed()
	{
		if (WaitCount > 0) {
			WaitCount--;
			return false;
		}
		bool result = SFSE::WinAPI::GetKeyState(Config::GetStrippingKeyNumber()) & 0x8000;
		if (result)
			WaitCount = WaitCountPlus;
		return result;
	}

	void ChangingCorpse2(RE::TESObjectREFR* targetActor, int HighOrLow)
	{
		std::string scriptMethod = "ChangingCorpse";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" \"{}\" \"{}\" \"{}\" \"{}\"", scriptMethod, targetActor->formID, GetCorpsesuit(targetActor)->formID, HighOrLow, Config::GetCorpseTimer()));
		LootedCorpseMap.erase(targetActor);
	}

	RE::TESBoundObject* GetArmor(std::string editorID)
	{
		if (!ArmorMap.contains(editorID))
			return nullptr;
		return ArmorMap[editorID];
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
		//std::vector<std::string> list = {
		//	"SACandidateCheckReady",
		//	"SACorpseCheckReady",
		//	"SADontStripThis",
		//	"SANeedDummysuit",
		//	"SAConditionNG",
		//	"SAConditionOK",
		//	"SADetailSleeping",
		//	"SADetailUnconscious",
		//	"SADetailBleedingOut",
		//	"SADetailCommanded",
		//	"SADetailEtc",
		//	"SATemparetureNormal",
		//	"SATemparetureLow",
		//	"SATemparetureHigh",
		//	"SACorpseNormal",
		//	"SACorpseFrozen",
		//	"SACorpseDusty",
		//	"SABreathableNG",
		//	"SABreathableOK",
		//	"SACandidateCheckByKey",
		//	"SACandidateCheckByLoot"
		//};
		std::unordered_map<std::string, int> pairs = {
			{ "SACandidateCheckReady", 0x827 },
			{ "SACorpseCheckReady", 0x828 },
			{ "SADontStripThis", 0x80a },
			{ "SANeedDummysuit", 0x81c },
			{ "SAConditionNG", 0x81d },
			{ "SAConditionOK", 0x822 },
			{ "SADetailSleeping", 0x823 },
			{ "SADetailUnconscious", 0x824 },
			{ "SADetailBleedingOut", 0x825 },
			{ "SADetailCommanded", 0x826 },
			{ "SADetailEtc", 0x829 },
			{ "SATemparetureNormal", 0x821 },
			{ "SATemparetureLow", 0x820 },
			{ "SATemparetureHigh", 0x81f },
			{ "SACorpseNormal", 0x80f },
			{ "SACorpseFrozen", 0x810 },
			{ "SACorpseDusty", 0x811 },
			{ "SABreathableNG", 0x82b },
			{ "SABreathableOK", 0x82a },
			{ "SACandidateCheckByKey", 0x82c },
			{ "SACandidateCheckByLoot", 0x82d },
			{ "SAConditionSealed", 0x82e },
			{ "SAConditionShouldShowSuit", 0x82f },
		};
		Info(format("MakeKeywordMapIfNeeded: process: list:{}", pairs.size()));
		auto form = RE::TESForm::LookupByEditorID("SACandidateCheckReady");
		if (!form) {
			Info(format("TEST: LookupByEditorID is inactive"));
		}		

		for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
			auto key = GetKeywordFromID(itr->second);
			if (!key) {
				Info("key is null");
			}
			KeywordMap[itr->first] = GetKeywordFromID(itr->second);
		}
		Info("MakeKeywordMapIfNeeded: process end:");
	}

	void MakeArmorMapIfNeeded()
	{
		Info(format("MakeArmorMapIfNeeded: start: ArmorMap:{}", ArmorMap.size()));

		if (!ArmorMap.size() == 0)
			return;
		std::unordered_map<std::string, int> pairs = {
			{ "Dummy_Normal_Spacesuit", 0x805 },
			{ "Dummy_NormalAlt_Clothes", 0x807 },
			{ "Dummy_NormalAlt_Spacesuit", 0x809 },
			{ "Dummy_CorpseFrozen_Spacesuit", 0x80b },
			{ "Dummy_CorpseFrozen_Clothes", 0x80c },
			{ "Dummy_CorpseDusty_Spacesuit", 0x80d },
			{ "Dummy_CorpseDusty_Clothes", 0x80e },
			{ "Dummy_CorpseFrozenAlt_Spacesuit", 0x818 },
			{ "Dummy_CorpseFrozenAlt_Clothes", 0x819 },
			{ "Dummy_CorpseDustyAlt_Spacesuit", 0x81a },
			{ "Dummy_CorpseDustyAlt_Clothes", 0x81b },
		};
		Info(format("MakeArmorMapIfNeeded: process: list:{}", pairs.size()));

		for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
			auto key = GetArmorFromID(itr->second);
			if (!key) {
				Info("armor is null");
			}
			ArmorMap[itr->first] = GetArmorFromID(itr->second);
		}
		Info("MakeArmorMapIfNeeded: process end:");
	}

}