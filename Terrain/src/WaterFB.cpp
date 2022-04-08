#include "..\include\WaterFB.h"
#include <iostream>

void WaterFB::initReflectFB()
{
	reflectFB = createFB();
	reflectTex = createTexAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	reflectDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);

	unbindFB();
}

void WaterFB::initRefractFB()
{
	refractFB = createFB();
	refractTex = createTexAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	depthTex = createDepthBufferAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	unbindFB();
}

void WaterFB::bindFB(int FB, int width, int height)
{
	glBindFramebuffer(1, FB);
	glViewport(0, 0, width, height);
}

int WaterFB::createFB()
{
	unsigned int fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	return fb;
}

int WaterFB::createTexAttachment(int width, int height)
{
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	return tex;
}

int WaterFB::createDepthTexAttachment(int width, int height)
{
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
	return 0;
}

int WaterFB::createDepthBufferAttachment(int width, int height)
{
	unsigned int depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}

WaterFB::WaterFB(int screenWidth, int screenHeight) : scrWidth(screenWidth), scrHeight(screenHeight)
{
	glActiveTexture(GL_TEXTURE0);
	initReflectFB();
	initRefractFB();
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, reflectTex);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, refractTex);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, depthTex);

}

void WaterFB::bindReflection()
{
	bindFB(reflectFB, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void WaterFB::bindRefraction()
{
	bindFB(refractFB,REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterFB::unbindFB()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scrWidth, scrHeight);
}

int WaterFB::getReflectTex()
{
	return reflectTex;
}

int WaterFB::getRefractTex()
{
	return refractTex;
}

int WaterFB::getDepthTex()
{
	return depthTex;
}
