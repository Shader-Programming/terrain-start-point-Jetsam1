#version 330 core
layout(location = 0) out vec4 fragColor ;

struct Lights
{
	vec3 pos;
	vec3 amb;
	vec3 spec;
	vec3 diff;
};
uniform Lights light;

in vec3 gWorldPos_FS_in;
in vec3 gNorms;
in float fog;

uniform float scale;
uniform vec3 camPos;
uniform vec3 sky;

vec3 colour;


void main()
{
	float height = gWorldPos_FS_in.y / scale;
	vec4 grass = vec4(0.196f, 0.453f, 0.25f, 1.000f);
	vec4 rock =  vec4(0.124f, 0.142f, 0.135f, 1.000f);
	vec4 snow =  vec4(1.000f, 0.980f, 0.980f, 1.000f);
		
	// Light Normal and Dir
	vec3 normal = gNorms;
	vec3 lightDir = normalize(light.pos - gWorldPos_FS_in);
	
	//Check height of the point to determine what to colour it:
	colour = vec3(0.235f, 0.145f, 0.082f);
	
	if (height < 0.8f)		colour = grass.rgb;
	if (height < 1.1f)		colour = vec3(mix(grass, rock, smoothstep(0.90f, 1.10f, height)).rgb);
	else if (height < 1.3f)	colour = vec3(mix(rock, snow,  smoothstep(1.20f, 1.30f, height)).rgb);
	else					colour = snow.rgb;

	//Ambient
	vec3 ambient = colour * light.amb;
	
	//Diffuse
	float diff = max(dot(normal, -light.pos), 0.0f);
	vec3 diffuse = (diff * colour) * light.diff;
	
	//Specular
	vec3 viewDir = normalize(camPos - gWorldPos_FS_in);
	vec3 halfWay = normalize(-lightDir + viewDir);
	float spec = pow(max(dot(halfWay, viewDir), 0.0f), 0.9f);
	vec3 specular = (spec * colour) * light.spec;

	//Regular Frag colour
	vec3 blinnPhong = (ambient + diffuse + specular);
	fragColor = vec4(blinnPhong, 1.0f);
	fragColor = mix(vec4(sky, 1.0f), fragColor, fog);
}
	