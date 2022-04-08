#include "Terrain.h"


//Terrain constructors
Terrain::Terrain(int widthIn, int heightIn, int stepSizeIn)
{
	width = widthIn;
	height = heightIn;
	stepSize = stepSizeIn;
	makeVertices(&vertices);

}

Terrain::Terrain() {
	width = 50;
	height = 50;
	stepSize = 10;
	makeVertices(&vertices);

}

unsigned int Terrain::getVAO() {
	
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), vertices.data(), GL_STATIC_DRAW);

		//xyz
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//texture
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	
		return VAO;
}

int Terrain::getSize() {
	return vertices.size();
}

double Terrain::cycleOctave(glm::vec3 pos, int no_Octaves)
{
	float total = 0.f;
	float maxAmp = 0.f;
	float amp = 100.f;
	float freq = 0.01f;

	for (int i = 0; i < no_Octaves; i++)
	{
		double x = pos.x * freq;
		double y = pos.y * freq;

		total = noise.noise(x, y, 0.1)*amp;
		maxAmp += amp;
		freq *= 2;
		amp /= 2;
	}
	return total/maxAmp;
}


std::vector<float> Terrain::getVertices() {
	return vertices;
}

void Terrain::makeVertices(std::vector<float> *vertices) {
	/* triangle a b c
		   b
		   | \
		   a _ c


		 triangle d f e
		   f _ e
			 \ |
			   d
		  */

	for (int y = 0; y < height - 1; y++)
	{
		float  offSetY = y * stepSize;
		for (int x = 0; x < width - 1; x++) {
			float offSetX = x * stepSize;
			makeVertex(offSetX, offSetY, vertices);  // a
			makeVertex(offSetX, offSetY + stepSize, vertices);  // b
			makeVertex(offSetX + stepSize, offSetY, vertices);   // c
			makeVertex(offSetX + stepSize, offSetY, vertices);  //d
			makeVertex(offSetX, offSetY + stepSize, vertices);  //e
			makeVertex(offSetX + stepSize, offSetY + stepSize, vertices);  //f
		}
	}
}

void Terrain::makeVertex(int x, int y, std::vector<float> *vertices) {

	double pNoise = noise.noise(x, y, 0.2);
	//x y z position
	vertices->push_back((float)x); //xPos
	vertices->push_back(pNoise); //yPos - always 0 for now. Going to calculate this on GPU - can change to calclaute it here.
	vertices->push_back((float)y); //zPos

   // add texture coords
	vertices->push_back((float)x / (width*stepSize));
	vertices->push_back((float)y / (height*stepSize));


}