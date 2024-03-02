#include "StrippingArmorLoot.h"

namespace StrippingArmorLoot
{
	void State_Loot()
	{
		SearchBodies();
		MonitoringBodiesForLoot();
	}

	void SearchBodies()
	{
		auto pairs = StrippingArmorCommon::CollectRefsInCell();
		for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
			auto member = itr->first;
			if (Utility::IsWrongForm(member, "SearchBodies"))
				continue;
			Debug(fmt::format("member: formID:{}", Utility::num2hex(member->formID)));
			Debug(fmt::format("member:{}({})", member->GetFormEditorID(), Utility::num2hex(member->formID)));

			if (!Utility::IsValidNPC(member))
				continue;

			if (StateMachine::IsListed(member))
				continue;

			if (member->IsDead(true)) {
				SelectStatusForLoot(member);
				//State_MonitoringCorpse(member);
			}
		}

		Debug(fmt::format("SearchBodies: before: readyForHouseKeep"));
		bool needHouseKeep = false;
		for (auto member : StateMachine::GetTargetBodiesAll()) {
			if (!pairs.contains(member)) {
				if (!Utility::IsInSameCell(StrippingArmorCommon::Player, member)) {
					Debug(fmt::format("SearchBodies: member:{}({}) is NOT in same cell", member->GetFormEditorID(), Utility::num2hex(member->formID)));
					StateMachine::SetStage(member, StateMachine::STAGE::kRemove, "SearchBodies");
					needHouseKeep = true;
				}
			}
		}
		Debug(fmt::format("SearchBodies: after: readyForHouseKeep"));
		if (needHouseKeep) {
			Debug(fmt::format("RemoveUnnecessaryMember: start:"));
			StateMachine::RemoveUnnecessaryMember();
			Debug(fmt::format("RemoveUnnecessaryMember: finish:"));
		}
	}

	void SelectStatusForLoot(RE::TESObjectREFR* member)
	{
		if (!(member && member->IsDead(true)))
			return;

		Debug(fmt::format("member: {}({}): IsDead", member->GetFormEditorID(), Utility::num2hex(member->formID)));

		auto stage = StateMachine::GetStageFromKeyword(member);
		if (stage == StateMachine::STAGE::kDefault) {
			stage = StateMachine::GetStageFromArmor(member);
		}
		if (stage == StateMachine::STAGE::kDefault) {
			stage = StateMachine::STAGE::kDead;
			StrippingArmorCommon::MemorizeArmorType(member);
			AddArmorsForLoot(member);
			Sleep(Config::GetTimePerFrame());
		}
		int num = static_cast<int>(stage);
		Debug(fmt::format("member: {}({}): stage:{}", member->GetFormEditorID(), Utility::num2hex(member->formID), num));

		StateMachine::SetStage(member, stage, "SelectStatusForLoot");
	}

	void MonitoringBodiesForLoot()
	{
		auto members = StateMachine::GetTargetBodiesAll();
		bool needHouseKeep = false;
		for (auto member : members) {
			//Debug(format("MonitoringBodiesForLoot: member {}({}): stage:{}", member->GetFormEditorID(), member->formID, static_cast<int>(StateMachine::GetStage(member))));
			if (!member || !Utility::IsInSameCell(StrippingArmorCommon::Player, member)) {
				Debug(fmt::format("MonitoringBodiesForLoot: member:{}({}) is NOT in same cell", member->GetFormEditorID(), Utility::num2hex(member->formID)));
				StateMachine::SetStage(member, StateMachine::STAGE::kRemove, "MonitoringBodiesForLoot");
				needHouseKeep = true;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kError) {
				continue;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kDead) {
				ProcessForStageDead(member);
				continue;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kLootCandidate) {
				ProcessForStageLootCandidate(member);
				continue;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kLootWaiting) {
				ProcessForStageLootWaiting(member);
				continue;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kLooted) {
				ProcessForStageLooted(member);
				continue;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kCorpseCandidate) {
				ProcessForStageCorpseCandidate(member);
				continue;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kCorpsed) {
				ProcessForStageCorpsed(member);
				continue;
			} else if (StateMachine::GetStage(member) == StateMachine::STAGE::kCorpsedAndNaked) {
				ProcessForStageCorpsedAndNaked(member);
				continue;
			}
		}
		if (needHouseKeep)
			StateMachine::RemoveUnnecessaryMember();
	}

	void ProcessForStageDead(RE::TESObjectREFR* member)
	{
		if (!StrippingArmorCommon::IsMemorizedArmor(member)) {
			Utility::Error(fmt::format("MonitoringBodiesForLoot: Unknown Error. {}({}) is not memorizedArmor", member->GetFormEditorID(), Utility::num2hex(member->formID)));
			StrippingArmorCommon::MemorizeArmorType(member);
		}
		AddKeywordForLoot(member);
		StateMachine::SetStage(member, StateMachine::STAGE::kLootCandidate, "ProcessForStageDead");
	}

	void ProcessForStageLootCandidate(RE::TESObjectREFR* member)
	{
		if (!StrippingArmorCommon::MCHasKeyword(member, "SACandidateCheckReady")) {
			return;
		}
		if (StrippingArmorCommon::MCHasKeyword(member, "SAConditionOK")) {
			if (!StrippingArmorCommon::ShouldShowSpacesuitMap.contains(member))
				StrippingArmorCommon::ShouldShowSpacesuitMap[member] = StrippingArmorCommon::ShouldShowSpaceSuit(member);
			StrippingArmorCommon::RemoveCandidateKeywords(member);
			StateMachine::SetStage(member, StateMachine::STAGE::kLootWaiting, "ProcessForStageLootCandidate1");
			return;
		}
		if (StrippingArmorCommon::MCHasKeyword(member, "SAConditionNG")) {
			Utility::Error(fmt::format("ProcessForStageLootCandidate: Unknown Error. {}({}) has SAConditionNG in Loot Route", member->GetFormEditorID(), Utility::num2hex(member->formID)));
			StrippingArmorCommon::RemoveCandidateKeywords(member);
			StateMachine::SetStage(member, StateMachine::STAGE::kError, "ProcessForStageLootCandidate2");
			return;
		}
	}

	void ProcessForStageLootWaiting(RE::TESObjectREFR* member)
	{
		if (RemoveArmorIfLooted(member)) {
			StrippingArmorCommon::EquipDummysuit(member);
			StateMachine::SetStage(member, StateMachine::STAGE::kLooted, "ProcessForStageLootWaiting");
		}
	}

	void ProcessForStageLooted(RE::TESObjectREFR* member)
	{
		RemoveArmorIfLooted(member);
		AddKeywordForCorpses(member);
		StateMachine::SetStage(member, StateMachine::STAGE::kCorpseCandidate, "ProcessForStageLooted");
	}

	void ProcessForStageCorpseCandidate(RE::TESObjectREFR* member)
	{
		RemoveArmorIfLooted(member);
		if (!StrippingArmorCommon::MCHasKeyword(member, "SACorpseCheckReady")) {
			return;
		}

		if (!StrippingArmorCommon::ShouldShowSpacesuitMap.contains(member))
			StrippingArmorCommon::ShouldShowSpacesuitMap[member] = StrippingArmorCommon::ShouldShowSpaceSuit(member);

		int HighOrLow = GetHighOrLow(member);

		if (!Config::GetChangingAppearanceOfCorpseEnabled() || HighOrLow == 0) {
			RemoveCorpseKeywords(member);
			StateMachine::SetStage(member, StateMachine::STAGE::kCorpsed, "ProcessForStageCorpseCandidate1");
			return;
		}

		ChangingCorpse(member, HighOrLow);
		RemoveCorpseKeywords(member);
		StateMachine::SetStage(member, StateMachine::STAGE::kCorpsed, "ProcessForStageCorpseCandidate2");
	}

	void ProcessForStageCorpsed(RE::TESObjectREFR* member)
	{
		RemoveArmorIfLooted(member);
		if (!StrippingArmorCommon::HasArmorsForLoot(member)) {
			StateMachine::SetStage(member, StateMachine::STAGE::kCorpsedAndNaked, "ProcessForStageCorpsed");
			return;
		}
		StrippingArmorCommon::UnmemorizeArmorType(member);
	}

	void ProcessForStageCorpsedAndNaked(RE::TESObjectREFR* member)
	{

	}

	void AddArmorsForLoot(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		Info(fmt::format("ReadyForLoot: Start:  {}: {}({})", Utility::num2hex(actor->formID), actor->GetFormEditorID(), actor->GetDisplayFullName()));

		auto armorMap = Utility::CollectInventoryArmors(actor);
		Info(fmt::format("  ReadyForLoot: Ger armorMap: {}: {}", Utility::num2hex(actor->formID), actor->GetFormEditorID()));
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			Info(fmt::format("  ReadyForLoot: Item's formID: {}", Utility::num2hex(item->formID)));
			Info(fmt::format("  ReadyForLoot: Item is {}()", item->GetFormEditorID(), Utility::num2hex(item->formID)));
			bool isDummySuits = StrippingArmorCommon::IsDummySuits(item);
			Info(fmt::format("  ReadyForLoot: isDummySuits: {}", isDummySuits));

			if (StrippingArmorCommon::IsDummySuits(item)) 
				continue;
			bool isEquipped = actor->IsObjectEquipped(item);
			Info(fmt::format("  ReadyForLoot: isEquipped: {}", isEquipped));

			if (!(count == 1 && isEquipped)) {
				Info(fmt::format("  skip: item:{}, count:{}, isEquipped:{}", item->GetFormEditorID(), count, isEquipped));
				continue;
			}
			Info(fmt::format("  ReadyForLoot: before: AddItem"));
			U::AddItem(actor->formID, item->formID, 1);
			Info(fmt::format("  ReadyForLoot: after : AddItem"));
		}
		Info(fmt::format("ReadyForLoot: Finish: {}: {}", Utility::num2hex(actor->formID), actor->GetFormEditorID()));
		return;
	}

	bool RemoveArmorIfLooted(RE::TESObjectREFR* member)
	{
		bool result = false;
		auto armors = GetLootedArmors(member);
		for (auto armor : armors) {
			Info(fmt::format("  Looted:{}: {}: {}", Utility::num2hex(armor->formID), armor->GetFormEditorID(), Utility::GetArmorType(armor)));

			if (Utility::GetArmorType(armor) == "Spacesuit") {
				//if (LootedCorpseMap.contains(member))
				//	LootedCorpseMap[member] = true;
				U::UnequipItem(member->formID, armor->formID);
				U::RemoveItem(member->formID, armor->formID, 99);
				result = true;
			} else {
				U::UnequipItem(member->formID, armor->formID);
				U::RemoveItem(member->formID, armor->formID, 99);
			}
		}
		return result;
	}

	void AddKeywordForLoot(RE::TESObjectREFR* member)
	{
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.AddKeywordForLoot\" \"{}\"",
			member->formID));
	}

	void AddKeywordForCorpses(RE::TESObjectREFR* member)
	{
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.AddKeywordForCorpses\" \"{}\"",
			member->formID));
	}

	bool IsReadyForCorpse(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return false;
		if (!member->IsDead(true))
			return false;
		return member->HasKeyword(StrippingArmorCommon::GetKeyword("SACorpseCheckReady"));
	}

	int GetHighOrLow(RE::TESObjectREFR* member)
	{
		int HighOrLow = 0;
		if (!IsReadyForCorpse(member))
			return 0;

		if (StrippingArmorCommon::MCHasKeyword(member, "SAConditionSealed"))
			return 0;

		if (StrippingArmorCommon::MCHasKeyword(member, "SATemparetureLow")) {
			HighOrLow = -1;
		} else if (StrippingArmorCommon::MCHasKeyword(member, "SATemparetureHigh")) {
			HighOrLow = 1;
		}
		return HighOrLow;
	}

	void ChangingCorpse(RE::TESObjectREFR* member, int HighOrLow)
	{
		Info(format("ChangingCorpseWithKeyword: Start:{}", member->GetFormEditorID()));

		if (!IsReadyForCorpse(member))
			return;

		ChangingCorpseByPapyrus(member, HighOrLow);
		Info(format("ChangingCorpseWithKeyword: Finish:{}", member->GetFormEditorID()));
	}


	void RemoveCorpseKeywords(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return;
		auto scriptMethod = "RemoveCorpseKeywords";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" {}", scriptMethod, member->formID));
		StrippingArmorCommon::ShouldShowSpacesuitMap.erase(member);
	}

	void ChangingCorpseByPapyrus(RE::TESObjectREFR* targetActor, int HighOrLow)
	{
		std::string scriptMethod = "ChangingCorpse";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" \"{}\" \"{}\" \"{}\" \"{}\"", scriptMethod, targetActor->formID, GetCorpsesuit(targetActor)->formID, HighOrLow, Config::GetCorpseTimer()));
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
		editorID += (StrippingArmorCommon::NeedDummysuit(member)) ? "Spacesuit" : "Clothes";

		auto armor = StrippingArmorCommon::GetArmor(editorID);
		return armor;
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
			if (StrippingArmorCommon::IsDummySuits(itr->first))
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

	void ResetParameter()
	{
		StateMachine::ResetLootStageMap();
		StrippingArmorCommon::ArmorClothCombinationMap.clear();
	}
}