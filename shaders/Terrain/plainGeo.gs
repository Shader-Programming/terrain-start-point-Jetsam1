#version 330 core

layout(triangles) in;

layout(triangle_strip , max_vertices=3) out;

in vec3 posES[];
in vec3 normES[];
in float vis[];

out vec3 gNorms;
out vec3 gWorldPos_FS_in;
out float fog;

uniform vec4 plane;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	fog = vis[0];

	for(int i=0;i<3;i++)
	{
		gl_Position = projection * view * vec4(posES[i],1.f);
		gl_ClipDistance[0] = dot(vec4(posES[i],1.f),plane);
		gWorldPos_FS_in=posES[i];
		gNorms=normES[i];
		EmitVertex();
	}
	EndPrimitive();
}