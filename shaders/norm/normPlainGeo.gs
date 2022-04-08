#version 330 core

layout(triangles) in;

layout(line_strip , max_vertices=2) out;

in vec3 normES[];
in vec3 posES[];

out vec3 gNorm;
out vec3 gWorldPos_FS_in;

uniform float length;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	for(int i=0;i<3;i++)
	{
		gNorm=normES[i];
		gl_Position=view*projection*vec4(gWorldPos_FS_in,1.f);
		EmitVertex();
		gl_Position=view*projection*(vec4(gWorldPos_FS_in,1.f)+vec4(gNorm * length,0.f));
		EmitVertex();
		EndPrimitive();

	}

}