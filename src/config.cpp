#include "config.hpp"

#include "bsml/shared/BSML-Lite.hpp"
#include "main.hpp"

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (!firstActivation)
        return;

    auto vertical = BSML::Lite::CreateVerticalLayoutGroup(self);
    vertical->childControlHeight = false;
    vertical->childForceExpandHeight = false;

    AddConfigValueSliderIncrement(vertical, getConfig().SaberWidth, 0.01, 0.01, 2);
    AddConfigValueSliderIncrement(vertical, getConfig().SaberLength, 0.01, 0.01, 2);
    AddConfigValueToggle(vertical, getConfig().Remove);
    AddConfigValueSliderIncrement(vertical, getConfig().TrailWidth, 0.01, 0.01, 1);
    AddConfigValueSliderIncrement(vertical, getConfig().TrailLength, 0.01, 0.01, 2);
    AddConfigValueIncrementFloat(vertical, getConfig().Opacity, 1, 0.1, 0.1, 1);
    AddConfigValueToggle(vertical, getConfig().Rainbow);
    AddConfigValueToggle(vertical, getConfig().Colors);
    AddConfigValueColorPicker(vertical, getConfig().LeftColor);
    AddConfigValueColorPicker(vertical, getConfig().RightColor);
}

static constexpr float rainbowTime = defaultTrailLength * 2;

inline float RainbowMod(float val) {
    float mod = rainbowTime;
    while (val >= mod)
        val -= mod;
    return val;
}

static constexpr std::array<UnityEngine::Color, 6> rainbow = {{{1, 0, 0, 1}, {1, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 1, 1}, {1, 0, 1, 1}}};

UnityEngine::Color RainbowManager::ColorAtTime(float time) {
    time = RainbowMod(time + timeOffset) / rainbowTime;
    int min = time * rainbow.size();
    float lerp = (time * rainbow.size()) - min;
    UnityEngine::Color first = rainbow[min % rainbow.size()];
    UnityEngine::Color second = rainbow[(min + 1) % rainbow.size()];
    return UnityEngine::Color::Lerp(first, second, lerp);
}

void RainbowManager::AddTime(float time) {
    timeOffset = RainbowMod(timeOffset + time);
}
