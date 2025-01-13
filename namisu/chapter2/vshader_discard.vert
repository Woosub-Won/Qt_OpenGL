#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
out vec3 FrontColor;
out vec3 BackColor;
out vec2 TexCoord;
struct LightInfo {
    vec4 Position; // Light position in eye coords.
    vec3 La; // Ambient light intensity
    vec3 Ld; // Diffuse light intensity
    vec3 Ls; // Specular light intensity


};
uniform LightInfo Light;
struct MaterialInfo {
     vec3 Ka; // Ambient reflectivity
     vec3 Kd; // Diffuse reflectivity
     vec3 Ks; // Specular reflectivity
     float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
void getEyeSpace( out vec3 norm, out vec4 position )
{
     norm = normalize( NormalMatrix * VertexNormal);
     position = ModelViewMatrix * vec4(VertexPosition,1.0);
}
vec3 phongModel( vec4 position, vec3 norm )
{
 // The ADS shading calculations go here (see: "Using
 // functions in shaders," and "Implementing
 // per-vertex ambient, diffuse and specular (ADS) shading")
}
void main()
{
     vec3 eyeNorm;
     vec4 eyePosition;
     TexCoord = VertexTexCoord;
     // Get the position and normal in eye space
     getEyeSpace(eyeNorm, eyePosition);
     FrontColor = phongModel( eyePosition, eyeNorm );
     BackColor = phongModel( eyePosition, -eyeNorm );
     gl_Position = MVP * vec4(VertexPosition,1.0);
}
