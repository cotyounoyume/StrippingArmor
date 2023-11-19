#include "StrippingArmor.h"

namespace StrippingArmor
{
	void info(std::string msg)
	{
		Utility::Notification(msg);
	}

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
			//} else if (Events::DialogueTarget != nullptr && Events::DialogueTarget->IsActor()) {
			//	target = Events::DialogueTarget;
		} else {
			target = nullptr;
		}
	}

	bool IsDialogOpen()
	{
		return RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu");
	}

	void StateSelector()
	{
		if (Events::NeedReset) {
			ResetParameter();
			return;
		}
		if (IsDialogOpen()) {
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
				//Utility::Notification("UNKNOWN ERROR: IsTargetValid: false");
				return false;
			} else {
				return true;
			}
		} else {
			if (target == nullptr || !target->IsActor()) {
				//Utility::Notification("UNKNOWN ERROR: IsTargetValid: false");
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
		//Utility::Notification(fmt::format("State_Dialogue: {}", Events::DialogueTarget->GetFormEditorID()));

		if (IsKeyPressed() && Events::DialogueTarget != nullptr && Events::DialogueTarget->IsActor()) {
			//Utility::Notification("route1");
		} else {
			//Utility::Notification("route2");
			return;
		}
		crosshairrefOn = true;
		LastTarget = Events::DialogueTarget;
		Utility::Notification(fmt::format("State_Dialogue: DialogueRoute: LastTarget:{}", LastTarget->GetFormEditorID()));

		AddKeywordForCandidates(LastTarget, true);
		//ArmorTypesMap = Utility::GetArmorTypes(LastTarget);
		//if (!ArmorClothCombinationMap.contains(LastTarget))
		//	ArmorClothCombinationMap[LastTarget] = GetArmorClothCombination();

		//WaitCount = 10;
		//RemoveEquipItems(true);
		////DropEquipItems();
		//StrippingArmor(LastTarget);
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
			//WaitCount = 10;
			//RemoveEquipItems(true);
			////DropEquipItems();
			//StrippingArmor(LastTarget);
		}
	}

	void StateTargetOffCrosshairOn_Common()
	{
		crosshairrefOn = false;
		//Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route3"));
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
			//RemoveEquipItems();
			//Sleep(Config::GetTimePerFrame());
			//StrippingArmor(LastTarget);
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
			Utility::Notification(fmt::format("  Looted:{}: {}: {}", Utility::num2hex(armor->formID), armor->GetFormEditorID(), ArmorTypesMap[armor]));

			if (ArmorTypesMap.contains(armor) && ArmorTypesMap[armor] == "Spacesuit") {
				if (LootedCorpseMap.contains(LastTarget))
					LootedCorpseMap[LastTarget] = true;
				//StrippingArmorAlt(LastTarget);
				AddKeywordForCandidates(LastTarget, false);
				//EquipDummysuit(
				//if (Config::GetEffectEnabled())
				//	Sleep(Config::GetTimePerFrame());
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
		if (LastTarget->GetCurrentLocation() == RE::PlayerCharacter::GetSingleton()->GetCurrentLocation() && Config::GetChangingAppearanceOfCorpseEnabled() && LootedCorpseMap.contains(LastTarget) && LootedCorpseMap[LastTarget] == true) {
			//Utility::Notification(fmt::format("StateTargetOffCrosshairOn: route2"));
			ChangingCorpse(LastTarget);
		}
		StateTargetOffCrosshairOn_Common();
	}

	void MemberChecker()
	{
		MemberCheckerCandidate();
		MemberCheckerCorpse();
	}

	bool MCHasKeyword(RE::TESObjectREFR* object, std::string keywordString)
	{
		auto keyword = Utility::GetKeyword(keywordString);
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
				Utility::Notification(fmt::format("ERROR: MemberCheckerCandidate: member({}) has SACandidateCheckReady, but with no conditions.", member->GetFormEditorID()));
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
		bool bForced = member->IsDead(true) || (RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu") || RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu"));

		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.AddKeywordForCandidates\" \"{}\" \"{}\" \"{}\"",
			member->formID, bForced, byKey));
		Candidates[member] = true;
	}

	void AddKeywordForCorpses(RE::TESObjectREFR* member)
	{
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.AddKeywordForCorpses\" \"{}\"",
			member->formID));
		Candidates[member] = true;
	}

	bool IsReady(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		return member->HasKeyword(Utility::GetKeyword("SACandidateCheckReady"));
	}

	bool IsReadyForCorpse(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		if (!member->IsDead(true))
			return false;
		return member->HasKeyword(Utility::GetKeyword("SACorpseCheckReady"));
	}

	bool CheckByKey(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		return member->HasKeyword(Utility::GetKeyword("SACandidateCheckByKey"));
	}

	bool IsBreathable(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		auto scriptMethod = "StrippingArmorWithKeyword";
		bool isReady = member->HasKeyword(Utility::GetKeyword("SACandidateCheckReady"));
		bool conditionOK = member->HasKeyword(Utility::GetKeyword("SAConditionOK"));
		//bool conditionNG = member->HasKeyword(Utility::GetKeyword("SAConditionNG"));
		bool breathableOK = member->HasKeyword(Utility::GetKeyword("SABreathableOK"));
		//bool breathableNG = member->HasKeyword(Utility::GetKeyword("SABreathableNG"));
		//bool isEtc = member->HasKeyword(Utility::GetKeyword("SADetailEtc"));
		//bool isSleeping = member->HasKeyword(Utility::GetKeyword("SADetailSleeping"));
		//bool isBleedingOut = member->HasKeyword(Utility::GetKeyword("SADetailBleedingOut"));
		//bool isCommanded = member->HasKeyword(Utility::GetKeyword("SADetailCommanded"));
		//bool isUnconscious = member->HasKeyword(Utility::GetKeyword("SADetailUnconscious"));
		bool byKey  = member->HasKeyword(Utility::GetKeyword("SACandidateCheckByKey"));
		bool byLoot = member->HasKeyword(Utility::GetKeyword("SACandidateCheckByLoot"));
		//Utility::Notification(fmt::format("  IsBreathable: {}: isReady:{}, conditionOK:{}, conditionNG:{}, isEtc:{}, isSleeping:{}, isBleedingOut:{}, isCommanded:{}, isUnconscious:{}",
		//	member->GetFormEditorID(), isReady, conditionOK, conditionNG, isEtc, isSleeping, isBleedingOut, isCommanded, isUnconscious));
		//Utility::Notification(fmt::format("  IsBreathable: {}: breathableOK:{}, breathableNG:{}, byKey:{}, byLoot:{}",
		//	member->GetFormEditorID(), breathableOK, breathableNG, byKey, byLoot));
		
		if (isReady && conditionOK && breathableOK)
				return true;
		return false;
	}

	void StrippingArmorWithKeyword(RE::TESObjectREFR* member)
	{
		if (!IsReady(member))
				return;

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

		RemoveCandidateKeywords(member);
	}

	void ChangingCorpseWithKeyword(RE::TESObjectREFR* member)
	{
		if (!IsReadyForCorpse(member))
				return;

		int HighOrLow = 0;
		if (MCHasKeyword(member, "SATemparetureLow")) {
			HighOrLow = -1;
		} else if (MCHasKeyword(member, "SATemparetureHigh")) {
			HighOrLow = 1;
		} else {
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
		Utility::Notification(fmt::format("DropEquipItems: start: {}", LastTarget->GetFormEditorID()));
		auto armors = Utility::CollectEquipItems(LastTarget, "ARMOR");
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			auto name = std::string(item->GetFormEditorID());
			if (name.contains("Crowd")) {
				Utility::Notification(fmt::format("  Skip. This item is for crowd race.: {}, {}", Utility::num2hex(item->GetFormID()), name));
				continue;
			}

			Utility::Notification(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));

			std::string scriptName = "UnequipItem2";
			if (Config::GetEnableDroppingItemsOn()) {
				scriptName = "DropObject2";
			}
			Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
				scriptName, LastTarget->formID, item->formID, Config::GetRePickTimer(), Config::GetCanStealDroppedItemOn()));

			//Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.DropObjectScript\" \"{}\" \"{}\" \"{}\" \"{}\" \"{}\"",
			//	LastTarget->formID, item->formID, ArmorClothCombinationMap[LastTarget], GetBitParams(), RepickTimer));
		}
		Utility::Notification(fmt::format("DropEquipItems: finish {}", LastTarget->GetFormEditorID()));
	}

	void RemoveEquipItems(bool leftOne)
	{
		Utility::Notification(fmt::format("RemoveEquipItem: start"));
		auto armors = Utility::CollectEquipItems(LastTarget, "ARMOR");
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;

			Utility::Notification(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));
			Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("RemoveItem {} {}", Utility::num2hex(item->formID), count - 1));
			if (leftOne) {
				Utility::ExecuteCommandStringOnFormID(LastTarget->formID, fmt::format("UnequipItem {}", Utility::num2hex(item->formID)));
			}
		}

		Utility::Notification(fmt::format("RemoveEquipItem: finish"));
	}

	bool NeedDummysuit(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		if (!IsBreathable(member)) {
			return true;
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
		info(fmt::format("EquipDummysuit: start: {}", member->GetFormEditorID()));
		if (NeedDummysuit(member) == false && Config::GetAlternativeClothEnabled() == false)
			return;

		std::string editorID = "Dummy_NormalAlt_Clothes";
		if (NeedDummysuit(member)) {
			if (Config::GetAlternativeClothEnabled())
				editorID = "Dummy_NormalAlt_Spacesuit";
			else
				editorID = "Dummy_Normal_Spacesuit";
		}

		auto armor = Utility::GetArmorFromString(editorID);
		Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("EquipItem {}", Utility::num2hex(armor->formID)));
		info(fmt::format("EquipDummysuit: finish: {}", member->GetFormEditorID()));
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
		Utility::Notification(fmt::format("GetArmorClothCombination: result:{}, clothOn:{}, suitOn:{}", result, clothOn, suitOn));
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

	int GetBitParams()
	{
		//0:bForced, 1:
		int  param = 0;
		bool bForced = (RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu") || RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu"));
		param += bForced ? 1 : 0;
		param += Config::GetAlternativeClothEnabled() ? 2 : 0;
		param += Config::GetChangingAppearanceOfCorpseEnabled() ? 4 : 0;
		param += Config::GetEffectEnabled() ? 8 : 0;
		param += Config::GetUseStrippingKeyToCorpse() ? 16 : 0;
		param += Config::GetEffectForCorpseEnabled() ? 32 : 0;
		return param;
	}

	void DoStrippingArmorPapyrusCommon(std::string scriptMethod, std::string type, RE::TESObjectREFR* targetActor)
	{
		if (targetActor == nullptr)
			return;
		std::string params = fmt::format("\"{}\" \"{}\" \"{}\" \"{}\" \"{}\"", 
			targetActor->formID,
			ArmorClothCombinationMap[targetActor], 
			GetBitParams(), 
			Config::GetEffectFormID(), 
			Config::GetCorpseTimer());
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" {} {}", scriptMethod, type, params));
	}

	void StrippingArmor(RE::TESObjectREFR* targetActor)
	{
		DoStrippingArmorPapyrusCommon("CommonRunMe", "StrippingKey", targetActor);
	}

	void StrippingArmorAlt(RE::TESObjectREFR* targetActor)
	{
		DoStrippingArmorPapyrusCommon("CommonRunMe", "Looting", targetActor);
	}

	void ChangingCorpse(RE::TESObjectREFR* targetActor)
	{
		DoStrippingArmorPapyrusCommon("CommonRunMe", "ChangingCorpse", targetActor);
		LootedCorpseMap.erase(targetActor);
	}

	void ChangingCorpse2(RE::TESObjectREFR* targetActor, int HighOrLow)
	{
		std::string scriptMethod = "ChangingCorpse";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" \"{}\" \"{}\" \"{}\"", scriptMethod, targetActor->formID, HighOrLow, Config::GetCorpseTimer()));
		LootedCorpseMap.erase(targetActor);
	}
}