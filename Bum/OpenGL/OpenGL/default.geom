#version 400


layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec3 color;
out vec2 texCoord;

/*
in gl_Vertex
{
	vec4 gl_Positioin;
	float gl_PositionSize;
	float gl_ClipDistance[];
}gl_in[]
*/

in Data
{
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
}data_in[];

void main()
{
	vec3 v0 = vec3( gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 v1 = vec3( gl_in[2].gl_Position - gl_in[1].gl_Position);
	vec4 surfaceNormal = vec4( normalize(cross(v0, v1))* 0.5f ,0.0f);

	surfaceNormal = vec4(0,0, 0,0);
	
	gl_Position = data_in[0].projection * ( gl_in[0].gl_Position + surfaceNormal);
	Normal = data_in[0].Normal;
	color = data_in[0].color;
	texCoord = data_in[0].texCoord;
	EmitVertex();
	
	gl_Position = data_in[1].projection *( gl_in[1].gl_Position + surfaceNormal);
	Normal = data_in[1].Normal;
	color = data_in[1].color;
	texCoord = data_in[1].texCoord;
	EmitVertex();
	
	gl_Position = data_in[2].projection * ( gl_in[2].gl_Position + surfaceNormal);
	Normal = data_in[2].Normal;
	color = data_in[2].color;
	texCoord = data_in[2].texCoord;
	EmitVertex();
	 

	EndPrimitive();
}