#version 400

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 FrontColor;
out vec3 BackColor;

struct LightInfo {
    vec4 Position; // Light position in eye coordinates
    vec3 Intensity; // Light intensity
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
};
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main() {
    // Normal vector transformed to eye coordinates
    vec3 tnorm = normalize(NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(Light.Position - eyeCoords));

    // Front-facing shading
    float frontDiffuse = max(dot(s, tnorm), 0.0);
    FrontColor = Light.Intensity * Material.Kd * frontDiffuse + Material.Ka * Light.Intensity;

    // Back-facing shading
    float backDiffuse = max(dot(s, -tnorm), 0.0);
    BackColor = Light.Intensity * Material.Kd * backDiffuse + Material.Ka * Light.Intensity;

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
