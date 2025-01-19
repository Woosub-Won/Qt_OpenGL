#version 400
in vec3 LightDir;
in vec2 TexCoord;
in vec3 ViewDir;
in vec3 Normal;

uniform sampler2D ColorTex;
uniform sampler2D NormalMapTex;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

layout( location = 0 ) out vec4 FragColor;

vec3 phongModel( vec3 norm, vec3 diffR ) {
    vec3 r = reflect( -LightDir, norm );
    vec3 ambient = LightIntensity * Ka;
    float sDotN = max( dot(LightDir, norm), 0.0 );
    vec3 diffuse = LightIntensity * diffR * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = LightIntensity * Ks * pow( max( dot(r,ViewDir), 0.0 ),Shininess );
    return ambient + diffuse + spec;
}
void main() {
    // Lookup the normal from the normal map
    vec4 normal = texture( NormalMapTex, TexCoord );
    // vec4 normal = vec4(Normal, 1.0);
    // The color texture is used as the diffuse reflectivity
    vec4 texColor = texture( ColorTex, TexCoord );
    FragColor = vec4( phongModel(normal.xyz, texColor.rgb), 1.0 );
}
