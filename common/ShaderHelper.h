#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class ShaderHelper
{
public:

	/*
		Shader helper constructor doesn't do much
	*/
	ShaderHelper()
	{
	}

	/*
		Reads shader code as text with iostream
		Inputs:
			1) Filname
			2) Shader type we want to compile
		Outputs:
			1) Compile shader
	*/
	GLuint readShader(const char* filename, GLenum shaderType)
	{
		std::string shaderCode;

		// Read shader code into string 
		std::ifstream file_stream(filename, std::ios::in);
		if (file_stream.is_open()) {
			shaderCode = std::string(std::istreambuf_iterator<char>(file_stream),
				std::istreambuf_iterator<char>());
			file_stream.close();
		}
		else {
			std::cout << "Could not open " << filename << std::endl;
			return 0;
		}

		// Compile shader
		GLuint shader = glCreateShader(shaderType);
		const char* shaderCStrCode = shaderCode.c_str();
		GLint shaderSourceLength = static_cast<GLuint>(strlen(shaderCStrCode));
		glShaderSource(shader, 1, &shaderCStrCode, &shaderSourceLength);
		glCompileShader(shader);

		return shader;
		
	}

	/*
		Compiling and linking shaders
		Inputs:
			1) Vertex shader filename
			2) Fragment shader filename
		Outputs:
			1) Linked Program
	*/
	GLuint compileShaders(const char* vShaderFilename, const char* fShaderFilename) {
		// Compile both shaders
		GLuint vShader = compileVShader(vShaderFilename);
		GLuint fShader = compileFShader(fShaderFilename);

		// Compile and link our shaders
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vShader);
		glAttachShader(shaderProgram, fShader);
		glLinkProgram(shaderProgram);

		// Check linking success
		int  success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
			return 0;
		}

		glDeleteShader(vShader);
		glDeleteShader(fShader);

		return shaderProgram;
	}

	/*
		Compiling vertex shader
		Inputs:
			1) Vertex shader filename
		Outputs:
			1) Vertex shader id
	*/
	GLuint compileVShader(const char* vShaderFilename) {
		// Compile vertex shader
		GLuint vShader = this->readShader(vShaderFilename, GL_VERTEX_SHADER);

		// Check success of vShader compilation
		int  success;
		char infoLog[512];
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			glDeleteShader(vShader);
			return 0;
		}

		return vShader;
	}

	/*
		Compiling Fragment shader
		Inputs:
			1) Fragment shader filename
		Outputs:
			1) Fragment shader id
	*/
	GLuint compileFShader(const char* fShaderFilename) {
		// Compile vertex shader
		GLuint fShader = this->readShader(fShaderFilename, GL_FRAGMENT_SHADER);

		// Check success of fShader compilation
		int  success;
		char infoLog[512];
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			glDeleteShader(fShader);
			return 0;
		}

		return fShader;
	}
	
};
