#pragma once
#include <string>
#include <map>
#include <glm/glm.hpp>
class Shader
{
public:
	void Compile(const std::string& vertexPath, const std::string& fragmentPath);
	void use();
	bool CheckCompilationErrors(unsigned int shader, const std::string& type);

	void setFloat(const std::string& name, float value);
	void setVec2(const std::string& name, const glm::vec2& v);
	void setMat4(const std::string& name, const glm::mat4& mat);
	void setVec3(const std::string& name, const glm::vec3& vec);
	void setInt(const std::string& name, int value);

	void CompileCompute(const std::string& computePath);

	Shader() = default;
	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
private:
	unsigned int ID = 0;
	std::map<std::string, unsigned int> uniformLocation;

	unsigned int GetUniformLocation(const std::string& name);
};


