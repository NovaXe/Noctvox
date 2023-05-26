#pragma once

#include <SDL2/SDL.h>
#include <gl\glew.h>
#include <SDL2/SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>



// Wrapper for opengl shit
// I will design this to work in a way that would let me swap apis if I wanted to
// Keep everything adstracted away


class Shader;
class Mesh;

enum VertexAttrib {
	ATTRIB_POS,
	ATTRIB_NORM,
	ATTRIB_UV
};


struct Vertex {
	Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv) : pos(pos), normal(normal), uv(uv) {}
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Mesh {
public:
	Mesh();
	void update();
	void addVertex(Vertex& vertex);
	void draw(Shader* shader);
	bool isOld = true;


private:
	void expandBuffer();
	GLint buffer_vertex_count = 1536;
	int vertex_count = 0;
	

	std::vector<Vertex> vertices;
	GLuint vao = 0;
	GLuint vbo = 0;
};



class Shader {
private:
	GLuint compile(const std::string& vertex_path, const std::string& fragment_path);
	GLuint id;
public:
	Shader(const std::string& vertex_path, const std::string& fragment_path);
	//Shader(const std::string& vertex_path, const std::string& fragment_path);
	Shader() = default;

	static std::unordered_map<std::string, std::unique_ptr<Shader>> shader_map;

	void use();

	void setBool(const std::string& name, bool value) const;

	void setInt(const std::string& name, int value) const;

	void setFloat(const std::string& name, float value) const;

	void setVec3(const std::string& name, glm::vec3 vec) const;

	void setMat4(const std::string& name, glm::mat4 mat) const;
};


class LineRenderer {
public:
	LineRenderer(Shader* shader);
	~LineRenderer();
	void draw(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
private:
	GLuint vao;
	Shader* shader;

};