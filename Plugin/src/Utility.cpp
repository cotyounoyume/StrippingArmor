#pragma once
#include "Utility.h"

namespace Utility
{
	void ExecuteCommandString(std::string command, std::string displayPrefix)
	{
		ExecuteCommand(0, command.data());
		Utility::Notification(displayPrefix + command);
	}

	void ExecuteCommandStringOnFormID(int formID, std::string subCommand)
	{
		std::string command = fmt::format("{}.{}", num2hex(formID), subCommand);
		ExecuteCommandString(command);
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

	std::vector<int> GetArmorFormIDs(RE::TESObjectREFR* actor, bool isEquipped)
	{
		SlotList.clear();
		if (actor == nullptr)
			return SlotList;

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor())
				SlotList.push_back(item.object->formID);
			return RE::BSContainer::ForEachResult::kContinue;
		};

		if (isEquipped) {
			actor->ForEachEquippedItem(scanner);
		} else {
			actor->ForEachInventoryItem(scanner);
		}
		//for (auto formID : SlotList) {
		//	Notification(fmt::format("  item: {}", num2hex(formID)));
		//}
		return SlotList;
	}

	std::vector<RE::TESBoundObject*> GetLootedArmors(RE::TESObjectREFR* actor)
	{
		std::vector<RE::TESBoundObject*> result;
		if (actor == nullptr)
			return result;
		
		auto armors = GetEquipmentCountPair(actor);
		for (auto itr = armors.begin(); itr != armors.end(); ++itr) {
			if (itr->second != 1 || !actor->IsObjectEquipped(itr->first))
				continue;
			result.push_back(itr->first);
		}
		return result;

		//ArmorList.clear();
		//if (actor == nullptr)
		//	return ArmorList;
		//std::vector<RE::TESBoundObject*> result;
		//CurrentActor = actor;
		//auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
		//	if (item.object->IsArmor()) {
		//		if (GetEquipmentStackCount(item) == 1) {
		//			ArmorList.push_back(item.object);
		//		}
		//	}
		//	return RE::BSContainer::ForEachResult::kContinue;
		//};
		//actor->ForEachEquippedItem(scanner);
		//return ArmorList;
	}

	void ReadyForLoot(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		Notification(fmt::format("ReadyForLoot: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		CurrentActor = actor;
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				if (GetEquipmentStackCount(item) == 1) {
					ExecuteCommandStringOnFormID(CurrentActor->formID, fmt::format("additem {} 1", num2hex(item.object->formID)));
				}
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		Notification(fmt::format("ReadyForLoot: Finish: {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		return;
	}

	void ReadyForLoot2(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		Notification(fmt::format("ReadyForLoot2: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		auto armorMap = GetEquipmentCountPair(actor);
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			int count = itr->second;
			auto item = itr->first;
			bool isEquipped = actor->IsObjectEquipped(item);
			if (count != 1 || !isEquipped) {
				Notification(fmt::format("  skip: item:{}, count:{}, isEquipped:{}", item->GetFormEditorID(), count, isEquipped));
				continue;
			}
			ExecuteCommandStringOnFormID(actor->formID, fmt::format("additem {} 1", num2hex(item->formID)));
		}
		Notification(fmt::format("ReadyForLoot2: Finish: {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		return;
	}

	void PrintArmorStacks(RE::TESObjectREFR* actor, bool isEquipped)
	{
		if (actor == nullptr)
			return;
		Notification(fmt::format("  PrintArmorStacks: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				Notification(fmt::format("    item: {}: {}: {}", num2hex(item.object->formID), item.object->GetFormEditorID(), GetEquipmentStackCount(item)));
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};		
		//if (isEquipped) {
		//	actor->ForEachEquippedItem(scanner);
		//} else {
			actor->ForEachInventoryItem(scanner);
		//}
		Notification(fmt::format("  PrintArmorStacks: Finish: {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
	}

	void PrintArmorStacks2(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		Notification(fmt::format("  PrintArmorStacks2: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		auto armorMap = GetEquipmentCountPair(actor);
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			Notification(fmt::format("  item: {}, count:{}, isEquipped:{}", itr->first->GetFormEditorID(), itr->second, actor->IsObjectEquipped(itr->first))); 
		}
		Notification(fmt::format("  PrintArmorStacks2: Finish: {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
	}

	std::unordered_map<RE::TESBoundObject*, int> GetEquipmentCountPair(RE::TESObjectREFR* actor)
	{
		EquipmentPair.clear();
		if (actor == nullptr)
			return EquipmentPair;
		//Notification(fmt::format("  GetEquipmentCountPair: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				EquipmentPair[item.object] += GetEquipmentStackCount(item);
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachInventoryItem(scanner);
		//Notification(fmt::format("  GetEquipmentCountPair: Finish: {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		return EquipmentPair;
	}

	int GetEquipmentStackCount(const RE::BGSInventoryItem& item)
	{
		int sum = 0;
		if (item.object->IsArmor()) {
			for (int i = 0; i < item.stacks.size(); i++) {
				//Notification(fmt::format("  item: {}: {}: stacks[{}].unk10={}", num2hex(item.object->formID), item.object->GetFormEditorID(), i, item.stacks[i].unk10));
				sum += item.stacks[i].unk10;
			}
		}
		return sum;
	}
	//void GetEquippedArmors(RE::TESObjectREFR* actor)
	//{
	//	if (actor == nullptr)
	//		return;
	//	CurrentActor = actor;
	//	auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
	//		if (item.object->IsArmor() && CurrentActor->IsObjectEquipped(item.object)) {
	//			int sum = 0;
	//			for (int i = 0; i < item.stacks.size(); i++) {
	//				sum += item.stacks[i].unk10;
	//			}
	//			if (sum == 1) {
	//				std::string command = fmt::format("{}.additem {} 1", num2hex(CurrentActor->formID), num2hex(item.object->formID));
	//				ExecuteCommandString(command);
	//			}
	//		}
	//		return RE::BSContainer::ForEachResult::kContinue;
	//	};
	//	actor->ForEachEquippedItem(scanner);
	//	return;
	//}

	std::vector<int> GetItemFormIDs(RE::TESObjectREFR* actor)
	{
		ItemList.clear();
		if (actor == nullptr)
			return ItemList;
		debugCount = 0;
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			debugCount++;
			if (item.object->IsArmor())
				ItemList.push_back(item.object->formID);
			bool lockStatus = item.flags.any(RE::BGSInventoryItem::Flag::kEquipStateLocked);
			bool any1 = item.flags.any(RE::BGSInventoryItem::Flag::kInvShouldEquip);
			bool any2 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex1);
			bool any3 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex2);
			bool any4 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex3);
			bool any5 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotMask);
			bool any6 = item.flags.any(RE::BGSInventoryItem::Flag::kTemporary);
			auto unk24 = item.unk24;
			//bool bPlayable = item.object->GetPlayable();

			Notification(fmt::format("item:{}, {}", num2hex(item.object->formID), item.object->GetFormEditorID()));
			Notification(fmt::format("  debugCount\t:{}", debugCount));
			Notification(fmt::format("  kEquipStateLocked\t:{}", lockStatus));
			Notification(fmt::format("  kInvShouldEquip\t:{}", any1));
			Notification(fmt::format("  kSlotIndex1\t:{}", any2));
			Notification(fmt::format("  kSlotIndex2\t:{}", any3));
			Notification(fmt::format("  kSlotIndex3\t:{}", any4));
			Notification(fmt::format("  kSlotMask\t:{}", any5));
			Notification(fmt::format("  kTemporary\t:{}", any6));
			Notification(fmt::format("  unk24\t:{}", unk24));
			if (!item.stacks.empty()) {
				for (int i = 0; i < item.stacks.size(); i++) {
					Notification(fmt::format("    {}:unk10:{}", i, item.stacks[i].unk10));
					item.stacks[i].extra->IncRef();
				}
			}
			//Notification(fmt::format("  GetPlayable\t:{}", bPlayable));
			return RE::BSContainer::ForEachResult::kContinue;
		};

		actor->ForEachInventoryItem(scanner);
		//for (auto formID : SlotList) {
		//	Notification(fmt::format("  item: {}", num2hex(formID)));
		//}
		return ItemList;		
	}

	std::unordered_map<int, RE::TESBoundObject*> GetArmorFormIDPairs(RE::TESObjectREFR* actor, bool isEquipped)
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

		if (isEquipped) {
			actor->ForEachEquippedItem(scanner);
		} else {
			actor->ForEachInventoryItem(scanner);
		}
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