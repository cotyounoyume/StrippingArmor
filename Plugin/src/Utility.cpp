#include "Utility.h"

namespace Utility
{
	void ExecuteCommandString(std::string command, std::string displayPrefix)
	{
		ExecuteCommand(0, command.data());
		Info(displayPrefix + command);
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

	void Notification(std::string message, bool flg)
	{
		bool NotificationOn = flg;
		bool InfoOn = false;
		//NotificationOn = true;
		InfoOn = true;
		if (NotificationOn)
			RE::DebugNotification(message.c_str());
		if (InfoOn)
			logger::info("{}", message);
	}

	void Info(std::string message)
	{
		Notification(message);
	}

	void Notify(std::string message)
	{
		Notification(message, true);
	}

	bool InGameScene()
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		if (player == nullptr)
			return false;
		if(RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu"))
			return true;
		if (RE::UI::GetSingleton()->IsMenuOpen("Console"))
			return false;
		if (IsMenuOthersOpen())
			return false;

		return true;
	}

	bool IsMenuOthersOpen()
	{
		if (IsMenuForSystemOpen())
			return true;
		if (IsMenuForTerminalOpen())
			return true;
		if (IsMenuForTradeOpen())
			return true;
		return false;
	}

	bool IsMenuInGameOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("WorkshopMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("CursorMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("MonocleMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu")
			)
			return true;
		return false;
	}

	bool IsMenuForSystemOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("PauseMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("Console") 
			|| RE::UI::GetSingleton()->IsMenuOpen("FaderMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("LoadingMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("MainMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("GalaxyStarMapMenu")
			)
			return true;
		return false;
	}

	bool IsMenuForTradeOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("ContainerMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("BarterMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("SpaceshipEditorMenu") 
			)
			return true;
		return false;
	}

	bool IsMenuForTerminalOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("MissionBoard") 
			|| RE::UI::GetSingleton()->IsMenuOpen("DataMenu") 
			|| RE::UI::GetSingleton()->IsMenuOpen("GenesisTerminalMenu") 
		)
			return true;
		return false;
	}

	bool IsMenuOpen(std::string MenuName, bool DialogueOnly)
	{
		if (MenuName != "DialogueMenu" || !DialogueOnly)
			return RE::UI::GetSingleton()->IsMenuOpen(MenuName);

		//DialogueMenuRoute
		bool result = RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu");
		if (IsMenuOthersOpen())
			result = false;
		return result;
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

	RE::BGSKeyword* GetKeywordFromID(int formID, int modIndex)
	{
		int  fullFormID = modIndex * (1 << 24) + formID;
		//Info(format("GetKeywordFromID: TEST: idDec:{}, idHex:{}", fullFormID, Utility::num2hex(fullFormID)));

		auto form = RE::TESForm::LookupByID<RE::BGSKeyword>(fullFormID);
		if (!form) {
			Info(format("ERROR: can't find id:{}", Utility::num2hex(fullFormID)));
			return nullptr;
		}
		return form;
	}

	int GetModIndex(int formID, std::string name)
	{
		std::string myName = ToLower(name);

		for (int i = 0; i < 256; i++) {
			int  num = formID + (1 << 24) * i;
			auto numstr = Utility::num2hex(num);
			auto form = RE::TESForm::LookupByID(num);
			if (!form)
				continue;
			std::string editorID = ToLower(std::string(form->GetFormEditorID()));
			if (editorID == myName) {
				Info(format("GetModIndex: myModIndex:{}, myName:{}", i, myName));
				return i;
			}
		}
		return 0;
	}

	std::string ToLower(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
			return std::tolower(c);
		});
		return str;
	}

	RE::BGSKeyword* GetKeywordFromString(std::string editorID)
	{
		auto form = RE::TESForm::LookupByEditorID(editorID.c_str());
		//auto form2 = RE::TESForm::LookupByEditorID<RE::BGSKeyword*>(editorID.c_str());
		auto form3 = RE::TESForm::LookupByID<RE::BGSKeyword>(0x02000827);
		if (form3) {
			Info(fmt::format("form3 is {}", form3->GetFormEditorID()));
			Info(fmt::format("form3 is {}", form3->GetObjectTypeName()));
		}
		//auto form4 = RE::TESForm::LookupByID<RE::BGSKeyword*>(0x02000827);
		//if (!form2) {
		//	Info(format("form2 is null"));
		//}
		if (!form3) {
			Info(fmt::format("form3 is null"));
		}
		//if (!form4) {
		//	Info(format("form4 is null"));
		//}
		if (!form) {
			Info(fmt::format("keyword({}) was not found.", editorID));
			return nullptr;
		}
		auto keyword = static_cast<RE::BGSKeyword*>(form);
		if (!keyword) {
			Info(fmt::format("keyword({}) can't dynamic_cast.", editorID));
			return nullptr;
		}
		Info(fmt::format("keyword({}) is {}", editorID, keyword->GetFormEditorID()));
		return keyword;
	}

	RE::TESBoundObject* GetArmorFromString(std::string editorID)
	{
		auto form = RE::TESForm::LookupByEditorID(editorID.c_str());
		if (!form) {
			Info(format("armor({}) was not found.", editorID));
			return nullptr;
		}
		auto armor = static_cast<RE::TESBoundObject*>(form);
		return armor;
	}

	RE::TESBoundObject* GetArmorFromID(int formID, int modIndex)
	{
		int  fullFormID = modIndex * (1 << 24) + formID;
		//Info(format("GetArmorFromID: TEST: idDec:{}, idHex:{}", fullFormID, Utility::num2hex(fullFormID)));
		auto form = RE::TESForm::LookupByID(fullFormID);
		if (!form) {
			Info(format("ERROR: can't find id:{}", Utility::num2hex(fullFormID)));
			return nullptr;
		}
		auto form2 = static_cast<RE::TESBoundObject*>(form);
		if (!form2) {
			Info(format("ERROR: can't cast id:{}, {}", Utility::num2hex(fullFormID), form2->GetFormEditorID()));
			return nullptr;
		}
		return form2;
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
		Info(fmt::format("  PrintArmorStacks2: Start:  {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
		auto armorMap = CollectInventoryItems(actor, "ARMOR");
		for (auto itr = armorMap.begin(); itr != armorMap.end(); ++itr) {
			Info(fmt::format("  item: {}, count:{}, isEquipped:{}", itr->first->GetFormEditorID(), itr->second, actor->IsObjectEquipped(itr->first))); 
		}
		Info(fmt::format("  PrintArmorStacks2: Finish: {}: {}", num2hex(actor->formID), actor->GetFormEditorID()));
	}

	int GetEquipmentStackCount(const RE::BGSInventoryItem& item)
	{
		int sum = 0;
		if (item.object->IsArmor()) {
			for (int i = 0; i < item.stacks.size(); i++) {
				//Info(fmt::format("  item: {}: {}: stacks[{}].unk10={}", num2hex(item.object->formID), item.object->GetFormEditorID(), i, item.stacks[i].unk10));
				sum += item.stacks[i].count;
				//sum += item.stacks[i].unk10;
			}
		}
		return sum;
	}

	std::unordered_map<RE::TESBoundObject*, std::string> GetArmorTypes(RE::TESObjectREFR* actor)
	{
		Info("GetArmorTypes Start:");
		ItemTypesForScanner.clear();
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				if (IsDummySuits(item)) {
					return RE::BSContainer::ForEachResult::kContinue;
				}
				ItemTypesForScanner[item.object] = GetArmorType(item);
			}

			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		Info("GetArmorTypes Finish:");
		return ItemTypesForScanner;
	}

	std::string GetArmorType(const RE::BGSInventoryItem& item)
	{
		std::string result = "";
		if (!item.object->IsArmor() || item.instanceData == nullptr || item.instanceData->GetKeywordData() == nullptr) {
			result = "Error";
			Info(fmt::format("    GetArmorType: item: {}: result:{}", item.object->GetFormEditorID(), result));
			return result;
		} 

		if (item.object->GetFilledSlots() != 0) {
			result = "Cloth";
			Info(fmt::format("    GetArmorType: item: {}: result:{}", item.object->GetFormEditorID(), result));
			return result;
		}

		if (item.instanceData->GetKeywordData()->ContainsKeywordString("ArmorTypeSpacesuitBody")) {
			result = "Spacesuit";
		} else if (item.instanceData->GetKeywordData()->ContainsKeywordString("ArmorTypeSpacesuitBackpack")) {
			result = "Backpack";
		} else {
			result = "Helmet";
		}
		Info(fmt::format("    GetArmorType: item: {}: result:{}", item.object->GetFormEditorID(), result));

		return result;
	}

	std::unordered_map<RE::TESBoundObject*, int> CollectEquipItems(RE::TESObjectREFR* actor, std::string itemType)
	{
		//Info("CollectInventoryItems Start:");
		ItemType = itemType;
		ItemForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			//Info(fmt::format("item: {}: {}", num2hex(item.object->formID), item.object->GetFormEditorID()));
			bool isTarget = false;
			isTarget = (ItemType == "ALL") ? true : isTarget;
			isTarget = (ItemType == "ARMOR" && item.object->IsArmor()) ? true : isTarget;
			isTarget = (ItemType == "WEAPON" && item.object->IsWeapon()) ? true : isTarget;
			isTarget = (ItemType == "BOUNDOBJECT" && item.object->IsBoundObject()) ? true : isTarget;
			isTarget = (IsDummySuits(item)) ? false : isTarget;
			if (isTarget)
				ItemForScanner[item.object] += GetEquipmentStackCount(item);
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		//Info("CollectInventoryItems Finish:");
		return ItemForScanner;
	}

	std::unordered_map<RE::TESBoundObject*, int> CollectInventoryItems(RE::TESObjectREFR* actor, std::string itemType)
	{
		//Info("CollectInventoryItems Start:");
		ItemType = itemType;
		ItemForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			//Info(fmt::format("item: {}: {}", num2hex(item.object->formID), item.object->GetFormEditorID())); 
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
		//Info("CollectInventoryItems Finish:");
		return ItemForScanner;
	}

	bool IsDummySuits(const RE::BGSInventoryItem& item)
	{
		std::string sFormIDSuffix = num2hex(item.object->formID, false, true).substr(2,6);
		return sFormIDSuffix == "000805" || sFormIDSuffix == "000807" || sFormIDSuffix == "000808" || sFormIDSuffix == "000809";
	}

	bool HasDontStripKeyword(const RE::BGSInventoryItem& item)
	{
		Info(fmt::format("HasDontStripKeyword: start: {}", item.object->GetFormEditorID()));
		if (item.instanceData == nullptr)
			return false;
		auto keywordData = item.instanceData->GetKeywordData();
		Info("HasDontStripKeyword: p1");
		if (keywordData == nullptr)
			return false;
		auto keywords = keywordData->keywords;
		Info("HasDontStripKeyword: p1.5");
		if (keywords.empty())
			Info("HasDontStripKeyword: p1.6: this keyword form empty");
		else
			Info("HasDontStripKeyword: p1.6: this keyword form NOT empty");

		Info(fmt::format("item:{}, keywordData->GetNumKeywords:{}", item.object->GetFormEditorID(), keywordData->GetNumKeywords()));
		bool flg = keywordData->ContainsKeywordString("SADontStripThis");
		Info("HasDontStripKeyword: p2");
		return flg;
	}

	std::string GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn)
	{
		std::string result = "";
		std::string quote = quoteOn ? "\"" : "";
		for (int formID : list) {
			std::string sFormID = (hexOn) ? num2hex(formID) : fmt::format("{}", formID);
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

	uint32_t hex2num(std::string formID)
	{
		int intValue;
		try {
			intValue = std::stoi(formID, 0, 16);  // ベース16（16進数）で変換
			return intValue;
		} catch (const std::invalid_argument& ia) {
			return 0;
		} catch (const std::out_of_range& oor) {
			return 0;
		}
		return 0;
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
	//	for (const RE::BGSInventoryItem& item : CollectInventoryItems(actor, "All")) {
	//		bool any0 = item.flags.any(RE::BGSInventoryItem::Flag::kEquipStateLocked);
	//		bool any1 = item.flags.any(RE::BGSInventoryItem::Flag::kInvShouldEquip);
	//		bool any2 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex1);
	//		bool any3 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex2);
	//		bool any4 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotIndex3);
	//		bool any5 = item.flags.any(RE::BGSInventoryItem::Flag::kSlotMask);
	//		bool any6 = item.flags.any(RE::BGSInventoryItem::Flag::kTemporary);
	//		auto unk24 = item.unk24;

	//		Notification(fmt::format("item:{}, {}", num2hex(item.object->formID), item.object->GetFormEditorID()));
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