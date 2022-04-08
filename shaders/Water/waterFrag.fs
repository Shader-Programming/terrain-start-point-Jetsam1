#version 330 core

in vec4 clipSpace;
in vec2 texCoords;
in vec3 camVec;
in vec3 lightVec;

out vec4 FragColor ;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D depthMap;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform vec3 lightCol;
uniform float flow;

const float waveStr=0.05f;
const float shine = 20.f;
const float specFac= 0.5f;
void main()
{

	vec2 ndc = (clipSpace.xy/clipSpace.w) /2.f +0.5f;
	vec2 reflectionTexCoords = vec2(ndc.x,-ndc.y);
	vec2 refractionTexCoords = vec2(ndc.x,ndc.y);

	float near=0.1f;
	float far=1200.f;
	float depth=texture(depthMap,refractionTexCoords).r;
	float floorDist = 2.f* near * far / (far +near - (2*depth -1.f)*(far - near));

	depth=gl_FragCoord.z;
	float waterDist = 2.f* near * far / (far +near - (2*depth -1.f)*(far - near));
	float waterDepth= floorDist-waterDist;

	vec2 distortTexCoords = texture(dudvMap,vec2(texCoords.x +flow,texCoords.y)).rg*0.1f;
	distortTexCoords = texCoords+vec2(distortTexCoords.x+distortTexCoords.y+flow);
	vec2 totalDistort = (texture(dudvMap,distortTexCoords).rg * 2.f - 1.f) *waveStr * clamp(waterDepth/20.f,0.f,1.f);

	reflectionTexCoords+=totalDistort;
	refractionTexCoords+=totalDistort;

	reflectionTexCoords.x=clamp(reflectionTexCoords.x,0.001,0.999);
	reflectionTexCoords.y=clamp(reflectionTexCoords.x,-0.001,-0.999);
	refractionTexCoords = clamp(refractionTexCoords,0.001,0.999);

	vec4 reflectionColour = texture(reflectionTex,reflectionTexCoords);
	vec4 refractionColour = texture(refractionTex,refractionTexCoords);

	vec4 normMapCol =texture(normalMap,distortTexCoords);
	vec3 normal =  vec3(normMapCol.r *2.f -1.f,normMapCol.b*3.f,normMapCol.g*2.f-1.f);
	normal =  normalize(normal);

	vec3 viewVec=normalize(camVec);
	float refractionStr=pow(dot(viewVec,normal),1.5f);

	vec3 reflectedLight = reflect(normalize(lightVec),normal);
	float spec= max(dot(reflectedLight,viewVec),0.f);
	spec=pow(spec,shine);
	vec3 specHighlight=lightCol *spec*specFac*clamp(waterDepth/5.f,0.f,1.f);

    FragColor =mix(reflectionColour,refractionColour,refractionStr);
	FragColor= mix(FragColor,vec4(0.f,0.3f,0.5f,1.0f),0.2f)+vec4(specHighlight,0.f);
}
	
	