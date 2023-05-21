#define FMT_HEADER_ONLY
//#define GLEW_BUILD
#include <fmt/core.h>
#include <fmt/printf.h>
#include "../headers/util.h"
#include "../headers/game.h"
#include "../headers/rendering.h"

#include <gl/glew.h>
//#include <gl/glu.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_Image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <fstream>
#include <string>
#include <memory>



util::Array3D<float> point_field;
GLuint program_id;
SDL_Event e;


glm::vec3 camera_pos;
float camera_yaw;
float camera_pitch;
glm::vec3 camera_front;
glm::vec3 camera_right;
glm::vec3 camera_up;
float light_brightness;



// unique ptr to sdl_window type
UWindow window_instance;
SDL_GLContext g_context;
std::unique_ptr<Mesh> world_mesh;

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


std::string readShaderSource(const std::string & path) {
	std::ifstream ifs(path);
	if (!ifs.is_open()) {
		fmt::printf("Couldn't open the fucking file, fix it: %s\n", path);
		return "";
	}

	std::string content(
		(std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>()
	);

	//const GLchar** source = (const GLchar**) content.c_str();
	//fmt::printf("shader:\n%s", content);

	return content;

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
	//SDL_CaptureMouse(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

// requires float values
	glClearColor(
		100.0f / 255.0f,	// Red
		104.0f / 255.0f,	// Green
		104.0f / 255.0f,	// Blue
		1.0f
	);
	return true;

	// set vertex data or something here, follow lazy foo to continue (SDL and modern OpenGL)
	// need to go to gym and will finish later
	// already taking too long



}


void printShaderLog(GLuint shader)
{
	//Make sure name is shader
	if (glIsShader(shader))
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[maxLength];

		//Get info log
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			//Print Log
			printf("%s\n", infoLog);
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a shader\n", shader);
	}
}


void initGame() {
	point_field = util::Array3D<float>(17, 17, 17);
	world_mesh = std::make_unique<Mesh>();
	light_brightness = 0.5f;

	Shader::shader_map["light_shader"] = std::make_unique<Shader>("shaders/general.vert", "shaders/general.frag");
	Shader::shader_map["line_shader"] = std::make_unique<Shader>("shaders/lines.vert", "shaders/lines.frag");

	camera_pos = { 0,0,0 };
	camera_front = { 0,0,1 };





}


void gameLoop() {
	while (is_running) {
		processEvents();

		render();

	}
}

void render() {
	//glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (world_mesh->needsRegen) {
		world_mesh->update();
	}

	static std::vector<float> cube_vertices = {
		// vertices	Texture Coords	Normal Vectors
		// Back Face
		0, 0, 0,	0.0f, 0.0f,		0.0f,  0.0f, -1.0f,		// Bottom-left
		1, 1, 0,	1.0f, 1.0f,		0.0f,  0.0f, -1.0f,		// top-right
		1, 0, 0,	1.0f, 0.0f,		0.0f,  0.0f, -1.0f,		// bottom-right       
		1, 1, 0,	1.0f, 1.0f,		0.0f,  0.0f, -1.0f,		// top-right
		0, 0, 0,	0.0f, 0.0f,		0.0f,  0.0f, -1.0f,		// bottom-left
		0, 1, 0,	0.0f, 1.0f,		0.0f,  0.0f, -1.0f,		// top-left
		// Front face					   			  
		0, 0, 1,	0.0f, 0.0f,		0.0f,  0.0f,  1.0f,		// bottom-left
		1, 0, 1,	1.0f, 0.0f,		0.0f,  0.0f,  1.0f,		// bottom-right
		1, 1, 1,	1.0f, 1.0f,		0.0f,  0.0f,  1.0f,		// top-right
		1, 1, 1,	1.0f, 1.0f,		0.0f,  0.0f,  1.0f,		// top-right
		0, 1, 1,	0.0f, 1.0f,		0.0f,  0.0f,  1.0f,		// top-left
		0, 0, 1,	0.0f, 0.0f,		0.0f,  0.0f,  1.0f,		// bottom-left
		// Left face								  
		0, 1, 1,	1.0f, 0.0f,	   -1.0f,  0.0f,  0.0f,		// top-right
		0, 1, 0,	1.0f, 1.0f,	   -1.0f,  0.0f,  0.0f,		// top-left
		0, 0, 0,	0.0f, 1.0f,	   -1.0f,  0.0f,  0.0f,		// bottom-left
		0, 0, 0,	0.0f, 1.0f,	   -1.0f,  0.0f,  0.0f,		// bottom-left
		0, 0, 1,	0.0f, 0.0f,	   -1.0f,  0.0f,  0.0f,		// bottom-right
		0, 1, 1,	1.0f, 0.0f,	   -1.0f,  0.0f,  0.0f,		// top-right
		// Right face	  		 	   			   	  
		1, 1, 1,	1.0f, 0.0f,		1.0f,  0.0f,  0.0f,		// top-left
		1, 0, 0,	0.0f, 1.0f,		1.0f,  0.0f,  0.0f,		// bottom-right
		1, 1, 0,	1.0f, 1.0f,		1.0f,  0.0f,  0.0f,		// top-right         
		1, 0, 0,	0.0f, 1.0f,		1.0f,  0.0f,  0.0f,		// bottom-right
		1, 1, 1,	1.0f, 0.0f,		1.0f,  0.0f,  0.0f,		// top-left
		1, 0, 1,	0.0f, 0.0f,		1.0f,  0.0f,  0.0f,		// bottom-left     
		// Bottom face	  		 	   			   	  
		0, 0, 0,	0.0f, 1.0f,		0.0f, -1.0f,  0.0f,		// top-right
		1, 0, 0,	1.0f, 1.0f,		0.0f, -1.0f,  0.0f,		// top-left
		1, 0, 1,	1.0f, 0.0f,		0.0f, -1.0f,  0.0f,		// bottom-left
		1, 0, 1,	1.0f, 0.0f,		0.0f, -1.0f,  0.0f,		// bottom-left
		0, 0, 1,	0.0f, 0.0f,		0.0f, -1.0f,  0.0f,		// bottom-right
		0, 0, 0,	0.0f, 1.0f,		0.0f, -1.0f,  0.0f,		// top-right
		// Top face				  		   		  
		0, 1, 0,	0.0f, 1.0f,		0.0f,  1.0f,  0.0f,		// top-left
		1, 1, 1,	1.0f, 0.0f,		0.0f,  1.0f,  0.0f,		// bottom-right
		1, 1, 0,	1.0f, 1.0f,		0.0f,  1.0f,  0.0f,		// top-right     
		1, 1, 1,	1.0f, 0.0f,		0.0f,  1.0f,  0.0f,		// bottom-right
		0, 1, 0,	0.0f, 1.0f,		0.0f,  1.0f,  0.0f,		// top-left
		0, 1, 1,	0.0f, 0.0f,		0.0f,  1.0f,  0.0f 		// bottom-left        
	};


	auto generateVAO = [](std::vector<float> vertices) mutable->GLuint {
		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		//glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

		//vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		

		//vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//safe to unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//The VAO can also be unbound to prevent other VAO calls from modifying the VAO, doesn't really happen though
		//In order to modify another VAO you need to call glBIndVertexArray so there isn't much reason to unbind the VAO
		glBindVertexArray(0);
		return VAO;
	};
	auto selectionVAO = generateVAO(cube_vertices);


	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(camera_pos, camera_pos + camera_front, {0,1,0});

	static glm::mat4 perspective_projection = glm::perspective(glm::radians(80.0f), ((float)width / (float)height), 0.1f, 500.0f);
	

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, { 1,1,1 });
	static auto lighting_shader = Shader::shader_map["light_shader"].get();
	static auto line_shader = Shader::shader_map["line_shader"].get();

	lighting_shader->use();

	lighting_shader->setMat4("projection", perspective_projection);
	lighting_shader->setMat4("view", view);
	lighting_shader->setMat4("model", model);
	lighting_shader->setVec3("lightPos", { 0,8, 8 });
	lighting_shader->setVec3("lightColor", glm::vec3( 1.0,1.0,1.0) * light_brightness);
	lighting_shader->setVec3("viewPos", camera_pos);



	glBindVertexArray(selectionVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	


	
	glm::vec3 start  = { 0, 0, 0 };
	glm::vec3 y_axis = { 0, 1, 0 };
	glm::vec3 x_axis = { 1, 0, 0 };
	glm::vec3 z_axis = { 0, 0, 1 };

	
	static auto line_renderer = std::make_unique<LineRenderer>(line_shader);
	line_shader->use();
	line_shader->setMat4("projection", perspective_projection);
	line_shader->setMat4("view", view);

	line_renderer->draw(start, x_axis, x_axis );
	line_renderer->draw(start, y_axis, y_axis);
	line_renderer->draw(start, z_axis, z_axis);
	glBindVertexArray(0);




	SDL_GL_SwapWindow(window_instance.get());

}


void editWorld(glm::vec3 pos, float value) {
	point_field.at(pos.x, pos.y, pos.z) = value;

}




void processEvents() {
	glm::vec3 right = glm::cross(camera_front, { 0,1,0 });
	const float movement_factor = 0.1;

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
					{
						camera_pos += camera_front * movement_factor;
					}
					break; 
					case SDLK_a:
					{
						camera_pos -= right * movement_factor;
					}
					break;
					case SDLK_s:
					{
						camera_pos -= camera_front * movement_factor;
					}
					break;
					case SDLK_d:
					{
						camera_pos += right * movement_factor;
					}
					break;
					case SDLK_SPACE:
					{
						camera_pos += glm::vec3(0.0,1.0,0.0) * movement_factor;
					}
					break;
					case SDLK_c:
					{
						camera_pos -= glm::vec3(0.0, 1.0, 0.0) * movement_factor;
					}
				}

			}
			break;
			case SDL_MOUSEMOTION:
			{
				int xpos = e.motion.xrel;
				int ypos = e.motion.yrel;
				
				mouseMoved(xpos, ypos);
			}
			break;
			case SDL_MOUSEWHEEL:
			{
				light_brightness += e.wheel.y *0.01;
				
			}
			break;



		}
	}
}

void mouseMoved(int deltax, int deltay) {
	const float sensitivity = 0.05f;

	//float deltax =  -1 * (xpos - lastx);
	//float deltay =  ypos - lasty;

	deltay = -deltay;


	

	turnCamera(deltay * sensitivity, deltax * sensitivity);

	auto pitch = &camera_pitch;
	auto yaw = &camera_yaw;

	if (*pitch > 89.0f)
		*pitch = 89.0f;
	if (*pitch < -89.0f)
		*pitch = -89.0f;
}

void turnCamera(float pitch_delta, float yaw_delta) {


	camera_pitch += pitch_delta;
	camera_yaw += yaw_delta;

	updateCameraVectors();


}


void updateCameraVectors() {
	camera_front = glm::normalize(glm::vec3(
		cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch)),
		sin(glm::radians(camera_pitch)),
		sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch))
	));
	camera_right = glm::normalize(glm::cross(camera_front, glm::vec3(0.0, 1.0, 0.0)));
	camera_up = glm::normalize(glm::cross(camera_right, camera_front));

}