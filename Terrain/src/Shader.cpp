
#include <shader.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geoPath, const char* tessControl, const char* tessEval)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::string tcCode;
	std::string teCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	std::ifstream tcShaderFile;
	std::ifstream teShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (geoPath != nullptr)
		{
			gShaderFile.open(geoPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
		if (tessControl != nullptr)
		{
			tcShaderFile.open(tessControl);
			std::stringstream tcShaderStream;
			tcShaderStream << tcShaderFile.rdbuf();
			tcShaderFile.close();
			tcCode = tcShaderStream.str();

		
		}
		if (tessEval != nullptr)
		{
			teShaderFile.open(tessEval);
			std::stringstream teShaderStream;
			teShaderStream << teShaderFile.rdbuf();
			teShaderFile.close();
			teCode = teShaderStream.str();
		
		}


	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment,geometry,tc,te;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	if (geoPath != nullptr)
	{
		const char*  gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1,&gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}
	//tess Control
	if (tessControl != nullptr)
	{
		const char* tcShaderCode = tcCode.c_str();
		tc = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tc, 1, &tcShaderCode, NULL);
		glCompileShader(tc);
		checkCompileErrors(tc, "Tess Control");
	}
	//tess eval
	if (tessEval != nullptr)
	{
		const char* teShaderCode = teCode.c_str();
		te = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(te, 1, &teShaderCode, NULL);
		glCompileShader(te);
		checkCompileErrors(te, "Tess Eval");
	}
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geoPath != nullptr)  glAttachShader(ID, geometry);
	if (tessControl != nullptr)  glAttachShader(ID,tc);
	if (tessEval != nullptr)  glAttachShader(ID, te);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geoPath != nullptr)  glDeleteShader(geometry);
	if (tessControl != nullptr)  glDeleteShader(tc);
	if (tessEval != nullptr)  glDeleteShader(te);
	


}

Shader::Shader(const char* computePath)
{
	std::string computeCode;
	std::ifstream cShaderFile;

	cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try
	{
		//opening files
		cShaderFile.open(computePath);
		std::stringstream cShaderStream;
		cShaderStream << cShaderFile.rdbuf();
		cShaderFile.close();
		computeCode = cShaderStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	unsigned int comp;
	const char* cShaderCode = computeCode.c_str();
	comp = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp, 1, &cShaderCode, NULL);
	glCompileShader(comp);

	ID = glCreateProgram();
	glAttachShader(ID, comp);
	glLinkProgram(ID);
	checkCompileErrors(ID, "Compute");
	glDeleteShader(comp);
}

void Shader::use()
{
	glUseProgram(ID);
}

// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

