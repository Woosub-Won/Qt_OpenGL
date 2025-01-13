#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

struct LightInfo {
    vec4 Position; // Light position in eye coordinates
    vec3 La;       // Ambient light intensity
    vec3 Ld;       // Diffuse light intensity
    vec3 Ls;       // Specular light intensity
};

uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka;        // Ambient reflectivity
    vec3 Kd;        // Diffuse reflectivity
    vec3 Ks;        // Specular reflectivity
    float Shininess; // Specular shininess factor
};

uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

// 함수 정의: 반사광을 계산
vec3 calculateSpecular(vec3 r, vec3 v) {
    return Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
}

// 함수 정의: 광원 계산
vec3 calculateLighting(vec3 tnorm, vec3 s, vec3 v) {
    vec3 ambient = Light.La * Material.Ka;
    float sDotN = max(dot(s, tnorm), 0.0);
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;
    vec3 specular = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 r = reflect(-s, tnorm);
        specular = calculateSpecular(r, v);
    }
    return ambient + diffuse + specular;
}

void main() {
    vec3 tnorm = normalize(NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(Light.Position - eyeCoords));
    vec3 v = normalize(-eyeCoords.xyz);

    // 함수 호출로 광원 계산
    LightIntensity = calculateLighting(tnorm, s, v);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
