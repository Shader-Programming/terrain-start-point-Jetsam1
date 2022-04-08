
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec4 clipSpace;
out vec2 texCoords;
out vec3 camVec;
out vec3 lightVec;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 camPos;

const float tiling=4.f;

void main()
{
	vec4 worldPos = model*vec4(aPos,1.f);
	clipSpace=projection*view*worldPos;
	gl_Position=clipSpace;
    texCoords = aTexCoords*tiling;  
	camVec = (0.5*camPos)-worldPos.xyz;
	lightVec= worldPos.xyz-lightPos;

}