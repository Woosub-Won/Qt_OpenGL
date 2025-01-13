#version 400
in vec3 Position;
in vec3 Normal;
struct LightInfo {
    vec4 position;
    vec3 intensity;
};
uniform LightInfo Light;
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
//uniform int levels;
const int levels = 5;
const float scaleFactor = 1.0 / levels;
layout( location = 0 ) out vec4 FragColor;
vec3 toonShade( )
{
    //float scaleFactor = 1.0f / levels;
    vec3 s = normalize( Light.position.xyz - Position.xyz );
    float cosine = max( 0.0, dot( s, Normal ) );
    vec3 diffuse = Kd * floor( cosine * levels ) * scaleFactor;
    return Light.intensity * (Ka + diffuse);
}
void main() {
    FragColor = vec4(toonShade(), 1.0);
}
