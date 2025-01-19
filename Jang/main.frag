#version 400
in vec3 ReflectDir; // The direction of the reflected ray
in vec3 RefractDir;

in vec3 worldNorm;
in vec3 worldView;

uniform float Eta;
uniform float ReflectionFactor;

uniform samplerCube CubeMapTex; // The cube map
uniform bool DrawSkyBox; // Are we drawing the sky box?
uniform vec3 WorldCameraPosition;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
layout( location = 0 ) out vec4 FragColor;
void main() {
    // Access the cube map texture
    vec4 reflectColor = texture(CubeMapTex, ReflectDir);
    vec4 refractColor = texture(CubeMapTex, RefractDir);
    // vec4 reflectColor = texture(CubeMapTex, normalize(ReflectDir));
    // vec4 refractColor = texture(CubeMapTex, normalize(RefractDir));

    if( DrawSkyBox ){
        FragColor = reflectColor;
    }
    else{
        FragColor = mix(refractColor, reflectColor, ReflectionFactor);
    }
}
