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

	bool GetCanStealDroppedItemOn()
	{
		return CanStealDroppedItemOn;
	}

	int GetTimePerFrame()
	{
		return TimePerFrame;
	}

}
