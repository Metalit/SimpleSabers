#include "hooks.hpp"

#include "GlobalNamespace/BladeMovementDataElement.hpp"
#include "GlobalNamespace/IBladeMovementData.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/SaberTrail.hpp"
#include "GlobalNamespace/SaberTrailRenderer.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/TimeHelper.hpp"
#include "GlobalNamespace/TrailElementCollection.hpp"
#include "config.hpp"
#include "main.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

static SaberTrail* leftTrail;
static TrailElementCollection* leftTrailElements;
static RainbowManager leftRainbow, rightRainbow;

MAKE_AUTO_HOOK_MATCH(SaberModelController_Init, &SaberModelController::Init, void, SaberModelController* self, Transform* parent, Saber* saber) {
    SaberModelController_Init(self, parent, saber);

    float thickness = getConfig().SaberWidth.GetValue();
    float length = getConfig().SaberLength.GetValue();

    auto transform = self->transform;
    transform->Find("BasicSaber")->localScale = {thickness, thickness, length};
    transform->Find("BasicSaber")->localPosition = {0, 0, defaultHandleLength * (length - 1)};
    transform->Find("FakeGlow0")->localScale = {thickness, length, thickness};
    transform->Find("FakeGlow0")->localPosition = {0, 0, length + defaultHandleLength * (length - 1)};
    transform->Find("FakeGlow1")->localScale = {thickness, length, thickness};

    if (saber->saberType == SaberType::SaberA) {
        leftTrail = self->_saberTrail;
        leftTrailElements = leftTrail->_trailElementCollection;
    }
}

MAKE_AUTO_HOOK_MATCH(SaberTrail_Init, &SaberTrail::Init, void, SaberTrail* self) {

    self->_trailDuration = getConfig().TrailLength.GetValue() * defaultTrailLength;

    if (getConfig().Colors.GetValue()) {
        if (self == leftTrail)
            self->_color = getConfig().LeftColor.GetValue().get_linear();
        else
            self->_color = getConfig().RightColor.GetValue().get_linear();
    }
    self->_color.a = getConfig().Opacity.GetValue();

    SaberTrail_Init(self);
}

MAKE_AUTO_HOOK_MATCH(SaberTrail_LateUpdate, &SaberTrail::LateUpdate, void, SaberTrail* self) {

    if (getConfig().Rainbow.GetValue()) {
        float time = self->_movementData->lastAddedData.time - self->_lastTrailElementTime;
        if (self == leftTrail)
            leftRainbow.AddTime(time);
        else
            rightRainbow.AddTime(time);
    }

    SaberTrail_LateUpdate(self);
}

MAKE_AUTO_HOOK_MATCH(SaberTrail_GetTrailWidth, &SaberTrail::GetTrailWidth, float, SaberTrail* self, BladeMovementDataElement lastAddedData) {

    auto value = SaberTrail_GetTrailWidth(self, lastAddedData);

    if (getConfig().Remove.GetValue())
        return 0;
    return value * getConfig().TrailWidth.GetValue() * getConfig().SaberLength.GetValue();
}

MAKE_AUTO_HOOK_MATCH(
    TrailElementCollection_SetHeadData, &TrailElementCollection::SetHeadData, void, TrailElementCollection* self, Vector3 start, Vector3 end, float time
) {
    float length = getConfig().SaberLength.GetValue();

    auto bladeVec = Vector3::op_Subtraction(end, start);
    bladeVec = Vector3::op_Multiply(bladeVec, length);
    end = Vector3::op_Addition(start, bladeVec);

    auto handleVec = Vector3::op_Multiply(bladeVec.normalized, defaultHandleLength * (length - 1));
    end = Vector3::op_Addition(end, handleVec);

    bladeVec = Vector3::op_Multiply(bladeVec, getConfig().TrailWidth.GetValue());
    start = Vector3::op_Subtraction(end, bladeVec);

    TrailElementCollection_SetHeadData(self, start, end, time);
}

MAKE_AUTO_HOOK_MATCH(
    SaberTrailRenderer_UpdateVertices,
    &SaberTrailRenderer::UpdateVertices,
    void,
    SaberTrailRenderer* self,
    TrailElementCollection* trailElementCollection,
    Color color
) {
    // ctrl c ctrl v
    if (getConfig().Rainbow.GetValue()) {
        TrailElementCollection::InterpolationState lerpState = {0, 0};
        Vector3 position;
        Vector3 normal;
        float time;
        trailElementCollection->Interpolate(0, byref(lerpState), byref(position), byref(normal), byref(time));
        for (int i = 0; i < self->_granularity; i++) {
            float timeDelta = TimeHelper::get_time() - time;
            float y = timeDelta / self->_trailDuration;
            normal = Vector3::op_Multiply(normal, self->_trailWidth * 0.5);
            auto vector = Vector3::op_Addition(position, normal);
            auto vector2 = position;
            auto vector3 = Vector3::op_Subtraction(position, normal);
            trailElementCollection->Interpolate((i + 1) / (float) self->_granularity, byref(lerpState), byref(position), byref(normal), byref(time));
            // modified part
            if (trailElementCollection == leftTrailElements)
                color = leftRainbow.ColorAtTime(time);
            else
                color = rightRainbow.ColorAtTime(time);
            color.a = getConfig().Opacity.GetValue();
            int num = i * 3;
            self->_vertices[num] = vector;
            self->_colors[num] = color;
            self->_uvs[num] = {0, y};
            self->_vertices[num + 1] = vector2;
            self->_colors[num + 1] = color;
            self->_uvs[num + 1] = {0.5, y};
            self->_vertices[num + 2] = vector3;
            self->_colors[num + 2] = color;
            self->_uvs[num + 2] = {1, y};
        }
    } else
        SaberTrailRenderer_UpdateVertices(self, trailElementCollection, color);
}
