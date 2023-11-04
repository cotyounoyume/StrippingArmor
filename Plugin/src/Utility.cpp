#pragma once
#include "Utility.h"

namespace Utility
{
	void ExecuteCommandString(std::string command)
	{
		ExecuteCommand(0, command.data());
		Utility::Notification(command);
	}

	void Notification(std::string message)
	{
		bool NotificationOn = false;
		bool InfoOn = false;
		//NotificationOn = true;
		InfoOn = true;
		if (NotificationOn)
			RE::DebugNotification(message.c_str());
		if (InfoOn)
			logger::info("{}", message);
	}

	bool InGameScene()
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		if (player == nullptr)
			return false;
		if (RE::UI::GetSingleton()->IsMenuOpen("Console"))
			return false;
		return true;
	}

	std::vector<int> DecomposeSlot(uint32_t value)
	{
		std::vector<int> powers;

		while (value > 0) {
			int logVal = static_cast<int>(std::log2(value));
			powers.push_back(logVal);
			value -= static_cast<unsigned int>(std::pow(2, logVal));
		}

		return powers;
	}

	bool HasSlot(uint32_t value, int slot)
	{
		return value & (1 << slot);
	}

	std::vector<int> GetArmorFormIDs(RE::TESObjectREFR* actor)
	{
		SlotList.clear();
		if (actor == nullptr)
			return SlotList;

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor())
				SlotList.push_back(item.object->formID);
			return RE::BSContainer::ForEachResult::kContinue;
		};

		actor->ForEachEquippedItem(scanner);
		//for (auto formID : SlotList) {
		//	Notification(fmt::format("  item: {}", num2hex(formID)));
		//}
		return SlotList;
	}

	void ReadyForLoot(RE::TESObjectREFR* actor) 
	{
		if (actor == nullptr)
			return;
		CurrentActor = actor;
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor() && CurrentActor->IsObjectEquipped(item.object)) {
				int sum = 0;
				for (int i = 0; i < item.stacks.size(); i++) {
					sum += item.stacks[i].unk10;
				}
				if (sum == 1) {
					std::string command = fmt::format("{}.additem {} 1", num2hex(CurrentActor->formID), num2hex(item.object->formID));
					ExecuteCommandString(command);
				}
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		return;
	}

	void GetEquippedArmors(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		CurrentActor = actor;
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor() && CurrentActor->IsObjectEquipped(item.object)) {
				int sum = 0;
				for (int i = 0; i < item.stacks.size(); i++) {
					sum += item.stacks[i].unk10;
				}
				if (sum == 1) {
					std::string command = fmt::format("{}.additem {} 1", num2hex(CurrentActor->formID), num2hex(item.object->formID));
					ExecuteCommandString(command);
				}
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		return;
	}

	//std::vector<int> GetItemFormIDs(RE::TESObjectREFR* actor)
	//{
	//	ItemList.clear();
	//	if (actor == nullptr)
	//		return ItemList;

	//	auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
	//		if (item.object->IsArmor())
	//			ItemList.push_back(item.object->formID);
	//		bool lockStatus = item.flags.any(RE::BGSInventoryItem::Flag::kEquipStateLocked);
	//		bool any1 = item.flags.any(RE::BGSInventoryItem::Flag::kInvShouldEquip);
	//		bool any2 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex1);
	//		bool any3 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex2);
	//		bool any4 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex3);
	//		bool any5 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotMask);
	//		bool any6 = item.flags.any(RE::BGSInventoryItem::Flag::kTemporary);
	//		auto unk24 = item.unk24;
	//		//bool bPlayable = item.object->GetPlayable();

	//		Notification(fmt::format("item:{}, {}", num2hex(item.object->formID), item.object->GetFormEditorID()));
	//		Notification(fmt::format("  kEquipStateLocked\t:{}", lockStatus));
	//		Notification(fmt::format("  kInvShouldEquip\t:{}", any1));
	//		Notification(fmt::format("  kSlotIndex1\t:{}", any2));
	//		Notification(fmt::format("  kSlotIndex2\t:{}", any3));
	//		Notification(fmt::format("  kSlotIndex3\t:{}", any4));
	//		Notification(fmt::format("  kSlotMask\t:{}", any5));
	//		Notification(fmt::format("  kTemporary\t:{}", any6));
	//		Notification(fmt::format("  unk24\t:{}", unk24));
	//		if (!item.stacks.empty()) {
	//			for (int i = 0; i < item.stacks.size(); i++) {
	//				Notification(fmt::format("    {}:unk10:{}", i, item.stacks[i].unk10));
	//			}
	//		}
	//		//Notification(fmt::format("  GetPlayable\t:{}", bPlayable));
	//		return RE::BSContainer::ForEachResult::kContinue;
	//	};

	//	actor->ForEachInventoryItem(scanner);
	//	//for (auto formID : SlotList) {
	//	//	Notification(fmt::format("  item: {}", num2hex(formID)));
	//	//}
	//	return ItemList;		
	//}

	std::unordered_map<int, RE::TESBoundObject*> GetArmorFormIDPairs(RE::TESObjectREFR* actor)
	{
		SlotPair.clear();
		if (actor == nullptr)
			return SlotPair;
		CurrentActor = actor;
			
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor() && CurrentActor->IsObjectEquipped(item.object))
				SlotPair[item.object->formID] = item.object;
			return RE::BSContainer::ForEachResult::kContinue;
		};

		actor->ForEachEquippedItem(scanner);
		return SlotPair;
	}

	std::vector<int> GetUnequippedFormID(std::vector<int> lastVector, std::vector<int> currentVector)
	{
		std::vector<int> diff;
		auto             currentMap = Vector2Map(currentVector);
		for (int num : lastVector) {
			if (currentMap.find(num) != currentMap.end())
				continue;
			diff.push_back(num);
		}
	}

	std::unordered_map<int, bool> Vector2Map(std::vector<int> vector)
	{
		std::unordered_map<int, bool> result;
		for (int num : vector) {
			result[num] = true;
		}
		return result;
	}

	std::string GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn)
	{
		std::string result = "";
		std::string quote = quoteOn ? "\"" : "";
		for (int formID : list) {
			std::string sFormID = (hexOn) ? num2hex(formID) : fmt::format("{}", formID);
			//result = result + separator + quote + Utility::int2hex(formID) + quote;
			result = result + separator + quote + sFormID + quote;
		}
		return result;
	}

	std::string num2hex(uint32_t formID)
	{
		return fmt::format("{:#x}", formID);
	}
}