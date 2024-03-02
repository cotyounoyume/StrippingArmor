#include "StrippingArmorByKey.h"

namespace StrippingArmorByKey
{
	void State_Dialogue()
	{
		if (!DialogTarget) {
			StateMachine::ClearListForForKeytap();
			CallAndCheckDialogTarget();
			return;
		}
		if (!StateMachine::IsListedInForKeytap(DialogTarget, true)) {
			AddKeywordForCandidates(DialogTarget, true);
			AddKeywordMonitoring(true);
		} else {
			if (StrippingArmorCommon::IsKeyPressed())
				ProcessByKey(true);
		}
	}

	void CallAndCheckDialogTarget()
	{
		auto pairs = StrippingArmorCommon::CollectRefsInCell();

		for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
			auto member = itr->first;
			if (!Utility::IsValidNPC(member))
				continue;
			AddKeywordForDialogTarget(member);
			if (StrippingArmorCommon::MCHasKeyword(member, "SAIAMSpeaker")) {
				DialogTarget = member;
				break;
			}
		}
	}

	void State_ToggleByKey()
	{
		if (!Config::GetToggleNormalOrOthersOn())
			return;
		if (StrippingArmorCommon::IsToggleKeyPressed())
			ToggleArmorsByKey();
	}

	void State_StrippingByKey()
	{
		if (!Config::GetStrippingKeyOn())
			return;
		if (StrippingArmorCommon::IsKeyPressed() || Config::GetDebugExecuteToCrossRefActorForcedOn()) {
			bool bForced = Config::GetDebugExecuteToAllActorsOn();
			SearchTargetsByKey(bForced);
		}
		AddKeywordMonitoring();
		ProcessByKey(false);
	}

	void SearchTargetsByKey(bool bForced)
	{
		auto pairs = StrippingArmorCommon::CollectRefsInCell();

		for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
			auto member = itr->first;
			if (!Utility::IsValidNPC(member))
				continue;

			if (StrippingArmorCommon::HasArmorsForLoot(member)) {
				AddKeywordForCandidates(member, bForced);
			}
		}
	}

	void AddKeywordForCandidates(RE::TESObjectREFR* member, bool isForcedByDebugging)
	{
		if (!Config::GetUseStrippingKeyToCorpse() && member->IsDead(true))
			return;

		bool bForced = member->IsDead(true);
		if (isForcedByDebugging)
			bForced = true;
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.AddKeywordForCandidates\" \"{}\" \"{}\" \"{}\"",
			member->formID, bForced, true));
		AddKeywordWaitingList[member] = true;
	}

	void AddKeywordMonitoring(bool inDialog)
	{
		std::vector<RE::TESObjectREFR*> list;
		for (auto itr = AddKeywordWaitingList.begin(); itr != AddKeywordWaitingList.end(); ++itr) {
			auto member = itr->first;

			if (!StrippingArmorCommon::MCHasKeyword(member, "SACandidateCheckReady")) {
				continue;
			}
			if (StrippingArmorCommon::MCHasKeyword(member, "SAConditionNG")) {
				StrippingArmorCommon::RemoveCandidateKeywords(member);
				list.push_back(member);
				continue;
			}

			if (StrippingArmorCommon::MCHasKeyword(member, "SAConditionOK")) {
				if (!StrippingArmorCommon::ShouldShowSpacesuitMap.contains(member))
					StrippingArmorCommon::ShouldShowSpacesuitMap[member] = StrippingArmorCommon::ShouldShowSpaceSuit(member);

				StrippingArmorCommon::MemorizeArmorType(member);
				StateMachine::AddToListForKeytap(member, inDialog);

				StrippingArmorCommon::RemoveCandidateKeywords(member);
				list.push_back(member);
				continue;
			}
		}
		for (auto member : list) {
			AddKeywordWaitingList.erase(member);
		}
	}

	void ProcessByKey(bool inDialog)
	{
		std::vector<RE::TESObjectREFR*> list;
		//Debug("in ProcessByKey:");
		//StateMachine::ForDebugGetMember(true);
		//StateMachine::ForDebugGetMember(false);

		for (auto member : StateMachine::GetForKeytapList(inDialog)) {
			if (member->IsDead(true)) {
				StealEquipArmors(member);
				StrippingArmorCommon::EquipDummysuit(member);
				StateMachine::SetStage(member, StateMachine::STAGE::kLooted);
			} else {
				//std::string msg = inDialog ? "in Dialog" : "in KeyPressed";
				//Debug(fmt::format("Debug: {}", msg));
				DropEquipItems(member);
				StrippingArmorCommon::EquipDummysuit(member);
			}

			StrippingArmorCommon::DoEffectShader(member);
			list.push_back(member);
		}

		for (auto member : list) {
			StateMachine::RemoveFromListForKeytap(member);
		}
	}

	void AddKeywordForDialogTarget(RE::TESObjectREFR* member)
	{
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.SetKeywordDialogTarget\" \"{}\"",
			member->formID));
	}

	void RemoveDialogTargetKeywords(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return;
		auto scriptMethod = "RemoveDialogTargetKeywords";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" {}", scriptMethod, member->formID));
	}

	void ToggleArmorsByKey()
	{
		auto pairs = StrippingArmorCommon::CollectRefsInCell();

		for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
			auto member = itr->first;
			Debug(fmt::format("ToggleArmorsByKey: member:{}({})", member->GetFormEditorID(), Utility::num2hex(member->formID)));
			if (!Utility::IsValidNPC(member))
				continue;
			StrippingArmorCommon::ToggleBodyRoute(member);
		}
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
			if (StrippingArmorCommon::IsDummySuits(item))
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

	void StealEquipArmors(RE::TESObjectREFR* member)
	{
		Info(format("StealEquipArmors: start"));
		auto armors = Utility::CollectEquipArmors(member);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			int  count = itr->second;
			auto item = itr->first;
			if (StrippingArmorCommon::IsDummySuits(item))
				continue;

			Info(fmt::format("  Equipped:{}: count:{} from {}({}) to player",
				item->GetFormEditorID(), count, member->GetFormEditorID(), U::num2hex(member->formID)));
			U::UnequipItem(member->formID, item->formID);
			U::AddItem(StrippingArmorCommon::Player->formID, item->formID, 1);
			U::RemoveItem(member->formID, item->formID, count);
		}
		Info(format("StealEquipArmors: finish"));
	}

	void HouseKeepByKey()
	{
		if (DialogTarget) {
			StateMachine::RemoveFromListForKeytap(DialogTarget);
		}
		DialogTarget = nullptr;
	}
}