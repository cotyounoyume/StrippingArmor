#include "Config.h"

namespace Config
{
	void SetTraditionalLootingOnly()
	{
		Info(fmt::format("StrippingArmor: TraditionalLootingOnlyOn. Disable other functions."));
		StrippingKeyOn = false;
		EffectOn = false;
		EffectForCorpseOn = false;
		AlternativeClothOn = false;
		UseStrippingKeyToCorpse = false;
		ChangingAppearanceOfCorpse = false;
		EnableDroppingItemsOn = false;
		CanStealDroppedItemOn = false;
		DebugExecuteToAllActorsOn = false;
		DebugExecuteToCrossRefActorForcedOn = false;
		TimePerFrame = 50;
	}

	void ReadIni()
	{
		constexpr auto        pathString = L"Data/SFSE/Plugins/StrippingArmor.ini";
		std::filesystem::path path = std::filesystem::path(pathString);
		if (!std::filesystem::exists(path))
			return;

		Info(fmt::format("StrippingArmor: path={}", path.string()));
		auto config = toml::parse_file(path.string());

		std::string traditionalLootingOnlyOn = config["Config"]["TraditionalLootingOnly"].value_or("");
		if (traditionalLootingOnlyOn == "")
			return;
		if (traditionalLootingOnlyOn == "True" || traditionalLootingOnlyOn == "true") {
			SetTraditionalLootingOnly();
			return;
		}

		std::string key = config["Config"]["StrippingKey"].value_or("");
		if (key != "") {
			StrippingKey = key;
			Info(fmt::format("StrippingArmor: key={}, keynum={}", key, key.c_str()[0]));
		}

		std::string strippingKeyOn = config["Config"]["StrippingKeyOn"].value_or("");
		if (strippingKeyOn != "") {
			StrippingKeyOn = (strippingKeyOn == "True" || strippingKeyOn == "true");
			Info(fmt::format("StrippingArmor: strippingKeyOn={}, StrippingKeyOn={}", strippingKeyOn, StrippingKeyOn));
		}

		std::string effect = config["Config"]["EffectShaderForStrippingOn"].value_or("");
		if (effect != "") {
			EffectOn = (effect == "True" || effect == "true");
			Info(fmt::format("StrippingArmor: effect={}, EffectOn={}", effect, EffectOn));
		}

		std::string effectForCorpse = config["Config"]["EffectShaderForChangingCorpseOn"].value_or("");
		if (effectForCorpse != "") {
			EffectForCorpseOn = (effectForCorpse == "True" || effectForCorpse == "true");
			Info(fmt::format("StrippingArmor: effectForCorpse={}, EffectForCorpse={}", effect, EffectForCorpseOn));
		}

		std::string effectFormID = config["Config"]["EffectShaderFormIDForStripping"].value_or("");
		if (effectFormID != "") {
			EffectFormID = effectFormID;
			Info(fmt::format("StrippingArmor: effectFormID={}, EffectFormID={}", effectFormID, EffectFormID));
		}

		std::string alternativeCloth = config["Config"]["AlternativeClothOn"].value_or("");
		if (alternativeCloth != "") {
			AlternativeClothOn = (alternativeCloth == "True" || alternativeCloth == "true");
			Info(fmt::format("StrippingArmor: alternativeCloth={}, AlternativeClothOn={}", alternativeCloth, AlternativeClothOn));
		}

		std::string useStrippingKeyToCorpse = config["Config"]["UseStrippingKeyToCorpse"].value_or("");
		if (useStrippingKeyToCorpse != "") {
			UseStrippingKeyToCorpse = (useStrippingKeyToCorpse == "True" || useStrippingKeyToCorpse == "true");
			Info(fmt::format("StrippingArmor: useStrippingKeyToCorpse={}, UseStrippingKeyToCorpse={}", useStrippingKeyToCorpse, UseStrippingKeyToCorpse));
		}

		std::string changingAppearanceOfCorpse = config["Config"]["ChangingAppearanceOfCorpse"].value_or("");
		if (changingAppearanceOfCorpse != "") {
			ChangingAppearanceOfCorpse = (changingAppearanceOfCorpse == "True" || changingAppearanceOfCorpse == "true");
			Info(fmt::format("StrippingArmor: changingAppearanceOfCorpse={}, ChangingAppearanceOfCorpse={}", changingAppearanceOfCorpse, ChangingAppearanceOfCorpse));
		}

		std::string enableDroppingItemsOn = config["Config"]["EnableDroppingItems"].value_or("");
		if (enableDroppingItemsOn != "") {
			EnableDroppingItemsOn = (enableDroppingItemsOn == "True" || enableDroppingItemsOn == "true");
			Info(fmt::format("StrippingArmor: enableDroppingItemsOn={}, EnableDroppingItemsOn={}", enableDroppingItemsOn, EnableDroppingItemsOn));
		}

		std::string canStealDroppedItemOn = config["Config"]["CanStealDroppedItem"].value_or("");
		if (canStealDroppedItemOn != "") {
			CanStealDroppedItemOn = (canStealDroppedItemOn == "True" || canStealDroppedItemOn == "true");
			Info(fmt::format("StrippingArmor: canStealDroppedItemOn={}, CanStealDroppedItemOn={}", canStealDroppedItemOn, CanStealDroppedItemOn));
		}

		std::string conditionTalkingOn = config["Config"]["ConditionTalking"].value_or("");
		if (conditionTalkingOn != "") {
			ConditionTalkingOn = (conditionTalkingOn == "True" || conditionTalkingOn == "true");
			Info(fmt::format("StrippingArmor: conditionTalkingOn={}, ConditionTalkingOn={}", conditionTalkingOn, ConditionTalkingOn));
		}

		std::string conditionSleepingOn = config["Config"]["ConditionSleeping"].value_or("");
		if (conditionSleepingOn != "") {
			ConditionSleepingOn = (conditionSleepingOn == "True" || conditionSleepingOn == "true");
			Info(fmt::format("StrippingArmor: conditionSleepingOn={}, ConditionSleepingOn={}", conditionSleepingOn, ConditionSleepingOn));
		}

		std::string conditionUnconsciousOn = config["Config"]["ConditionUnconscious"].value_or("");
		if (conditionUnconsciousOn != "") {
			ConditionUnconsciousOn = (conditionUnconsciousOn == "True" || conditionUnconsciousOn == "true");
			Info(fmt::format("StrippingArmor: conditionUnconsciousOn={}, ConditionUnconsciousOn={}", conditionUnconsciousOn, ConditionUnconsciousOn));
		}

		std::string conditionIsCommandedOn = config["Config"]["ConditionIsCommanded"].value_or("");
		if (conditionIsCommandedOn != "") {
			ConditionIsCommandedOn = (conditionIsCommandedOn == "True" || conditionIsCommandedOn == "true");
			Info(fmt::format("StrippingArmor: conditionIsCommandedOn={}, ConditionIsCommandedOn={}", conditionIsCommandedOn, ConditionIsCommandedOn));
		}

		std::string conditionPickingPocketOn = config["Config"]["ConditionPickingPocket"].value_or("");
		if (conditionPickingPocketOn != "") {
			ConditionPickingPocketOn = (conditionPickingPocketOn == "True" || conditionPickingPocketOn == "true");
			Info(fmt::format("StrippingArmor: conditionPickingPocketOn={}, ConditionPickingPocketOn={}", conditionPickingPocketOn, ConditionPickingPocketOn));
		}

		std::string conditionBleedingOutOn = config["Config"]["ConditionBleedingOut"].value_or("");
		if (conditionBleedingOutOn != "") {
			ConditionBleedingOutOn = (conditionBleedingOutOn == "True" || conditionBleedingOutOn == "true");
			Info(fmt::format("StrippingArmor: conditionBleedingOutOn={}, ConditionBleedingOutOn={}", conditionBleedingOutOn, ConditionBleedingOutOn));
		}

		std::string debugExecuteToAllActorsOn = config["Config"]["DebugExecuteToAllActorsInSameCell"].value_or("");
		if (debugExecuteToAllActorsOn != "") {
			DebugExecuteToAllActorsOn = (debugExecuteToAllActorsOn == "True" || debugExecuteToAllActorsOn == "true");
			Info(fmt::format("StrippingArmor: debugExecuteToAllActorsOn={}, DebugExecuteToAllActorsOn={}", debugExecuteToAllActorsOn, DebugExecuteToAllActorsOn));
		}

		std::string debugExecuteToCrossRefActorForcedOn = config["Config"]["DebugExecuteToCrossRefActorForced"].value_or("");
		if (debugExecuteToCrossRefActorForcedOn != "") {
			DebugExecuteToCrossRefActorForcedOn = (debugExecuteToCrossRefActorForcedOn == "True" || debugExecuteToCrossRefActorForcedOn == "true");
			Info(fmt::format("StrippingArmor: debugExecuteToCrossRefActorForcedOn={}, DebugExecuteToCrossRefActorForcedOn={}", debugExecuteToCrossRefActorForcedOn, DebugExecuteToCrossRefActorForcedOn));
		}

		std::string perfectTouchOn = config["Config"]["PerfectTouch"].value_or("");
		if (perfectTouchOn != "") {
			PerfectTouchOn = (perfectTouchOn == "True" || perfectTouchOn == "true");
			Info(fmt::format("StrippingArmor: perfectTouchOn={}, PerfectTouchOn={}", perfectTouchOn, PerfectTouchOn));
		}

		std::string corpseTimer = config["Config"]["CorpseTimer"].value_or("");
		if (corpseTimer == "") {
			CorpseTimer = 5;
		}
		try {
			CorpseTimer = stoi(corpseTimer);
		} catch (std::exception const& e) {
			Info(fmt::format("StrippingArmor: Exception in Config.cpp: corpseTimer is not number:{}", corpseTimer));
			CorpseTimer = 5;
		}
		Info(fmt::format("StrippingArmor: corpseTimer={}, CorpseTimer={}", corpseTimer, CorpseTimer));

		std::string repickTimer = config["Config"]["TakingBackEquipmentTimer"].value_or("");
		if (repickTimer == "") {
			RePickTimer = 5;
		}
		try {
			RePickTimer = stoi(repickTimer);
		} catch (std::exception const& e) {
			Info(fmt::format("StrippingArmor: Exception in Config.cpp: repickTimer is not number:{}", repickTimer));
			RePickTimer = 5;
		}
		Info(fmt::format("StrippingArmor: repickTimer={}, RePickTimer={}", repickTimer, RePickTimer));

		std::string timePerFrame = config["Config"]["TimePerFrame"].value_or("");
		if (timePerFrame == "") {
			TimePerFrame = 50;
		}
		try {
			TimePerFrame = stoi(timePerFrame);
		} catch (std::exception const& e) {
			Info(fmt::format("StrippingArmor: Exception in Config.cpp: timePerFrame is not number:{}", timePerFrame));
			TimePerFrame = 50;
		}
		Info(fmt::format("StrippingArmor: timePerFrame={}, TimePerFrame={}", timePerFrame, TimePerFrame));


		std::string ppHat = config["Config"]["PickpocketLevelHat"].value_or("");
		if (ppHat != "") {
			try {
				PickpocketHat = stoi(ppHat);
			} catch (std::exception const& e) {
				Info(fmt::format("StrippingArmor: Exception in Config.cpp: ppHat is not number:{}", ppHat));
			}
		}
		Info(fmt::format("StrippingArmor: ppHat={}, PickpocketHat={}", ppHat, PickpocketHat));


		std::string ppCloth = config["Config"]["PickpocketLevelCloth"].value_or("");
		if (ppCloth != "") {
			try {
				PickpocketCloth = stoi(ppCloth);
			} catch (std::exception const& e) {
				Info(fmt::format("StrippingArmor: Exception in Config.cpp: ppHat is not number:{}", ppCloth));
			}
		}
		Info(fmt::format("StrippingArmor: ppCloth={}, PickpocketCloth={}", ppCloth, PickpocketCloth));

		std::string ppHelmet = config["Config"]["PickpocketLevelHelmet"].value_or("");
		if (ppHelmet != "") {
			try {
				PickpocketHelmet = stoi(ppHelmet);
			} catch (std::exception const& e) {
				Info(fmt::format("StrippingArmor: Exception in Config.cpp: ppHelmet is not number:{}", ppHelmet));
			}
		}
		Info(fmt::format("StrippingArmor: ppHelmet={}, PickpocketHelmet={}", ppHelmet, PickpocketHelmet));

		std::string ppBackpack = config["Config"]["PickpocketLevelBackpack"].value_or("");
		if (ppBackpack != "") {
			try {
				PickpocketBackpack = stoi(ppBackpack);
			} catch (std::exception const& e) {
				Info(fmt::format("StrippingArmor: Exception in Config.cpp: ppBackpack is not number:{}", ppBackpack));
			}
		}
		Info(fmt::format("StrippingArmor: ppBackpack={}, PickpocketBackpack={}", ppBackpack, PickpocketBackpack));

		std::string ppSpacesuit = config["Config"]["PickpocketLevelSpacesuit"].value_or("");
		if (ppSpacesuit != "") {
			try {
				PickpocketSpacesuit = stoi(ppSpacesuit);
			} catch (std::exception const& e) {
				Info(fmt::format("StrippingArmor: Exception in Config.cpp: ppSpacesuit is not number:{}", ppSpacesuit));
			}
		}
		Info(fmt::format("StrippingArmor: ppSpacesuit={}, PickpocketSpacesuit={}", ppSpacesuit, PickpocketSpacesuit));
	}

	std::string GetStrippingKey()
	{
		return StrippingKey;
	}

	char GetStrippingKeyNumber()
	{
		return StrippingKey.c_str()[0];
	}

	bool GetEffectEnabled()
	{
		return EffectOn;
	}

	bool GetDebugExecuteToAllActorsOn()
	{
		return DebugExecuteToAllActorsOn;
	}

	bool GetDebugExecuteToCrossRefActorForcedOn()
	{
		return DebugExecuteToCrossRefActorForcedOn;
	}

	bool GetStrippingKeyOn()
	{
		return StrippingKeyOn;
	}

	bool GetEffectForCorpseEnabled()
	{
		return EffectForCorpseOn;
	}

	std::string GetEffectFormID()
	{
		return EffectFormID;
	}

	bool GetAlternativeClothEnabled()
	{
		return AlternativeClothOn;
	}

	bool GetChangingAppearanceOfCorpseEnabled()
	{
		return ChangingAppearanceOfCorpse;
	}

	int GetCorpseTimer()
	{
		return CorpseTimer;
	}

	int GetRePickTimer()
	{
		return RePickTimer;
	}

	bool GetUseStrippingKeyToCorpse()
	{
		return UseStrippingKeyToCorpse;
	}

	bool GetEnableDroppingItemsOn()
	{
		return EnableDroppingItemsOn;
	}

	bool GetConditionTalkingOn()
	{
		return ConditionTalkingOn;
	}

	bool GetConditionSleepingOn()
	{
		return ConditionSleepingOn;
	}

	bool GetConditionUnconsciousOn()
	{
		return ConditionUnconsciousOn;
	}

	bool GetConditionIsCommandedOn()
	{
		return ConditionIsCommandedOn;
	}

	bool GetConditionPickingPocketOn()
	{
		return ConditionPickingPocketOn;
	}

	bool GetConditionBleedingOutOn()
	{
		return ConditionBleedingOutOn;
	}

	bool GetPerfectTouchOn()
	{
		return PerfectTouchOn;
	}

	bool GetCanStealDroppedItemOn()
	{
		return CanStealDroppedItemOn;
	}

	int GetTimePerFrame()
	{
		return TimePerFrame;
	}

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

	int GetPPHat()
	{
		return PickpocketHat;
	}

	int GetPPCloth()
	{
		return PickpocketCloth;
	}

	int GetPPHelmet()
	{
		return PickpocketHelmet;
	}

	int GetPPBackpack()
	{
		return PickpocketBackpack;
	}

	int GetPPSpacesuit()
	{
		return PickpocketSpacesuit;
	}
}
