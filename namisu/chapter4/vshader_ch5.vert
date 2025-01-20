#version 400
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec3 ReflectDir; // The direction of the reflected ray
uniform bool DrawSkyBox;
uniform vec3 WorldCameraPosition;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    if( DrawSkyBox ) {
        ReflectDir = vPosition;
    } else {
        vec3 worldPos = vec3( ModelMatrix * vec4(vPosition,1.0) );
        vec3 worldNorm = vec3(ModelMatrix * vec4(vNormal, 0.0));
        vec3 worldView = normalize( WorldCameraPosition - worldPos);
        ReflectDir = reflect(-worldView, worldNorm );
    }
    gl_Position = MVP * vec4(vPosition,1.0);
}
