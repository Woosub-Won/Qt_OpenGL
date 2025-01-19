#version 400
in vec3 Position;
in vec3 Normal;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

const int levels = 3;
const float scaleFactor = 1.0 / levels;

layout( location = 0 ) out vec4 FragColor;

vec3 ads( )
{
    vec3 n = normalize( Normal );
    vec3 s = normalize( vec3(LightPosition) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 h = normalize( v + s );
    return LightIntensity * ( Ka + Kd * max( dot(s, n), 0.0 ) + Ks * pow( max( dot(h, v), 0.0 ), Shininess ) );
}

vec3 toonShade( )
{
    vec3 s = normalize( LightPosition.xyz - Position.xyz );
    float cosine = max( 0.0, dot( s, Normal ) );
    vec3 diffuse = Kd * floor( cosine * levels ) *
    scaleFactor;
    return LightIntensity * (Ka + diffuse);
}

void main() {
    FragColor = vec4(toonShade(), 1.0);
}
