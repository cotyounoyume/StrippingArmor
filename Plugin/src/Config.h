#pragma once
#include "Common.h"
//#include "winuser.h"

namespace Config
{
	inline bool                       EsmCheck = true;
	inline std::string                UniqueEditorID = "SAHasDummySuit";
	inline toml::v3::ex::parse_result            TomlConfig;
	inline std::vector<std::string>   Categories = {
        "GeneralMajor",
        "GeneralMinor",
        "CorpseVisualEffect",
        "Theft",
        "Debug",
	};
	inline bool tomlParseError = false;

	inline std::unordered_map<std::string, bool> SettingsBoolMapGeneralMajor = {
		{ "TraditionalLootingOnly", true },
		{ "StrippingKeyOn", true },
		{ "EffectShaderForStrippingOn", true },
		{ "UseStrippingKeyToCorpse", true },
		{ "EnableDroppingItems", true },
		{ "CanStealDroppedItem", false },
		{ "ToggleNormalOrOthers", false },		
	};
	inline std::unordered_map<std::string, bool> SettingsBoolMapGeneralMinor = {
		{ "AlternativeClothOn", false },
		{ "ConditionTalking", true },
		{ "ConditionSleeping", true },
		{ "ConditionUnconscious", true },
		{ "ConditionIsCommanded", true },
		{ "ConditionBleedingOut", true },
	};
	inline std::unordered_map<std::string, bool> SettingsBoolMapCorpseVisualEffect = {
		{ "ChangingAppearanceOfCorpse", true },
		{ "EffectShaderForChangingCorpseOn", true },
	};
	inline std::unordered_map<std::string, bool> SettingsBoolMapTheft = {
		{ "PerfectTouch", true },
		{ "ConditionPickingPocket", false },
	};
	inline std::unordered_map<std::string, bool> SettingsBoolMapDebug = {
		{ "DebugExecuteToAllActorsInSameCell", false },
		{ "DebugExecuteToCrossRefActorForced", false },
	};

	inline std::unordered_map<std::string, std::string> SettingsStringMapGeneralMajor = {
		{ "StrippingKey", "T" },
		{ "ToggleKey", "O" },
	};

	inline std::unordered_map<std::string, std::string> SettingsStringMapGeneralMinor = {
		{ "EffectShaderFormIDForStripping", "0026319b" },
	};

	inline std::unordered_map<std::string, std::string> SettingsStringMapCorpseVisualEffect = {};
	inline std::unordered_map<std::string, std::string> SettingsStringMapTheft = {};
	inline std::unordered_map<std::string, std::string> SettingsStringMapDebug = {};

	inline std::unordered_map<std::string, int> SettingsIntMapGeneralMajor = {};

	inline std::unordered_map<std::string, int> SettingsIntMapGeneralMinor = {
		{ "TimePerFrame", 50 },
		{ "TakingBackEquipmentTimer", 10 },
		{ "CellSearchRadius", 3},
	};

	inline std::unordered_map<std::string, int> SettingsIntMapCorpseVisualEffect = {
		{ "CorpseTimer", 3 },
	};

	inline std::unordered_map<std::string, int> SettingsIntMapTheft = {
		{ "PickpocketLevelHat", 1 },
		{ "PickpocketLevelHelmet", 1 },
		{ "PickpocketLevelCloth", 2 },
		{ "PickpocketLevelBackpack", 3 },
		{ "PickpocketLevelSpacesuit", 4 },
	};

	inline std::unordered_map<std::string, int> SettingsIntMapDebug = {
		{ "LogLevel", 0 },
		{ "CellSearchRadiusDebug", 20},
	};


	void        ReadIni();
	void        SetTraditionalLootingOnly();
	void        SetTomlPath();
	void        DumpSettings();
	int         GetLogLevel();
	std::string GetStrippingKey();
	std::string GetToggleKey();
	char        GetStrippingKeyNumber();
	char        GetToggleKeyNumber();
	bool        GetEffectEnabled();
	bool        GetStrippingKeyOn();
	bool        GetEffectForCorpseEnabled();
	std::string GetEffectFormID();
	bool        GetAlternativeClothEnabled();
	bool        GetChangingAppearanceOfCorpseEnabled();
	int         GetCorpseTimer();
	bool        GetUseStrippingKeyToCorpse();
	int         GetTimePerFrame();
	int         GetCellSearchRadius();
	int         GetCellSearchRadiusDebug();
	int         GetRePickTimer();
	bool        GetEnableDroppingItemsOn();
	bool        GetCanStealDroppedItemOn();
	bool        GetConditionTalkingOn();
	bool        GetConditionSleepingOn();
	bool        GetConditionUnconsciousOn();
	bool        GetConditionIsCommandedOn();
	bool        GetConditionPickingPocketOn();
	bool        GetConditionBleedingOutOn();
	bool        GetPerfectTouchOn();
	int         GetPPHat();
	int         GetPPCloth();
	int         GetPPHelmet();
	int         GetPPBackpack();
	int         GetPPSpacesuit();
	int         GetPPLevel(std::string type);
	bool        GetDebugExecuteToAllActorsOn();
	bool        GetDebugExecuteToCrossRefActorForcedOn();
	bool        GetTraditionalLootingOn();
	bool        GetToggleNormalOrOthersOn();
	void        EsmNotLoadCheck();


	std::unordered_map<std::string, bool>*        GetBoolMapByCategory(std::string category);
	std::unordered_map<std::string, std::string>* GetStringMapByCategory(std::string category);
	std::unordered_map<std::string, int>*         GetIntMapByCategory(std::string category);
	void                                          ReadConfigBool(std::string category, std::string name);
	void                                          ReadConfigString(std::string category, std::string name);
	void                                          ReadConfigInt(std::string category, std::string name);
}