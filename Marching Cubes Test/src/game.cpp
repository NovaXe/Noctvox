#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/printf.h>
#include "../headers/util.h"

#include "../headers/game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <gl/glew.h>
#include <gl/glu.h>

#include <fstream>
#include <string>
#include <memory>



util::Array3D<int> point_field(17, 17, 17);
GLuint program_id;
SDL_Event e;

// unique ptr to sdl_window type
UWindow window_instance;
SDL_GLContext g_context;


const int width = 800;
const int height = 600;

bool is_running = true;




// http://paulbourke.net/geometry/polygonise/
// Gonna use that

// world made up of an even number of points must have an odd number of psudo voxels
// One less voxel than number of points

// Have a game world that contains a point field
// Function to access elements in the point field\
// Create a function that snags the 8 points that make up a cube and put them in 
// order for processing of an individual "voxel" when marching

// TODO
// Initialize SDL2 and opengl
// setup mesh generator to render the world mesh
// setup marching cubes algorithm via use of marching and lookup tables

// render the mesh using opegnl shader
// projections and such neccessary

// create camera controller
// Don't utilize code from former projects
// only tutorials from learn opengl and lazyfoo
// must relearn all


const GLchar** readShaderSource(const std::string& path) {
	std::ifstream ifs(path);
	std::string content;
	content.assign(
		std::istreambuf_iterator<char>(ifs),
		std::istreambuf_iterator<char>()
	);
	const GLchar** source = (const GLchar**) content.c_str();
	return source;

}



void initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fmt::print("failed to initialize sdl");
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);




	window_instance = UWindow(
		SDL_CreateWindow("Noctvox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN),
		SDL_Deleter()
	);

	g_context = SDL_GL_CreateContext(window_instance.get());
	if (g_context == NULL) {
		fmt::print("Failed to create opengl context");
		return;
	}

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		fmt::printf("Error intializing GLEW! %s\n", (char*)glewGetErrorString(glewError));

	}

	if (!initGL()) {
		fmt::printf("Unable to initialize OpenGL!\n");
	}



}

bool initGL() {
	program_id = glCreateProgram();
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);



	auto vertex_shader_source = readShaderSource("shader/general.vert.glsl");
	auto fragment_shader_source = readShaderSource("shader/general.frag.glsl");
	

	glShaderSource(vertex_shader, 1, (const GLchar**)vertex_shader_source, NULL);

	glCompileShader(vertex_shader);

	GLint vertex_shader_compiled = GL_FALSE;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compiled);
	if (vertex_shader_compiled != GL_TRUE) {
		fmt::printf("Unable to compile vertex shader %d!\n", vertex_shader);
		return false;
	}

	glAttachShader(program_id, vertex_shader);



	glShaderSource(fragment_shader, 1, (const GLchar**)fragment_shader_source, NULL);

	glCompileShader(fragment_shader);

	GLint fragment_shader_compiled = GL_FALSE;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_compiled);
	if (fragment_shader_compiled != GL_TRUE) {
		fmt::printf("Unable to compile vertex shader %d!\n", fragment_shader);
		
		return false;
	}

	glAttachShader(program_id, fragment_shader);
	

	glLinkProgram(program_id);
	GLint program_success = GL_TRUE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &program_success);
	if (program_success != GL_TRUE) {
		fmt::printf("Error linking program %d!\n", program_id);
		return false;
	}


	// set vertex data or something here, follow lazy foo to continue (SDL and modern OpenGL)
	// need to go to gym and will finish later
	// already taking too long



}




void gameLoop() {
	while (is_running) {



	}
}






void processEvents() {
	while (SDL_PollEvent(&e) != 0) {
		switch (e.type) {
		case SDL_QUIT:
			is_running = false;
		break;
		case SDL_KEYDOWN:
		{
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				is_running = false;
			break;
			case SDLK_w:
			{break; }
			case SDLK_a:
			{break; }
			case SDLK_s:
			{break; }
			case SDLK_d:
			{break; }

			}

		}



		}
	}
}