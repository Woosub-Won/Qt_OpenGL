// attribute vec4 qt_Vertex;
// attribute vec4 qt_MultiTexCoord0;
// uniform mat4 qt_ModelViewProjectionMatrix;
// varying vec4 qt_TexCoord0;

// void main(void)
// {
//     gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
//     qt_TexCoord0 = qt_MultiTexCoord0;
// }



// #version 400
// layout (location = 0) in vec3 VertexPosition;
// layout (location = 1) in vec3 VertexColor;

// out vec3 Color;
// uniform mat4 RotationMatrix;
// uniform BlobSettings {
//     vec4 InnerColor;
//     vec4 OuterColor;
//     float RadiusInner;
//     float RadiusOuter;
// };

// void main()
// {
//     Color = VertexColor;
//     gl_Position = RotationMatrix * vec4(VertexPosition,1.0);
// }

#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexTexCoord;
out vec3 TexCoord;

void main()
{
    TexCoord = VertexTexCoord;
    gl_Position = vec4(VertexPosition,1.0);
}

