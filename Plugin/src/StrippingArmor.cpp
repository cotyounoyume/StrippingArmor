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

	void ResetParameter()
	{
		Events::NeedReset = false;
		target = nullptr;
		LastTarget = nullptr;
		crosshairrefOn = false;
		WaitCount = 0;
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

		if (Config::GetDebugExecuteToAllActorsOn() && IsKeyPressed()){
			State_DebugToSameCell();
		} else if (Utility::IsMenuOpen("DialogueMenu")) {
			State_Dialogue();
		} else if (Utility::IsMenuOpen("PickpocketMenu")) {
			State_Pickpocket();
		} else {
			State_Common(target != nullptr, crosshairrefOn);
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

	void State_DebugToSameCell()
	{
		auto cell = RE::PlayerCharacter::GetSingleton()->parentCell;
		if (!cell) {
			Error(format("can't get parentCell"));
			return;
		}
		int cnt = 0;
		for (auto ref : cell->references) {
			auto obj = ref.get();
			if (!obj || !obj->IsActor())
				continue;
			if (obj->formID == 0x14) {
				Info(fmt::format("DebugToSameCell: formID 0x14 is player. Skip"));
				continue;
			}
			auto racename = obj->GetVisualsRace()->GetFormEditorID();
			auto raceid = obj->GetVisualsRace()->GetFormID();

			if (raceid != 0x347d) {
				Info(fmt::format("DebugToSameCell: formID:{} is NOT HumanRace({}). Skip", Utility::num2hex(obj->formID), racename));
				continue;
			}

			Info(fmt::format("DebugToSameCell: to {}:{} (count={})", Utility::num2hex(obj->formID), obj->GetFormEditorID(), cnt++));
			LastTarget = obj;
			AddKeywordForCandidates(LastTarget, true, true);
			if (LastTarget->IsDead(true)) {
				LootedCorpseMap[LastTarget] = true;
				StrippingKeyTappedMap[LastTarget] = true;
			}
			Sleep(Config::GetTimePerFrame()/5);
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

	void State_Pickpocket()
	{
		if (!LastTarget || !LastTarget->IsActor())
			return;

		if (Config::GetPerfectTouchOn()) {
			if (!HasPickpocketItems(LastTarget)) {
				AddPickpocketItems(LastTarget);
				return;
			} else {
				CheckPickpocket(LastTarget);
			}
		} else {
			if (!IsKeyPressed() || !Config::GetConditionPickingPocketOn())
				return;
			crosshairrefOn = true;

			AddKeywordForCandidates(LastTarget, true);
		}
	}

	void StateTargetOffCrosshairOff()
	{
		return;
	}

	void StateTargetOnCrosshairOff_CommonRoute()
	{
		crosshairrefOn = true;
		LastTarget = target;
		MemorizeArmorType(LastTarget);
	}

	void MemorizeArmorType(RE::TESObjectREFR* member)
	{
		//ArmorTypesMap = GetArmorTypes(LastTarget);
		if (!ArmorClothCombinationMap.contains(LastTarget))
			ArmorClothCombinationMap[LastTarget] = GetArmorClothCombination(LastTarget);
	}

	void UnmemorizeArmorType(RE::TESObjectREFR* member)
	{
		ArmorClothCombinationMap.erase(LastTarget);
	}

	void StateTargetOnCrosshairOff_LivingRoute()
	{
		StateTargetOnCrosshairOff_CommonRoute();
	}

	void StateTargetOnCrosshairOn_LivingRoute()
	{
		if (IsKeyPressed()) {
			AddKeywordForCandidates(LastTarget, true, Config::GetDebugExecuteToCrossRefActorForcedOn());
		}
	}

	void StateTargetOffCrosshairOn_Common()
	{
		crosshairrefOn = false;
		UnmemorizeArmorType(LastTarget);
		LastTarget = nullptr;
		Events::DialogueTarget = nullptr;
	}

	void StateTargetOffCrosshairOn_LivingRoute()
	{
		StateTargetOffCrosshairOn_Common();
	}

	void StateTargetOnCrosshairOff_CorpseRoute()
	{
		StateTargetOnCrosshairOff_CommonRoute();

		if (IsTapped())
			return;
		if (IsReady())
			return;

		PrepareForCorpseLooted();
	}

	void StateTargetOnCrosshairOn_CorpseRoute()
	{
		if (!IsKeyPressed()) {
			if (IsTapped())
				return;
			if (ReadyStateMap[LastTarget]) {
				LootAndRemove();
			} else {
				PrepareForCorpseLooted();
			}
			return;
		}

		if (!Config::GetUseStrippingKeyToCorpse())
			return;

		if (HasDummySuits(LastTarget))
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

	void StateTargetOffCrosshairOn_CorpseRoute()
	{
		ReadyStateMap.erase(LastTarget);
		StateTargetOffCrosshairOn_Common();
	}

	void MemberChecker()
	{
		MemberCheckerCandidate();
		MemberCheckerCorpse();
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
				Error(format("MemberCheckerCandidate: member({}) has SACandidateCheckReady, but with no conditions.", member->GetFormEditorID()));
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

	void PrepareForCorpseLooted()
	{
		ReadyForLoot(LastTarget);
		LootedCorpseMap[LastTarget] = false;
		StrippingKeyTappedMap[LastTarget] = false;
		ReadyStateMap[LastTarget] = true;
		WaitCount = WaitCountPlus;
	}

	bool MCHasKeyword(RE::TESObjectREFR* object, std::string keywordString)
	{
		auto keyword = GetKeyword(keywordString);
		if (!keyword)
			return false;
		return object->HasKeyword(keyword);
	}

	bool IsTapped() { return StrippingKeyTappedMap[LastTarget]; }
	bool IsReady() { return ReadyStateMap[LastTarget]; }
	bool IsLooted() { return LootedCorpseMap[LastTarget]; }

	void ReadyForLoot(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		Info(fmt::format("ReadyForLoot: Start:  {}: {}", Utility::num2hex(actor->formID), actor->GetFormEditorID()));
		auto armorMap = Utility::CollectInventoryArmors(actor);
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (IsDummySuits(item))
				continue;
			bool isEquipped = actor->IsObjectEquipped(item);
			if (count != 1 || !isEquipped) {
				Info(fmt::format("  skip: item:{}, count:{}, isEquipped:{}", item->GetFormEditorID(), count, isEquipped));
				continue;
			}
			U::AddItem(actor->formID, item->formID, 1);
		}
		Info(fmt::format("ReadyForLoot: Finish: {}: {}", Utility::num2hex(actor->formID), actor->GetFormEditorID()));
		return;
	}

	void AddPickpocketItems(RE::TESObjectREFR* member)
	{
		int theftLevel = U::GetPerkLevel(RE::PlayerCharacter::GetSingleton(), 0x2c555b); //Skill Theft
		Info(fmt::format("debug: GetTraditionalLootingOn():{}", Config::GetTraditionalLootingOn()));
		Info(fmt::format("debug: GetTraditionalLootingOn():{}", Config::SettingsBoolMapGeneralMajor["TraditionalLootingOnly"]));

		Info(format("AddPickpocketItems: {}", member->GetFormEditorID()));
		if (member == nullptr)
			return;
		auto armors = Utility::CollectEquipArmorsWithoutCount(member);
		std::unordered_map<std::string, bool> flag;
		for (auto armor : armors) {
			Info(fmt::format("GetArmorType: {} is {}", armor->GetFormEditorID(), Utility::GetArmorType(armor)));
			flag[Utility::GetArmorType(armor)] = true;
		}
		for (auto itr = flag.begin(); itr != flag.end(); ++itr) {
			std::string type = itr->first;
			std::string        itemName = fmt::format("Dummy_{}_for_PickPocket", type);
			auto weapon = U::GetWeaponFromString(itemName);
			if (!weapon)
				continue;
			if (!AllowTypeByTheftLevel(type, theftLevel))
				continue;

			Info(fmt::format("GetArmorType: {} is {}", itemName, weapon->GetFormEditorID()));
			U::AddItem(member->formID, weapon->formID, 1);

			std::string     sKeyword = fmt::format("SAPickpocket{}", type);
			Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.SAAddKeywordFromSFSE\" \"{}\" \"{}\"",
				member->formID, sKeyword));
		}
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.SAAddKeywordFromSFSE\" \"{}\" \"{}\"",
			member->formID, "SAPickpocketStart"));
	}

	bool AllowTypeByTheftLevel(std::string type, int level)
	{
		Info(fmt::format("AllowTypeByTheftLevel: type:{}, level:{}, PPLevel:{}", type, level, Config::GetPPLevel(type)));
		return level >= Config::GetPPLevel(type);
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

	void CheckPickpocket(RE::TESObjectREFR* member)
	{
		if (!member)
			return;
		//Info(format("CheckPickpocket: {}", member->GetFormEditorID()));
		if (Utility::HasKeyword(member, "SAPickpocketDone"))
			return;

		auto armorMap = GetArmorTypesInverse(member);
		auto miscMap = GetPickpocketFlagItems(member);

		bool rest = false;
		for (auto part : PartList) {
			std::string sKeyword = fmt::format("SAPickpocket{}", part);
			if (!Utility::HasKeyword(member, sKeyword))
				continue;
			bool flg = miscMap.contains(part);
			//Info(fmt::format("debug: part:{}, sKeyword:{}, flg:{}", part, sKeyword, flg)); 
			if (miscMap.contains(part)) {
				rest = true;
				continue;
			}
			if (!armorMap.contains(part)) {
				Error(fmt::format("flagItem({}) exists, but, armor doesn't exist. skip and delete keyword({})", miscMap[part]->GetFormEditorID(), sKeyword));
				RemoveKeyword(member, sKeyword);
				continue;
			}
			if (!armorMap[part]->GetPlayable())
				armorMap[part]->SetPlayable(true);

			U::UnequipItem(member->formID, armorMap[part]->formID);
			Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.StealItemFromOtherInventory\" \"{}\" \"{}\"",
				member->formID, armorMap[part]->formID));
			RemoveKeyword(member, sKeyword);
			auto weapon = U::GetWeaponFromString(Part2MiscMap[part]);
			//Info(fmt::format("part:{}, name:{}", part, Part2MiscMap[part]));
			//if (!weapon) {
			//	Info(fmt::format("weapon is null"));
			//}
			int num = U::GetItemCount(0x14, weapon->formID, "WEAPON");
			Info(fmt::format("flagItem:{}, num:{}", weapon->GetFormEditorID(), num));

			if (num > 0)
				U::RemoveItem(0x14, weapon->formID, 99);
		}
		if (!rest)
			AddKeyword(member, "SAPickpocketDone");
	}

	void LootAndRemove()
	{
		auto armors = GetLootedArmors(LastTarget);
		for (auto armor : armors) {
			Info(fmt::format("  Looted:{}: {}: {}", Utility::num2hex(armor->formID), armor->GetFormEditorID(), Utility::GetArmorType(armor)));

			if (Utility::GetArmorType(armor) == "Spacesuit") {
				if (LootedCorpseMap.contains(LastTarget))
					LootedCorpseMap[LastTarget] = true;
				AddKeywordForCandidates(LastTarget, false);
				U::UnequipItem(LastTarget->formID, armor->formID);
				U::RemoveItem(LastTarget->formID, armor->formID, 99);
			} else {
				U::UnequipItem(LastTarget->formID, armor->formID);
				U::RemoveItem(LastTarget->formID, armor->formID, 99);
			}
		}
	}

	void AddKeywordForCandidates(RE::TESObjectREFR* member, bool byKey, bool isForcedByDebugging)
	{
		bool bForced = member->IsDead(true) 
			|| (Utility::IsMenuOpen("PickpocketMenu") && Config::GetConditionPickingPocketOn()) 
			|| (Utility::IsMenuOpen("DialogueMenu") && Config::GetConditionTalkingOn());
		if (isForcedByDebugging)
			bForced = true;
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
		Info(fmt::format("StrippingArmorWithKeyword: Start:{}:{}",Utility::num2hex(member->formID)  ,member->GetFormEditorID()));
		if (!IsReady(member))
			return;
		if (!CheckConditionOK(member)) {
			RemoveCandidateKeywords(member);
			return;
		}
		if (ShouldShowSpaceSuit(member) || ArmorClothCombinationMap[member] == 1) {
			ShouldShowSpacesuitMap[member] = true;
		}
		if (!ArmorClothCombinationMap.contains(member))
			ArmorClothCombinationMap[member] = GetArmorClothCombination(member);

		if (!member->IsDead(true)) {
			DropEquipItems(member);
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
		Info(fmt::format("StrippingArmorWithKeyword: Finish:{}:{}",Utility::num2hex(member->formID)  ,member->GetFormEditorID()));
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
		Info(format("ChangingCorpseWithKeyword: Start:{}", member->GetFormEditorID()));

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
		Info(format("ChangingCorpseWithKeyword: Finish:{}", member->GetFormEditorID()));
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
		ShouldShowSpacesuitMap.erase(member);
	}

	void DropEquipItems(RE::TESObjectREFR* member)
	{
		if (!member)
			return;
		Info(fmt::format("DropEquipItems: start: {}:{}", Utility::num2hex(member->formID), member->GetFormEditorID()));
		auto armors = Utility::CollectEquipArmors(member);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (IsDummySuits(item))
				continue;
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
				scriptName, member->formID, item->formID, Config::GetRePickTimer(), Config::GetCanStealDroppedItemOn()));
		}
		Info(fmt::format("DropEquipItems: finish {}:{}", Utility::num2hex(member->formID), member->GetFormEditorID()));
	}

	bool HasDummySuits(RE::TESObjectREFR* member)
	{
		auto armors = Utility::CollectEquipArmors(LastTarget);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (IsDummySuits(item))
				return true;
		}
		return false;
	}

	bool HasPickpocketItems(RE::TESObjectREFR* member)
	{
		if (!member)
			return false;
		if (Utility::HasKeyword(member, "SAPickpocketStart"))
			return true;
		//Info(format("HasPickpocketItems:{} start", member->GetFormEditorID()));
		auto items = Utility::CollectInventoryWeapons(LastTarget);
		for (auto itr = items.begin(); itr != items.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			//Info(format("  item:{}", item->GetFormEditorID()));
			if (IsPickpocketItems(item))
				return true;
		}
		//Info(format("HasPickpocketItems:{} end", member->GetFormEditorID()));
		return false;
	}

	std::unordered_map<std::string, RE::TESObjectWEAP*> GetPickpocketFlagItems(RE::TESObjectREFR* member)
	{
		std::unordered_map<std::string, RE::TESObjectWEAP*> result;
		if (!member)
			return result;
		if (Utility::HasKeyword(member, "SAPickpocketDone"))
			return result;
		//Info(format("GetPickpocketFlagItems:{} start", member->GetFormEditorID()));
		auto items = Utility::CollectInventoryWeapons(LastTarget);
		for (auto itr = items.begin(); itr != items.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (!IsPickpocketItems(item))
				continue;
			if (!Misc2PartMap.contains(item->GetFormEditorID()))
				continue;
			result[Misc2PartMap[item->GetFormEditorID()]] = item;
			//Info(fmt::format("    item:{}, part:{}", item->GetFormEditorID(), Misc2PartMap[item->GetFormEditorID()]));
		}
		//Info(format("GetPickpocketFlagItems:{} end", member->GetFormEditorID()));
		return result;
	}

	void RemoveEquipItems(bool leftOne)
	{
		Info(format("RemoveEquipItem: start"));
		auto armors = Utility::CollectEquipArmors(LastTarget);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (IsDummySuits(item))
				continue;

			Info(fmt::format("  Equipped:{}: {}", Utility::num2hex(item->formID), item->GetFormEditorID()));
			U::RemoveItem(LastTarget->formID, item->formID, count - 1);
			if (leftOne) {
				U::UnequipItem(LastTarget->formID, item->formID);
			}
		}

		Info(format("RemoveEquipItem: finish"));
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
		}else if (NeedDummysuit(member)) {
			if (Config::GetAlternativeClothEnabled()) {
				editorID = "Dummy_NormalAlt_Spacesuit";
				editorIDsub = "Dummy_NormalAlt_Clothes";
			} else {
				editorID = "Dummy_Normal_Spacesuit";
				editorIDsub = "Dummy_Normal_Clothes";
			}
		}

		auto armor = GetArmor(editorID);
		auto armorsub = armor;
		if (editorIDsub == "")
			armorsub = nullptr;
		else
			armorsub = GetArmor(editorIDsub);
		if(armorsub)
			Utility::ExecuteCommandStringOnFormID(member->formID, fmt::format("EquipItem {}", Utility::num2hex(armorsub->formID)));
		Sleep(1);
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

	RE::TESObjectARMO* GetCorpsesuit(RE::TESObjectREFR* member)
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

	int GetArmorClothCombination(RE::TESObjectREFR* member)
	{
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

	bool IsKeyPressed()
	{
		if (!Config::GetStrippingKeyOn())
			return false;
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

	std::vector<RE::TESObjectARMO*> GetLootedArmors(RE::TESObjectREFR* actor)
	{
		std::vector<RE::TESObjectARMO*> result;
		if (actor == nullptr)
			return result;

		auto armorMap = Utility::CollectInventoryArmors(actor);
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			if (itr->second != 1 || !actor->IsObjectEquipped(itr->first))
				continue;
			if (IsDummySuits(itr->first))
				continue;
			result.push_back(itr->first);
		}
		return result;
	}

	int GetEquipmentStackCount(const RE::BGSInventoryItem& item)
	{
		int sum = 0;
		if (item.object->IsArmor()) {
			for (int i = 0; i < item.stacks.size(); i++) {
				sum += item.stacks[i].count;
			}
		}
		return sum;
	}

	std::unordered_map<RE::TESObjectARMO*, std::string> GetArmorTypes(RE::TESObjectREFR* actor)
	{
		Info("GetArmorTypes Start:");
		std::unordered_map<RE::TESObjectARMO*, std::string> result;
		auto                                                armors = Utility::CollectEquipArmors(actor);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			if (IsDummySuits(itr->first))
				continue;
			result[itr->first] = Utility::GetArmorType(itr->first);
		}
		Info("GetArmorTypes Finish:");
		return result;
	}

	std::unordered_map<std::string, RE::TESObjectARMO*> GetArmorTypesInverse(RE::TESObjectREFR* actor)
	{
		//Info("GetArmorTypesInverse Start:");
		std::unordered_map<std::string, RE::TESObjectARMO*> result;
		auto                                                armors = Utility::CollectEquipArmors(actor);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			if (IsDummySuits(itr->first))
				continue;
			result[Utility::GetArmorType(itr->first)] = itr->first;
		}
		//Info("GetArmorTypes Finish:");
		return result;
	}

	bool IsDummySuits(RE::TESObjectARMO* armor)
	{
		if (!armor)
			return false;
		return armor->ContainsKeywordString("DontStripThis");
	}

	bool IsPickpocketItems(RE::TESBoundObject* item)
	{
		auto weap = static_cast<RE::TESObjectWEAP*>(item);
		if (!weap)
			return false;
		return weap->ContainsKeywordString("SAForPickpocket");
	}

}