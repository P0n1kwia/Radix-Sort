
#include <shader.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
void shader::Compile(const std::string& vertexPath, const std::string& fragmentPath)
{
	bool error = false;
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexFile, fragmentFile;
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream vertexSS, fragmentSS;
		vertexFile.open(vertexPath);
		vertexSS << vertexFile.rdbuf();
		vertexFile.close();

		fragmentFile.open(fragmentPath);
		fragmentSS << fragmentFile.rdbuf();
		fragmentFile.close();

		vertexCode = vertexSS.str();
		fragmentCode = fragmentSS.str();

	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "Failed  to open shader File! " << e.what() << "\n";
	}
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();
	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, nullptr);
	glCompileShader(vertex);
	CheckCompilationErrors(vertex, "VERTEX");

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, nullptr);
	glCompileShader(fragment);
	CheckCompilationErrors(fragment, "FRAGMENT");

	unsigned int temp_ID = glCreateProgram();
	glAttachShader(temp_ID, vertex);
	glAttachShader(temp_ID, fragment);
	glLinkProgram(temp_ID);

	if (!CheckCompilationErrors(temp_ID, "PROGRAM"))
	{
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		
		return;
	}
	else
	{
		glDeleteProgram(ID);
		ID = temp_ID;
		uniformLocation.clear();
		glDeleteShader(vertex);
		glDeleteShader(fragment);

	}



}
void shader::use()
{
	glUseProgram(ID);
}
bool shader::CheckCompilationErrors(unsigned int shader, const std::string& type)
{
	bool flag = true;
	int success;
	char log[1024];
	if (type == "PROGRAM")
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			std::cerr << "Failed to link program!\n";
			glGetProgramInfoLog(shader, 1024, nullptr, log);
			std::cerr << log << std::endl;
			flag = true;
		}
	}
	else
	{
		if (type == "VERTEX")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				std::cerr << "Failed to complie Vertex Shader!\n";
				glGetProgramInfoLog(shader, 1024, nullptr, log);
				std::cerr << log << std::endl;
				flag = true;
			}
		}
		
		else if (type == "FRAGMENT")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				std::cerr << "Failed to complie Fragment Shader!\n";
				glGetProgramInfoLog(shader, 1024, nullptr, log);
				std::cerr << log << std::endl;
				flag = true;
			}
		}	
	}
	return flag;
}
void shader::setFloat(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}
void shader::setVec2(const std::string& name, const glm::vec2& v)
{
	glUniform2fv(GetUniformLocation(name),1, &v[0]);
}
void shader::setMat4(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}
void shader::setInt(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}
void shader::setVec3(const std::string& name, const glm::vec3& vec)
{
	glUniform3fv(GetUniformLocation(name), 1, &vec[0]);
}
shader::~shader()
{
	glDeleteProgram(ID);
}
unsigned int shader::GetUniformLocation(const std::string& name)
{
	if (uniformLocation.find(name) != uniformLocation.end())
	{
		return uniformLocation[name];
	}
	unsigned int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1)
	{
		std::cerr << "Failed to find uniform location : "<<name<<"\n";
	}
	uniformLocation[name] = location;

	return location;
}


