#include "Config.h"

namespace Config
{
	void ReadIni()
	{
		constexpr auto        pathString = L"Data/SFSE/Plugins/StrippingArmor.ini";
		std::filesystem::path path = std::filesystem::path(pathString);
		if (!std::filesystem::exists(path))
			return;

		Info(fmt::format("StrippingArmor: path={}", path.string()));
		auto config = toml::parse_file(path.string());

		std::string key = config["Config"]["StrippingKey"].value_or("");
		if (key == "")
			return;
		StrippingKey = key;
		Info(fmt::format("StrippingArmor: key={}, keynum={}", key, key.c_str()[0]));

		std::string strippingKeyOn = config["Config"]["StrippingKeyOn"].value_or("");
		if (strippingKeyOn == "")
			return;
		StrippingKeyOn = (strippingKeyOn == "True" || strippingKeyOn == "true");
		Info(fmt::format("StrippingArmor: strippingKeyOn={}, StrippingKeyOn={}", strippingKeyOn, StrippingKeyOn));

		std::string effect = config["Config"]["EffectShaderForStrippingOn"].value_or("");
		if (effect == "")
			return;
		EffectOn = (effect == "True" || effect == "true");
		Info(fmt::format("StrippingArmor: effect={}, EffectOn={}", effect, EffectOn));

		std::string effectForCorpse = config["Config"]["EffectShaderForChangingCorpseOn"].value_or("");
		if (effectForCorpse == "")
			return;
		EffectForCorpseOn = (effectForCorpse == "True" || effectForCorpse == "true");
		Info(fmt::format("StrippingArmor: effectForCorpse={}, EffectForCorpse={}", effect, EffectForCorpseOn));

		std::string effectFormID = config["Config"]["EffectShaderFormIDForStripping"].value_or("");
		if (effectFormID == "")
			return;
		EffectFormID = effectFormID;
		Info(fmt::format("StrippingArmor: effectFormID={}, EffectFormID={}", effectFormID, EffectFormID));

		std::string alternativeCloth = config["Config"]["AlternativeClothOn"].value_or("");
		if (alternativeCloth == "")
			return;
		AlternativeClothOn = (alternativeCloth == "True" || alternativeCloth == "true");
		Info(fmt::format("StrippingArmor: alternativeCloth={}, AlternativeClothOn={}", alternativeCloth, AlternativeClothOn));

		std::string useStrippingKeyToCorpse = config["Config"]["UseStrippingKeyToCorpse"].value_or("");
		if (useStrippingKeyToCorpse == "")
			return;
		UseStrippingKeyToCorpse = (useStrippingKeyToCorpse == "True" || useStrippingKeyToCorpse == "true");
		Info(fmt::format("StrippingArmor: useStrippingKeyToCorpse={}, UseStrippingKeyToCorpse={}", useStrippingKeyToCorpse, UseStrippingKeyToCorpse));

		std::string changingAppearanceOfCorpse = config["Config"]["ChangingAppearanceOfCorpse"].value_or("");
		if (changingAppearanceOfCorpse == "")
			return;
		ChangingAppearanceOfCorpse = (changingAppearanceOfCorpse == "True" || changingAppearanceOfCorpse == "true");
		Info(fmt::format("StrippingArmor: changingAppearanceOfCorpse={}, ChangingAppearanceOfCorpse={}", changingAppearanceOfCorpse, ChangingAppearanceOfCorpse));

		std::string enableDroppingItemsOn = config["Config"]["EnableDroppingItems"].value_or("");
		if (enableDroppingItemsOn == "")
			return;
		EnableDroppingItemsOn = (enableDroppingItemsOn == "True" || enableDroppingItemsOn == "true");
		Info(fmt::format("StrippingArmor: enableDroppingItemsOn={}, EnableDroppingItemsOn={}", enableDroppingItemsOn, EnableDroppingItemsOn));

		std::string canStealDroppedItemOn = config["Config"]["CanStealDroppedItem"].value_or("");
		if (canStealDroppedItemOn == "")
			return;
		CanStealDroppedItemOn = (canStealDroppedItemOn == "True" || canStealDroppedItemOn == "true");
		Info(fmt::format("StrippingArmor: canStealDroppedItemOn={}, CanStealDroppedItemOn={}", canStealDroppedItemOn, CanStealDroppedItemOn));

		std::string conditionTalkingOn = config["Config"]["ConditionTalking"].value_or("");
		if (conditionTalkingOn == "")
			return;
		ConditionTalkingOn = (conditionTalkingOn == "True" || conditionTalkingOn == "true");
		Info(fmt::format("StrippingArmor: conditionTalkingOn={}, ConditionTalkingOn={}", conditionTalkingOn, ConditionTalkingOn));

		std::string conditionSleepingOn = config["Config"]["ConditionSleeping"].value_or("");
		if (conditionSleepingOn == "")
			return;
		ConditionSleepingOn = (conditionSleepingOn == "True" || conditionSleepingOn == "true");
		Info(fmt::format("StrippingArmor: conditionSleepingOn={}, ConditionSleepingOn={}", conditionSleepingOn, ConditionSleepingOn));

		std::string conditionUnconsciousOn = config["Config"]["ConditionUnconscious"].value_or("");
		if (conditionUnconsciousOn == "")
			return;
		ConditionUnconsciousOn = (conditionUnconsciousOn == "True" || conditionUnconsciousOn == "true");
		Info(fmt::format("StrippingArmor: conditionUnconsciousOn={}, ConditionUnconsciousOn={}", conditionUnconsciousOn, ConditionUnconsciousOn));

		std::string conditionIsCommandedOn = config["Config"]["ConditionIsCommanded"].value_or("");
		if (conditionIsCommandedOn == "")
			return;
		ConditionIsCommandedOn = (conditionIsCommandedOn == "True" || conditionIsCommandedOn == "true");
		Info(fmt::format("StrippingArmor: conditionIsCommandedOn={}, ConditionIsCommandedOn={}", conditionIsCommandedOn, ConditionIsCommandedOn));

		std::string conditionPickingPocketOn = config["Config"]["ConditionPickingPocket"].value_or("");
		if (conditionPickingPocketOn == "")
			return;
		ConditionPickingPocketOn = (conditionPickingPocketOn == "True" || conditionPickingPocketOn == "true");
		Info(fmt::format("StrippingArmor: conditionPickingPocketOn={}, ConditionPickingPocketOn={}", conditionPickingPocketOn, ConditionPickingPocketOn));

		std::string conditionBleedingOutOn = config["Config"]["ConditionBleedingOut"].value_or("");
		if (conditionBleedingOutOn == "")
			return;
		ConditionBleedingOutOn = (conditionBleedingOutOn == "True" || conditionBleedingOutOn == "true");
		Info(fmt::format("StrippingArmor: conditionBleedingOutOn={}, ConditionBleedingOutOn={}", conditionBleedingOutOn, ConditionBleedingOutOn));

		std::string debugExecuteToAllActorsOn = config["Config"]["DebugExecuteToAllActorsInSameCell"].value_or("");
		if (debugExecuteToAllActorsOn == "")
			return;
		DebugExecuteToAllActorsOn = (debugExecuteToAllActorsOn == "True" || debugExecuteToAllActorsOn == "true");
		Info(fmt::format("StrippingArmor: debugExecuteToAllActorsOn={}, DebugExecuteToAllActorsOn={}", debugExecuteToAllActorsOn, DebugExecuteToAllActorsOn));

		std::string debugExecuteToCrossRefActorForcedOn = config["Config"]["DebugExecuteToCrossRefActorForced"].value_or("");
		if (debugExecuteToCrossRefActorForcedOn == "")
			return;
		DebugExecuteToCrossRefActorForcedOn = (debugExecuteToCrossRefActorForcedOn == "True" || debugExecuteToCrossRefActorForcedOn == "true");
		Info(fmt::format("StrippingArmor: debugExecuteToCrossRefActorForcedOn={}, DebugExecuteToCrossRefActorForcedOn={}", debugExecuteToCrossRefActorForcedOn, DebugExecuteToCrossRefActorForcedOn));


		

		std::string corpseTimer = config["Config"]["CorpseTimer"].value_or("");
		if (corpseTimer == "")
			return;
		try {
			CorpseTimer = stoi(corpseTimer);
		} catch (std::exception const& e) {
			Info(fmt::format("StrippingArmor: Exception in Config.cpp: corpseTimer is not number:{}", corpseTimer));
			CorpseTimer = 5;
		}
		Info(fmt::format("StrippingArmor: corpseTimer={}, CorpseTimer={}", corpseTimer, CorpseTimer));

		std::string repickTimer = config["Config"]["TakingBackEquipmentTimer"].value_or("");
		if (repickTimer == "")
			return;
		try {
			RePickTimer = stoi(repickTimer);
		} catch (std::exception const& e) {
			Info(fmt::format("StrippingArmor: Exception in Config.cpp: repickTimer is not number:{}", repickTimer));
			RePickTimer = 5;
		}
		Info(fmt::format("StrippingArmor: repickTimer={}, RePickTimer={}", repickTimer, RePickTimer));

		std::string timePerFrame = config["Config"]["TimePerFrame"].value_or("");
		if (timePerFrame == "")
			return;
		try {
			TimePerFrame = stoi(timePerFrame);
		} catch (std::exception const& e) {
			Info(fmt::format("StrippingArmor: Exception in Config.cpp: timePerFrame is not number:{}", timePerFrame));
			TimePerFrame = 50;
		}
		Info(fmt::format("StrippingArmor: timePerFrame={}, TimePerFrame={}", timePerFrame, TimePerFrame));
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


	bool GetCanStealDroppedItemOn()
	{
		return CanStealDroppedItemOn;
	}

	int GetTimePerFrame()
	{
		return TimePerFrame;
	}

}
