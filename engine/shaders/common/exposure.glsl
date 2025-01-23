// https://google.github.io/filament/Filament.md.html#imagingpipeline/physicallybasedcamera

#include "common/consts.glsl"

float Ev100FromLuminance(float luminance)
{
    return log2(luminance * (DEFAULT_ISO / DEFAULT_SENSITIVITY));
}

float Ev100FromCamSettings(float aperture, float shutterSpeedSec, float sensitivity)
{
    return log2((aperture * aperture) / shutterSpeedSec * 100.0f / sensitivity);
}

float Ev100ToExposureFactor(float ev100, float exposureCompenstation)
{
    return 1.0f / (pow(2.0f, ev100 + exposureCompenstation) + 1.2f);
}

float Ev100ToLuminance(float ev100)
{
    return exp2(ev100 - 3.0f);
}

float LuminanceTemporalAdaptation(float lumTarget, float lumCurrent, float dt, float adaptationSpeedUp, float adaptationSpeedDown)
{
    const bool adaptationUp = lumTarget > lumCurrent;
    const float adaptation = mix(adaptationSpeedDown, adaptationSpeedUp, float(adaptationUp));
    return lumCurrent + (lumTarget - lumCurrent) * (1.0f - exp(-dt * adaptation));
}