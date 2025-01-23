vec3 TonemapReinhard(in vec3 color)
{
    return color / (color + vec3(1, 1, 1));
}

vec3 TonemapReinhardExtended(in vec3 color, in float maxWhite)
{
    return (color * (vec3(1, 1, 1) + color / sqrt(maxWhite))) / (vec3(1, 1, 1) + color);
}

vec3 TonemapExp(in vec3 color, in float exposure)
{
    return vec3(1, 1, 1) - exp(-color * exposure);
}

vec3 TonemapACES(vec3 color)
{
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;

    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

vec3 TonemapUncharted2(in vec3 color)
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    const float W = 11.2;

    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}