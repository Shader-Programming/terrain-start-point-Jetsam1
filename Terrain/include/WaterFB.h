#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class WaterFB
{
private:
	void initReflectFB();
	void initRefractFB();
	void bindFB(int FB, int width, int height);
	int createFB();
	int createTexAttachment(int width, int height);
	int createDepthTexAttachment(int width, int height);
	int createDepthBufferAttachment(int width, int height);

	int reflectFB = 0;
	int reflectTex = 0;
	int reflectDepthBuffer = 0;

	int refractFB = 0;
	int refractTex = 0;
	int depthTex = 0;
	int scrWidth, scrHeight;
public:
	WaterFB(int screenWidth, int screenHeight);
	void bindReflection();
	void bindRefraction();
	void unbindFB();
	int getReflectTex();
	int getRefractTex();
	int getDepthTex();

	  static constexpr int REFLECTION_WIDTH =640 ;
	  static constexpr int REFLECTION_HEIGHT =360 ;
	  static constexpr int REFRACTION_WIDTH = 1280;
	  static constexpr int REFRACTION_HEIGHT = 720;
};
