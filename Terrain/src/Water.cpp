#include <Water.h>

Water::Water(float midX, float midY, float scale) : x(midX), y(midY), scale(scale)
{}

unsigned int Water::getVAO()
{
	vertices = { x - (scale / 2),height,y + (scale / 2),0.f,1.f,
				x + (scale / 2),height,y + (scale / 2),1.f,1.f,
				x - (scale / 2),height,y - (scale / 2),0.f,0.f,
				x + (scale / 2),height,y - (scale / 2),1.f,0.f
	};
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);
	//Pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//UV
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}
