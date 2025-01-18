#version 400
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout( location = 0 ) out vec4 FragColor;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

uniform float FogMaxDist;
uniform float FogMinDist;
uniform vec3 FogColor;

vec3 ads( )
{
    vec3 n = normalize( Normal );
    vec3 s = normalize( vec3(LightPosition) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, n );
    return LightIntensity * ( Ka +Kd * max( dot(s, n), 0.0 ) + Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}

vec3 ads_back( )
{
    vec3 n = normalize( -Normal );
    vec3 s = normalize( vec3(LightPosition) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, n );
    return LightIntensity * ( Ka +Kd * max( dot(s, n), 0.0 ) + Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}

vec3 ads_middle( )
{
    vec3 s = normalize( LightPosition.xyz - Position.xyz );
    vec3 v = normalize(vec3(-Position));
    vec3 h = normalize( v + s );
    vec3 ambient = Ka * LightIntensity;
    vec3 diffuse = LightIntensity * Kd *
    max(0.0, dot(s, Normal) );
    vec3 spec = LightIntensity * Ks * pow( max( 0.0, dot( h, Normal) ), Shininess );
    return ambient + diffuse + spec;
}

void main() {
    const float scale = 15.0;
    bvec2 toDiscard = greaterThan( fract(TexCoord * scale), vec2(0.2,0.2) );
    if( all(toDiscard) )
        discard;

    float dist = abs( Position.z );
    float fogFactor = (FogMaxDist - dist) / (FogMaxDist - FogMinDist);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );
    // vec3 shadeColor = ads_middle();
    // vec3 shadeColor = ads();
    // vec3 color = mix( FogColor, shadeColor, fogFactor );
    if (gl_FrontFacing){
        FragColor = vec4(ads(), 1.0);
    }
    else{
        FragColor = vec4(ads_back(), 1.0);
    }
    // FragColor = vec4(shadeColor, 1.0);
}
