#pragma once

#include "HMUI/ViewController.hpp"
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(Config) {
    CONFIG_VALUE(SaberWidth, float, "Saber Thickness", 0.5);
    CONFIG_VALUE(SaberLength, float, "Saber Length", 1);
    CONFIG_VALUE(Remove, bool, "Disable Trail", false);
    CONFIG_VALUE(TrailWidth, float, "Trail Thickness", 0.1);
    CONFIG_VALUE(TrailLength, float, "Trail Length", 1);
    CONFIG_VALUE(Opacity, float, "Trail Opacity", 1);
    CONFIG_VALUE(Rainbow, bool, "Rainbow Trail", false);
    CONFIG_VALUE(Colors, bool, "Override Trail Colors", false);
    CONFIG_VALUE(LeftColor, UnityEngine::Color, "Left Color", {});
    CONFIG_VALUE(RightColor, UnityEngine::Color, "Right Color", {});
};

static constexpr float defaultHandleLength = 0.17;
static constexpr float defaultTrailLength = 0.4;

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

struct RainbowManager {
    UnityEngine::Color ColorAtTime(float time);
    void AddTime(float time);

   private:
    float timeOffset;
};
