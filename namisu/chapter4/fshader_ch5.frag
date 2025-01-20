#version 400
in vec3 ReflectDir; // The direction of the reflected ray
uniform samplerCube CubeMapTex; // The cube map
uniform bool DrawSkyBox; // Are we drawing the sky box?
uniform float ReflectFactor; // Amount of reflection
uniform vec4 MaterialColor; // Color of the object's "Tint"

layout( location = 0 ) out vec4 FragColor;


void main() {
    // Access the cube map texture
    vec4 CubeMapColor = texture(CubeMapTex,ReflectDir);
    if( DrawSkyBox )
        FragColor = CubeMapColor;
    else
        FragColor = mix(MaterialColor, CubeMapColor, ReflectFactor);

}
