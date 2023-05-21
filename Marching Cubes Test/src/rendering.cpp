#include <headers/rendering.h>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <gl\glew.h>
#include <SDL2/SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>







Mesh::Mesh() {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->buffer_vertex_count * sizeof(Vertex), this->vertices.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(ATTRIB_NORM, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Mesh::expandBuffer() {
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	//GLint size = 0;
	//glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	this->buffer_vertex_count *= 2;
	glBufferData(GL_ARRAY_BUFFER, this->buffer_vertex_count, this->vertices.data(), GL_DYNAMIC_DRAW);

}

void Mesh::update() {
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)0, sizeof(Vertex) * this->vertex_count, this->vertices.data());
	this->vertices.clear();
	this->needsRegen = false;
}


void Mesh::addVertex(Vertex& vertex)
{
	this->vertices.push_back(vertex);
	if (this->vertices.size() > this->buffer_vertex_count) {
		this->expandBuffer();
	}
	this->vertex_count++;
	

}

void Mesh::draw(Shader* shader) {
	shader->use();

}



std::unordered_map<std::string, std::unique_ptr<Shader>> Shader::shader_map;

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
	this->id = this->compile(vertex_path, fragment_path);
}



GLuint Shader::compile(const std::string& vertex_path, const std::string& fragment_path) {
	// Vertex and Fragment shader files
	std::string vertexCode, fragmentCode;
	std::ifstream vertexFile, fragmentFile;

	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// Reads the code from the vertex and fragment shader files
	try {

		vertexFile.open(vertex_path, std::ios_base::in);;
		fragmentFile.open(fragment_path, std::ios_base::in);;
		std::stringstream v_stream, f_stream;

		v_stream << vertexFile.rdbuf();
		f_stream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = v_stream.str();
		fragmentCode = f_stream.str();

	}
	catch (std::ifstream::failure e) {
		fmt::print("ERROR::SHADER::FILE_FAILED_READ");
	}

	// Code for Shader as c style string
	const char* vertexShaderCode = vertexCode.c_str();
	const char* fragmentShaderCode = fragmentCode.c_str();


	GLuint programID, vertex, fragment;
	int success;
	char infoLog[512];

	// Compiles the vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}
	// Compiles the fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertex);
	glAttachShader(programID, fragment);
	glLinkProgram(programID);
	//checks for linking errors
	glGetShaderiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(programID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINK_FAILED\n%s\n", infoLog);
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return programID;
}


void Shader::use() {
		glUseProgram(this->id);
		//CurrentShader = std::make_unique<Shader>(this);
		
	}

	void Shader::setBool(const std::string& name, bool value) const {

	}
	void Shader::setInt(const std::string& name, int value) const {

	}
	void Shader::setFloat(const std::string& name, float vlaue) const {

	}

	void Shader::setVec3(const std::string& name, glm::vec3 vec) const {
		glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(vec));
	}

	void Shader::setMat4(const std::string& name, glm::mat4 mat) const {
		glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}