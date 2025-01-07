// attribute vec4 qt_Vertex;
// attribute vec4 qt_MultiTexCoord0;
// uniform mat4 qt_ModelViewProjectionMatrix;
// varying vec4 qt_TexCoord0;

// void main(void)
// {
//     gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
//     qt_TexCoord0 = qt_MultiTexCoord0;
// }

#version 400
in vec3 VertexPosition;
in vec3 VertexColor;
out vec3 Color;
void main()
{
    Color = VertexColor;
    gl_Position = vec4( VertexPosition, 1.0 );
}
