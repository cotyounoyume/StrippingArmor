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

	std::vector<RE::TESBoundObject*> GetLootedArmors(RE::TESObjectREFR* actor)
	{
		std::vector<RE::TESBoundObject*> result;
		if (actor == nullptr)
			return result;
		
		auto armorMap = CollectInventoryItems(actor, "ARMOR");
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			if (itr->second != 1 || !actor->IsObjectEquipped(itr->first))
				continue;
			result.push_back(itr->first);
		}
		return result;
	}

	void ReadyForLoot2(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		Notification(fmt::format("ReadyForLoot2: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		auto armorMap = CollectInventoryItems(actor, "ARMOR");
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

	void PrintArmorStacks2(RE::TESObjectREFR* actor)
	{
		if (actor == nullptr)
			return;
		Notification(fmt::format("  PrintArmorStacks2: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		auto armorMap = CollectInventoryItems(actor, "ARMOR");
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			Notification(fmt::format("  item: {}, count:{}, isEquipped:{}", itr->first->GetFormEditorID(), itr->second, actor->IsObjectEquipped(itr->first))); 
		}
		Notification(fmt::format("  PrintArmorStacks2: Finish: {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
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

	std::unordered_map<RE::TESBoundObject*, std::string> GetArmorTypes(RE::TESObjectREFR* actor)
	{
		Notification("GetArmorTypes Start:");
		ItemTypesForScanner.clear();
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				if (IsDummySuits(item)) {
					return RE::BSContainer::ForEachResult::kContinue;
				}
				ItemTypesForScanner[item.object] = GetArmorType(item);
				//if (item.instanceData->GetKeywordData()->ContainsKeywordString("ArmorTypeSpacesuitBody")) {
				//	Notification(fmt::format("  item:{} is spacesuit", num2hex(item.object->formID)));
				//	ItemTypesForScanner[item.object] = "Spacesuit";
				//} else if (item.instanceData->GetKeywordData()->ContainsKeywordString("ArmorTypeSpacesuitBackpack")) {
				//	Notification(fmt::format("  item:{} is backpack", num2hex(item.object->formID)));
				//	ItemTypesForScanner[item.object] = "Backpack";
				//} else {
				//	ItemTypesForScanner[item.object] = "";
				//}
					//if (HasDontStripKeyword(item)) {
						//if (item.instanceData->GetKeywordData()->ContainsKeywordString("DontStripThis")) {
					//skip
				//} else if (item.instanceData->GetKeywordData()->ContainsKeywordString("ArmorTypeSpacesuitBody")) {
				//	Notification(fmt::format("  item:{} is spacesuit", num2hex(item.object->formID)));
				//	ItemTypesForScanner[item.object] = "Spacesuit";
				//} else if (item.instanceData->GetKeywordData()->ContainsKeywordString("ArmorTypeSpacesuitBackpack")) {
				//	Notification(fmt::format("  item:{} is backpack", num2hex(item.object->formID)));
				//	ItemTypesForScanner[item.object] = "Backpack";
				//} else {
			}

			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		//actor->ForEachInventoryItem(scanner);
		Notification("GetArmorTypes Finish:");
		return ItemTypesForScanner;
	}

	std::string GetArmorType(const RE::BGSInventoryItem& item)
	{
		std::string result = "";
		auto        slotNum = item.object->GetFilledSlots();
		auto        slotNum2 = item.object->GetFilledSlotsImpl();
		auto        name = item.object->GetFormEditorID();
		auto        objectTypeName = item.object->GetObjectTypeName();
		//auto        desirability = item.object->GetDesirability();
		//Notification(fmt::format("  item: {}: slotNum={}, slotNum2={}, objectTypeName={}, desirability={}", name, slotNum, slotNum2, objectTypeName, desirability));
		Notification(fmt::format("  item: {}: slotNum={}, slotNum2={}, objectTypeName={}", name, slotNum, slotNum2, objectTypeName));
		return result;
	}

	std::unordered_map<RE::TESBoundObject*, int> CollectInventoryItems(RE::TESObjectREFR* actor, std::string itemType)
	{
		Notification("CollectInventoryItems Start:");
		ItemType = itemType;
		ItemForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			Notification(fmt::format("item: {}: {}", num2hex(item.object->formID), item.object->GetFormEditorID())); 
			bool isTarget = false;
			isTarget = (ItemType == "ALL") ? true : isTarget;
			isTarget = (ItemType == "ARMOR" && item.object->IsArmor()) ? true : isTarget;
			isTarget = (ItemType == "WEAPON" && item.object->IsWeapon()) ? true : isTarget;
			isTarget = (ItemType == "BOUNDOBJECT" && item.object->IsBoundObject()) ? true : isTarget;
			isTarget = (IsDummySuits(item)) ? false : isTarget;
			if(isTarget)
				ItemForScanner[item.object] += GetEquipmentStackCount(item);
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachInventoryItem(scanner);
		Notification("CollectInventoryItems Finish:");
		return ItemForScanner;
	}

	bool IsDummySuits(const RE::BGSInventoryItem& item)
	{
		std::string sFormIDSuffix = num2hex(item.object->formID, false, true).substr(2,6);
		return sFormIDSuffix == "000805" || sFormIDSuffix == "000807" || sFormIDSuffix == "000808" || sFormIDSuffix == "000809";
	}

	bool HasDontStripKeyword(const RE::BGSInventoryItem& item)
	{
		Notification(fmt::format("HasDontStripKeyword: start: {}", item.object->GetFormEditorID()));
		if (item.instanceData == nullptr)
			return false;
		auto keywordData = item.instanceData->GetKeywordData();
		Notification("HasDontStripKeyword: p1");
		if (keywordData == nullptr)
			return false;
		auto keywords = keywordData->keywords;
		Notification("HasDontStripKeyword: p1.5");
		if (keywords.empty())
			Notification("HasDontStripKeyword: p1.6: this keyword form empty");
		else
			Notification("HasDontStripKeyword: p1.6: this keyword form NOT empty");

		Notification(fmt::format("item:{}, keywordData->GetNumKeywords:{}", item.object->GetFormEditorID(), keywordData->GetNumKeywords()));
		bool flg = keywordData->ContainsKeywordString("DontStripThis");
		Notification("HasDontStripKeyword: p2");
		return flg;
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

	std::string num2hex(uint32_t formID, bool prefixOn, bool paddingOn)
	{
		std::string result = paddingOn ? fmt::format("{:08x}", formID) : fmt::format("{:x}", formID);
		result = prefixOn ? "0x" + result : result;
		return result;
	}

	NumAndType::NumAndType(int _num, std::string _type)
	{
		num = _num;
		type = _type;
	}
	//void SurveyInventory(RE::TESObjectREFR* actor)
	//{
	//	if (actor == nullptr)
	//		return;
	//	int debugCount = 0;
	//	for (const RE::BGSInventoryItem& item : CollectInventoryItems(actor, "All")) {
	//		debugCount++;
	//		bool any0 = item.flags.any(RE::BGSInventoryItem::Flag::kEquipStateLocked);
	//		bool any1 = item.flags.any(RE::BGSInventoryItem::Flag::kInvShouldEquip);
	//		bool any2 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex1);
	//		bool any3 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex2);
	//		bool any4 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex3);
	//		bool any5 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotMask);
	//		bool any6 = item.flags.any(RE::BGSInventoryItem::Flag::kTemporary);
	//		auto unk24 = item.unk24;

	//		Notification(fmt::format("item:{}, {}", num2hex(item.object->formID), item.object->GetFormEditorID()));
	//		Notification(fmt::format("  debugCount\t:{}", debugCount));
	//		Notification(fmt::format("  kEquipStateLocked\t:{}", any0));
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
	//	}
	//}
}