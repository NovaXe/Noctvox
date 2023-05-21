#pragma once
#include <gl/glew.h>
#include <gl/glu.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_Image.h>
#include <glm/glm.hpp>
#include <vector>

struct SDL_Deleter {
	void operator()(SDL_Window* p) const {
		SDL_DestroyWindow(p);
	}
	/*void operator()(SDL_GLContext*  p) const {
		SDL_GL_DeleteContext(p);
	}*/
};






using UWindow = std::unique_ptr<SDL_Window, SDL_Deleter>;





std::string readShaderSource(const std::string& path);

void initSDL();


bool initGL();

void printShaderLog(GLuint shader);

void initGame();

void gameLoop();

void render();

void editWorld(glm::vec3 pos, float value);

void processEvents();

void mouseMoved(int xpos, int ypos);

void turnCamera(float pitch_delta, float yaw_delta);

void updateCameraVectors();
