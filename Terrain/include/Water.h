#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Water
{
private:
	float scale = 1.f, x = 0.f, y = 0.f, height = 75.f;
	std::vector<float> vertices;
	std::vector<int> indices;
	unsigned int VAO, VBO;
public:
	Water(float midX,float midY,float scale);
	inline int getSize() { return vertices.size(); }
	inline float getHeight() { return height; }
	unsigned int getVAO();
};