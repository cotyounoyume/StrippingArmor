#include "Config.h"

namespace Config
{
	void ReadIni()
	{
		constexpr auto        pathString = L"Data/SFSE/Plugins/StrippingArmor.ini";
		std::filesystem::path path = std::filesystem::path(pathString);
		if (!std::filesystem::exists(path))
			return;

		Utility::Notification(fmt::format("StrippingArmor: path={}", path.string()));
		auto config = toml::parse_file(path.string());

		std::string key = config["Config"]["StrippingKey"].value_or("");
		if (key == "")
			return;
		StrippingKey = key;
		Utility::Notification(fmt::format("StrippingArmor: key={}, keynum={}", key, key.c_str()[0]));

		std::string effect = config["Config"]["EffectShaderForStripping"].value_or("");
		if (effect == "")
			return;
		EffectOn = (effect == "True" || effect == "true");
		Utility::Notification(fmt::format("StrippingArmor: effect={}, EffectOn={}", effect, EffectOn));

		std::string effectForCorpse = config["Config"]["EffectShaderForChangingCorpse"].value_or("");
		if (effectForCorpse == "")
			return;
		EffectForCorpseOn = (effectForCorpse == "True" || effectForCorpse == "true");
		Utility::Notification(fmt::format("StrippingArmor: effectForCorpse={}, EffectForCorpse={}", effect, EffectForCorpseOn));

		std::string effectFormID = config["Config"]["EffectFormID"].value_or("");
		if (effectFormID == "")
			return;
		EffectFormID = effectFormID;
		Utility::Notification(fmt::format("StrippingArmor: effectFormID={}, EffectFormID={}", effectFormID, EffectFormID));

		std::string alternativeCloth = config["Config"]["AlternativeCloth"].value_or("");
		if (alternativeCloth == "")
			return;
		AlternativeClothOn = (alternativeCloth == "True" || alternativeCloth == "true");
		Utility::Notification(fmt::format("StrippingArmor: alternativeCloth={}, AlternativeClothOn={}", alternativeCloth, AlternativeClothOn));

		std::string useStrippingKeyToCorpse = config["Config"]["UseStrippingKeyToCorpse"].value_or("");
		if (useStrippingKeyToCorpse == "")
			return;
		UseStrippingKeyToCorpse = (useStrippingKeyToCorpse == "True" || useStrippingKeyToCorpse == "true");
		Utility::Notification(fmt::format("StrippingArmor: useStrippingKeyToCorpse={}, UseStrippingKeyToCorpse={}", useStrippingKeyToCorpse, UseStrippingKeyToCorpse));

		std::string changingAppearanceOfCorpse = config["Config"]["ChangingAppearanceOfCorpse"].value_or("");
		if (changingAppearanceOfCorpse == "")
			return;
		ChangingAppearanceOfCorpse = (changingAppearanceOfCorpse == "True" || changingAppearanceOfCorpse == "true");
		Utility::Notification(fmt::format("StrippingArmor: changingAppearanceOfCorpse={}, ChangingAppearanceOfCorpse={}", changingAppearanceOfCorpse, ChangingAppearanceOfCorpse));

		std::string corpseTimer = config["Config"]["CorpseTimer"].value_or("");
		if (corpseTimer == "")
			return;
		try {
			CorpseTimer = stoi(corpseTimer);
		} catch (std::exception const& e) {
			Utility::Notification(fmt::format("StrippingArmor: Exception in Config.cpp: corpseTimer is not number:{}", corpseTimer));
			CorpseTimer = 5;
		}
		Utility::Notification(fmt::format("StrippingArmor: corpseTimer={}, CorpseTimer={}", corpseTimer, CorpseTimer));

		std::string timePerFrame = config["Config"]["TimePerFrame"].value_or("");
		if (timePerFrame == "")
			return;
		try {
			TimePerFrame = stoi(timePerFrame);
		} catch (std::exception const& e) {
			Utility::Notification(fmt::format("StrippingArmor: Exception in Config.cpp: timePerFrame is not number:{}", timePerFrame));
			TimePerFrame = 50;
		}
		Utility::Notification(fmt::format("StrippingArmor: timePerFrame={}, TimePerFrame={}", timePerFrame, TimePerFrame));
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

	bool GetUseStrippingKeyToCorpse()
	{
		return UseStrippingKeyToCorpse;
	}

	int GetTimePerFrame()
	{
		return TimePerFrame;
	}

}
