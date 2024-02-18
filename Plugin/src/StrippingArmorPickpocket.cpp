#include "StrippingArmorPickpocket.h"

namespace StrippingArmorPickpocket
{
	void State_Pickpocket()
	{
		if (Config::GetPerfectTouchOn()) {
			auto pairs = StrippingArmorCommon::CollectRefsInCell();

			for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
				auto member = itr->first;
				if (!Utility::IsValidNPC(member))
					continue;

				if (!StateMachine::IsListedInPickpocketTarget(member)) {
					if (!HasPickpocketItems(member))
						AddPickpocketItems(member);
					StateMachine::AddToListForPickpocketTarget(member);
					continue;
				} else {
					CheckPickpocket(member);
				}
			}
			return;
		} else {
			if (!StrippingArmorCommon::IsKeyPressed() || !Config::GetConditionPickingPocketOn())
				return;

			//AddKeywordForCandidates(LastTarget, true);
		}
	}

	void AddPickpocketItems(RE::TESObjectREFR* member)
	{
		int theftLevel = U::GetPerkLevel(RE::PlayerCharacter::GetSingleton(), 0x2c555b);  //Skill Theft
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
			std::string itemName = fmt::format("Dummy_{}_for_PickPocket", type);
			auto        weapon = U::GetWeaponFromString(itemName);
			if (!weapon)
				continue;
			if (!AllowTypeByTheftLevel(type, theftLevel))
				continue;

			Info(fmt::format("GetArmorType: {} is {}", itemName, weapon->GetFormEditorID()));
			U::AddItem(member->formID, weapon->formID, 1);

			std::string sKeyword = fmt::format("SAPickpocket{}", type);
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
			if (!Utility::HasKeyword(member, sKeyword)) {
				continue;
			}
			bool flg = miscMap.contains(part);
			//Info(fmt::format("debug: part:{}, sKeyword:{}, flg:{}", part, sKeyword, flg));
			if (miscMap.contains(part)) {
				rest = true;
				continue;
			}
			if (!armorMap.contains(part)) {
				Error(fmt::format("flagItem({}) exists, but, armor doesn't exist. skip and delete keyword({})", miscMap[part]->GetFormEditorID(), sKeyword));
				StrippingArmorCommon::RemoveKeyword(member, sKeyword);
				continue;
			}
			if (!armorMap[part]->GetPlayable())
				armorMap[part]->SetPlayable(true);

			Info(fmt::format("debug: part:{}, sKeyword:{}, flg:{}", part, sKeyword, flg));
			U::UnequipItem(member->formID, armorMap[part]->formID);
			Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.StealItemFromOtherInventory\" \"{}\" \"{}\"",
				member->formID, armorMap[part]->formID));
			StrippingArmorCommon::RemoveKeyword(member, sKeyword);
			auto weapon = U::GetWeaponFromString(Part2MiscMap[part]);
			Info(fmt::format("part:{}, name:{}", part, Part2MiscMap[part]));
			if (!weapon) {
				Info(fmt::format("weapon is null"));
			}
			int num = U::GetItemCount(0x14, weapon->formID, "WEAPON");
			Info(fmt::format("flagItem:{}, num:{}", weapon->GetFormEditorID(), num));

			if (num > 0)
				U::RemoveItem(0x14, weapon->formID, 99);
		}
		if (!rest)
			StrippingArmorCommon::AddKeyword(member, "SAPickpocketDone");
	}

	bool HasPickpocketItems(RE::TESObjectREFR* member)
	{
		if (!member)
			return false;
		if (Utility::HasKeyword(member, "SAPickpocketStart"))
			return true;
		//Info(format("HasPickpocketItems:{} start", member->GetFormEditorID()));
		auto items = Utility::CollectInventoryWeapons(member);
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
		auto items = Utility::CollectInventoryWeapons(member);
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

	void HouseKeepPickpocket()
	{
		for (auto member : StateMachine::GetPickpocketTargetList()) {
			Debug(fmt::format("HouseKeepPickpocket: member:{}({})", member->GetFormEditorID(), Utility::num2hex(member->formID)));
			auto pairs = GetPickpocketFlagItems(member);
			for (auto itr = pairs.begin(); itr != pairs.end(); ++itr) {
				auto item = itr->second;
				Utility::RemoveItem(member->formID, item->formID, 99);
			}
			RemovePickpocketKeywords(member);
		}
		StateMachine::ClearListForPickpocketTarget();
	}

	void RemovePickpocketKeywords(RE::TESObjectREFR* member)
	{
		if (member == nullptr)
			return;
		auto scriptMethod = "RemovePickpocketKeywords";
		Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.{}\" {}", scriptMethod, member->formID));
	}

	std::unordered_map<RE::TESObjectARMO*, std::string> GetArmorTypes(RE::TESObjectREFR* actor)
	{
		Info("GetArmorTypes Start:");
		std::unordered_map<RE::TESObjectARMO*, std::string> result;
		auto                                                armors = Utility::CollectEquipArmors(actor);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			if (StrippingArmorCommon::IsDummySuits(itr->first))
				continue;
			result[itr->first] = Utility::GetArmorType(itr->first);
		}
		Info("GetArmorTypes Finish:");
		return result;
	}

	std::unordered_map<std::string, RE::TESObjectARMO*> GetArmorTypesInverse(RE::TESObjectREFR* actor)
	{
		std::unordered_map<std::string, RE::TESObjectARMO*> result;
		auto                                                armors = Utility::CollectEquipArmors(actor);

		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			if (StrippingArmorCommon::IsDummySuits(itr->first))
				continue;
			result[Utility::GetArmorType(itr->first)] = itr->first;
		}
		return result;
	}

	bool IsPickpocketItems(RE::TESBoundObject* item)
	{
		auto weap = static_cast<RE::TESObjectWEAP*>(item);
		if (!weap)
			return false;
		return weap->ContainsKeywordString("SAForPickpocket");
	}
}
