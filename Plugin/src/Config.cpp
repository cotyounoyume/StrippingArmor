#include "Config.h"

namespace Config
{
	void SetTomlPath()
	{
		constexpr auto        pathString = L"Data/SFSE/Plugins/StrippingArmor.ini";
		std::filesystem::path path = std::filesystem::path(pathString);
		if (!std::filesystem::exists(path)) {
			Error(fmt::format("path:{} not found", path.string()));
			return;
		}

		Info(fmt::format("StrippingArmor: path={}", path.string()));
		TomlConfig = toml::parse_file(path.string());
	}

	void ReadIni()
	{
		SetTomlPath();

		DumpSettings();
		for (auto category : Categories) {
			auto boolMap = GetBoolMapByCategory(category);
			for (auto itr = (*boolMap).begin(); itr != (*boolMap).end(); ++itr) {
				ReadConfigBool(category, itr->first);
			}

			auto stringMap = GetStringMapByCategory(category);
			for (auto itr = (*stringMap).begin(); itr != (*stringMap).end(); ++itr) {
				ReadConfigString(category, itr->first);
			}

			auto intMap = GetIntMapByCategory(category);
			for (auto itr = (*intMap).begin(); itr != (*intMap).end(); ++itr) {
				ReadConfigInt(category, itr->first);
			}
		}

		DumpSettings();

		if (GetTraditionalLootingOn())
			SetTraditionalLootingOnly();

		U::SetLogLevel(GetLogLevel());
	}

	void DumpSettings()
	{
		for (auto category : Categories) {
			auto boolMap = GetBoolMapByCategory(category);
			for (auto itr = (*boolMap).begin(); itr != (*boolMap).end(); ++itr) {
				Info(fmt::format("Bool: category:{}, name:{}, value:{}", category, itr->first, itr->second));
			}

			auto stringMap = GetStringMapByCategory(category);
			for (auto itr = (*stringMap).begin(); itr != (*stringMap).end(); ++itr) {
				Info(fmt::format("String: category:{}, name:{}, value:{}", category, itr->first, itr->second));
			}

			auto intMap = GetIntMapByCategory(category);
			for (auto itr = (*intMap).begin(); itr != (*intMap).end(); ++itr) {
				Info(fmt::format("Int: category:{}, name:{}, value:{}", category, itr->first, itr->second));
			}
		}
	}

	void SetTraditionalLootingOnly()
	{
		Info(fmt::format("StrippingArmor: TraditionalLootingOnlyOn. Disable other functions."));
		SettingsBoolMapGeneralMajor["StrippingKeyOn"] = false;
		SettingsBoolMapGeneralMajor["EffectShaderForStrippingOn"] = false;
		SettingsBoolMapGeneralMajor["UseStrippingKeyToCorpse"] = false;
		SettingsBoolMapGeneralMajor["EnableDroppingItems"] = false;
		SettingsBoolMapGeneralMajor["CanStealDroppedItem"] = false;

		SettingsBoolMapGeneralMinor["AlternativeClothOn"] = false;
		SettingsIntMapGeneralMinor["TimePerFrame"] = 50;

		SettingsBoolMapGeneralMinor["DebugExecuteToAllActorsInSameCell"] = false;
		SettingsBoolMapGeneralMinor["DebugExecuteToCrossRefActorForced"] = false;

		SettingsBoolMapCorpseVisualEffect["ChangingAppearanceOfCorpse"] = false;
		SettingsBoolMapCorpseVisualEffect["EffectShaderForChangingCorpseOn"] = false;
	}

	//GeneralMajor
	bool GetTraditionalLootingOn() { return SettingsBoolMapGeneralMajor["TraditionalLootingOnly"]; }

	bool GetStrippingKeyOn() { return SettingsBoolMapGeneralMajor["StrippingKeyOn"]; }

	bool GetEffectEnabled() { return SettingsBoolMapGeneralMajor["EffectShaderForStrippingOn"]; }

	bool GetUseStrippingKeyToCorpse(){ return SettingsBoolMapGeneralMajor["UseStrippingKeyToCorpse"]; }

	bool GetEnableDroppingItemsOn() { return SettingsBoolMapGeneralMajor["EnableDroppingItems"]; }

	bool GetCanStealDroppedItemOn() { return SettingsBoolMapGeneralMajor["CanStealDroppedItem"]; }

	//GeneralMinor
	bool GetAlternativeClothEnabled() { return SettingsBoolMapGeneralMinor["AlternativeClothOn"]; }

	bool GetConditionTalkingOn() { return SettingsBoolMapGeneralMinor["ConditionTalking"]; }

	bool GetConditionSleepingOn() { return SettingsBoolMapGeneralMinor["ConditionSleeping"]; }

	bool GetConditionUnconsciousOn() { return SettingsBoolMapGeneralMinor["ConditionUnconscious"]; }

	bool GetConditionIsCommandedOn() { return SettingsBoolMapGeneralMinor["ConditionIsCommanded"]; }

	bool GetConditionBleedingOutOn() { return SettingsBoolMapGeneralMinor["ConditionBleedingOut"]; }

	//CorpseVisualEffect
	bool GetChangingAppearanceOfCorpseEnabled() { return SettingsBoolMapCorpseVisualEffect["ChangingAppearanceOfCorpse"]; }

	bool GetEffectForCorpseEnabled() { return SettingsBoolMapCorpseVisualEffect["EffectShaderForChangingCorpseOn"]; }

	//Theft
	bool GetPerfectTouchOn() { return SettingsBoolMapTheft["PerfectTouch"]; }

	bool GetConditionPickingPocketOn() { return SettingsBoolMapTheft["ConditionPickingPocket"]; }

	//Debug
	bool GetDebugExecuteToAllActorsOn() { return SettingsBoolMapDebug["DebugExecuteToAllActorsInSameCell"]; }

	bool GetDebugExecuteToCrossRefActorForcedOn() { return SettingsBoolMapDebug["DebugExecuteToCrossRefActorForced"]; }

	std::string GetStrippingKey() { return SettingsStringMapGeneralMajor["StrippingKey"]; }

	char GetStrippingKeyNumber()
	{ 
		return SettingsStringMapGeneralMajor["StrippingKey"].c_str()[0];
	}

	std::string GetEffectFormID() { return SettingsStringMapGeneralMinor["EffectShaderFormIDForStripping"]; }
	
	int GetTimePerFrame() { return SettingsIntMapGeneralMinor["TimePerFrame"]; }

	int GetRePickTimer() { return SettingsIntMapGeneralMinor["TakingBackEquipmentTimer"]; }

	int GetCorpseTimer() { return SettingsIntMapCorpseVisualEffect["CorpseTimer"]; }

	int GetPPLevel(std::string type)
	{
		if (type == "Hat")
			return GetPPHat();
		else if (type == "Cloth")
			return GetPPCloth();
		else if (type == "Helmet")
			return GetPPHelmet();
		else if (type == "Backpack")
			return GetPPBackpack();
		else if (type == "Spacesuit")
			return GetPPSpacesuit();
		return 99;
	}

	int GetPPHat() { return SettingsIntMapTheft["PickpocketLevelHat"]; }

	int GetPPCloth() { return SettingsIntMapTheft["PickpocketLevelCloth"]; }

	int GetPPHelmet() { return SettingsIntMapTheft["PickpocketLevelHelmet"]; }

	int GetPPBackpack() { return SettingsIntMapTheft["PickpocketLevelBackpack"]; }

	int GetPPSpacesuit() { return SettingsIntMapTheft["PickpocketLevelSpacesuit"]; }

	int GetLogLevel() { return SettingsIntMapDebug["LogLevel"]; }

	std::unordered_map<std::string, bool>* GetBoolMapByCategory(std::string category)
	{
		if (category == "GeneralMajor")
			return &SettingsBoolMapGeneralMajor;
		else if (category == "GeneralMinor")
			return &SettingsBoolMapGeneralMinor;
		else if (category == "CorpseVisualEffect")
			return &SettingsBoolMapCorpseVisualEffect;
		else if (category == "Theft")
			return &SettingsBoolMapTheft;
		else if (category == "Debug")
			return &SettingsBoolMapDebug;

		std::unordered_map<std::string, bool> result;
		return &result;
	}

	std::unordered_map<std::string, std::string>* GetStringMapByCategory(std::string category)
	{
		if (category == "GeneralMajor")
			return &SettingsStringMapGeneralMajor;
		else if (category == "GeneralMinor")
			return &SettingsStringMapGeneralMinor;
		else if (category == "CorpseVisualEffect")
			return &SettingsStringMapCorpseVisualEffect;
		else if (category == "Theft")
			return &SettingsStringMapTheft;
		else if (category == "Debug")
			return &SettingsStringMapDebug;

		std::unordered_map<std::string, std::string> result;
		return &result;
	}

	std::unordered_map<std::string, int>* GetIntMapByCategory(std::string category)
	{
		if (category == "GeneralMajor")
			return &SettingsIntMapGeneralMajor;
		else if (category == "GeneralMinor")
			return &SettingsIntMapGeneralMinor;
		else if (category == "CorpseVisualEffect")
			return &SettingsIntMapCorpseVisualEffect;
		else if (category == "Theft")
			return &SettingsIntMapTheft;
		else if (category == "Debug")
			return &SettingsIntMapDebug;

		std::unordered_map<std::string, int> result;
		return &result;
	}

	void ReadConfigBool(std::string category, std::string name)
	{
		auto        map = GetBoolMapByCategory(category);
		std::string value = TomlConfig[category][name].value_or("");

		if (value != "") {
			(*map)[name] = (value == "True" || value == "true");
			Debug(fmt::format("StrippingArmor: Category:{}, SettingName:{}, ConfigFileValue:{}, Settings:{}", category, name, value, (*map)[name]));
		}
	}

	void ReadConfigString(std::string category, std::string name)
	{
		auto        map = GetStringMapByCategory(category);
		std::string value = TomlConfig[category][name].value_or("");

		if (value != "") {
			(*map)[name] = value;
			Debug(fmt::format("StrippingArmor: Category:{}, SettingName:{}, ConfigFileValue:{}, Settings:{}", category, name, value, (*map)[name]));
		}
	}

	void ReadConfigInt(std::string category, std::string name)
	{
		auto        map = GetIntMapByCategory(category);
		std::string value = TomlConfig[category][name].value_or("");
		if (value != "") {
			try {
				(*map)[name] = stoi(value);
				Debug(fmt::format("StrippingArmor: Category:{}, SettingName:{}, ConfigFileValue:{}, Settings:{}", category, name, value, (*map)[name]));
			} catch (std::exception const& e) {
				Error(fmt::format("can't parse value:{} to int in Category:{}, SettingName:{}", value, category, name));
			}
		}
	}
}