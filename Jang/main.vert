#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;
out vec3 Normal;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    // Transform normal and tangent to eye space
    vec3 norm = normalize(NormalMatrix * VertexNormal);
    vec3 tang = normalize(NormalMatrix * vec3(VertexTangent));
    // Compute the binormal
    vec3 binormal = normalize( cross( norm, tang ) ) *
    VertexTangent.w;
    // Matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(
    tang.x, binormal.x, norm.x,
    tang.y, binormal.y, norm.y,
    tang.z, binormal.z, norm.z ) ;
    // Get the position in eye coordinates
    vec3 pos = vec3( ModelViewMatrix *
    vec4(VertexPosition,1.0) );
    // Transform light dir. and view dir. to tangent space
    LightDir = normalize( toObjectLocal * (LightPosition.xyz - pos) );
    ViewDir = toObjectLocal * normalize(-pos);
    // Pass along the texture coordinate
    TexCoord = VertexTexCoord;
    gl_Position = MVP * vec4(VertexPosition,1.0);
    Normal = normalize( NormalMatrix * VertexNormal);
}
