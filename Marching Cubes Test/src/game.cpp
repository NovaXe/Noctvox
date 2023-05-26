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
#include <glm/gtc/noise.hpp>

#include <array>
#include <fstream>
#include <string>
#include <memory>


std::unique_ptr<util::Array3D<float>> point_field;
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
	glCullFace(GL_FRONT);
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
	const int world_size = 64;

	point_field = std::make_unique<util::Array3D<float>>(world_size+1, world_size+1, world_size+1);
	
	for (int z = 0; z < point_field->zSize() - 1; z++) {
		for (int y = 0; y < point_field->ySize() - 1; y++) {
			for (int x = 0; x < point_field->xSize() - 1; x++) {
				const float div = 16.f;
				auto noise = glm::perlin(glm::vec3(x/ div, y/ div, z/ div));
				noise = abs(noise);
				point_field->at(x, y, z) = noise;


			}
		}
	}


	//point_field->at(2, 2, 2) = 1.0;
	//point_field->at(2, 2, 3) = 0.6;
	//point_field->at(2, 2, 4) = 0.6;
	//point_field->at(2, 2, 5) = 0.6;
	//point_field->at(3, 2, 5) = 0.6;
	//point_field->at(3, 3, 5) = 0.6;
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
	//model = glm::translate(model, { 1,1,1 });
	static auto lighting_shader = Shader::shader_map["light_shader"].get();
	static auto line_shader = Shader::shader_map["line_shader"].get();

	// Check if the chunk mesh needs to be updated (mesh->isOld)
	// IE it isn't in sync with the field data
	// If so rebuild the mesh
	// otherwise do nothing and draw with the existing mesh
	// after rebuilding the mesh update the vertex array and mark it as not old
	// mark the mesh as old whenever the chunk is modified

	lighting_shader->use();

	lighting_shader->setMat4("projection", perspective_projection);
	lighting_shader->setMat4("view", view);
	lighting_shader->setMat4("model", model);
	lighting_shader->setVec3("lightPos", { -8,32, 32 });
	lighting_shader->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0)* light_brightness);
	lighting_shader->setVec3("viewPos", camera_pos);


	checkMesh();
	world_mesh->draw(lighting_shader);




	/*lighting_shader->use();

	lighting_shader->setMat4("projection", perspective_projection);
	lighting_shader->setMat4("view", view);
	lighting_shader->setMat4("model", model);
	lighting_shader->setVec3("lightPos", { 0,8, 8 });
	lighting_shader->setVec3("lightColor", glm::vec3( 1.0,1.0,1.0) * light_brightness);
	lighting_shader->setVec3("viewPos", camera_pos);*/



	/*glBindVertexArray(selectionVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);*/

	


	
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
	point_field->at(pos.x, pos.y, pos.z) = value;
	
}

void checkMesh() {
	if (world_mesh->isOld) {
		buildMesh();
		world_mesh->update();
		world_mesh->isOld = false;
	}
}

/*
	   4--------5     *---4----*
	  /|       /|    /|       /|
	 / |      / |   7 |      5 |
	/  |     /  |  /  8     /  9
   7--------6   | *----6---*   |
   |   |    |   | |   |    |   |
   |   0----|---1 |   *---0|---*
   |  /     |  /  11 /     10 /
   | /      | /   | 3      | 1
   |/       |/    |/       |/
   3--------2     *---2----*

*/


void buildMesh() {
	for (int z = 0; z < point_field->zSize() - 1; z++) {
		for (int y = 0; y < point_field->ySize() - 1; y++) {
			for (int x = 0; x < point_field->xSize() - 1; x++) {
	//for (int x = 0; x < point_field->xSize() - 1; x++) {
	//	for (int y = 0; y < point_field->ySize() - 1;  y++) {
	//		for (int z = 0; z < point_field->zSize() - 1; z++) {

				std::array<float, 8> cell = fetchCell(point_field.get(), x, y, z);
				static float isolevel = 0.5;

				

				unsigned int cube_index = 0;
				if (cell[0] < isolevel) cube_index |= 1;
				if (cell[1] < isolevel) cube_index |= 2;
				if (cell[2] < isolevel) cube_index |= 4;
				if (cell[3] < isolevel) cube_index |= 8;
				if (cell[4] < isolevel) cube_index |= 16;
				if (cell[5] < isolevel) cube_index |= 32;
				if (cell[6] < isolevel) cube_index |= 64;
				if (cell[7] < isolevel) cube_index |= 128;

				// 12 bit number that contains info on what edges are split by the surface
				// 1 bit for every edge in the cell
				// 

				std::array<glm::vec3, 12> vert_list = {};

 				auto edge_flags = edgeTable[cube_index];
				if (edge_flags == 0) {
					continue;
				}
				

				if (edge_flags & 1) 
					vert_list[0] = interpolateEdgePoint(isolevel, cell_points[0], cell_points[1], cell[0], cell[1]);
				if (edge_flags & 2)
					vert_list[1] = interpolateEdgePoint(isolevel, cell_points[1], cell_points[2], cell[1], cell[2]);
				if (edge_flags & 4)
					vert_list[2] = interpolateEdgePoint(isolevel, cell_points[2], cell_points[3], cell[2], cell[3]);
				if (edge_flags & 8)
					vert_list[3] = interpolateEdgePoint(isolevel, cell_points[3], cell_points[0], cell[3], cell[0]);
				if (edge_flags & 16)
					vert_list[4] = interpolateEdgePoint(isolevel, cell_points[4], cell_points[5], cell[4], cell[5]);
				if (edge_flags & 32)
					vert_list[5] = interpolateEdgePoint(isolevel, cell_points[5], cell_points[6], cell[5], cell[6]);
				if (edge_flags & 64)
					vert_list[6] = interpolateEdgePoint(isolevel, cell_points[6], cell_points[7], cell[6], cell[7]);
				if (edge_flags & 128)
					vert_list[7] = interpolateEdgePoint(isolevel, cell_points[7], cell_points[4], cell[7], cell[4]);
				if (edge_flags & 256)
					vert_list[8] = interpolateEdgePoint(isolevel, cell_points[0], cell_points[4], cell[0], cell[4]);
				if (edge_flags & 512)
					vert_list[9] = interpolateEdgePoint(isolevel, cell_points[1], cell_points[5], cell[1], cell[5]);
				if (edge_flags & 1024)
					vert_list[10] = interpolateEdgePoint(isolevel, cell_points[2], cell_points[6], cell[2], cell[6]);
				if (edge_flags & 2048)
					vert_list[11] = interpolateEdgePoint(isolevel, cell_points[3], cell_points[7], cell[3], cell[7]);

				//std::vector<glm::vec3> points;
				
				if (z < 13) {
					int i = 0;
					//continue;
				}


				// shift the vertices to where the should be relative to the point field
				// before this they are in local space
				auto current_pos = glm::vec3(x, y, z);
				for (auto& vert : vert_list) {
					vert = vert + current_pos;
				}

				// output vertices
				std::vector<Vertex> vertices;

				

				auto vertex_sequence = triTable[cube_index];
				int number_of_triangles = 0;
				for (int i = 0; vertex_sequence[i] != -1; i += 3, number_of_triangles++) {

					glm::vec3 triangle[] = {
						vert_list[vertex_sequence[i]],
						vert_list[vertex_sequence[i + 1]],
						vert_list[vertex_sequence[i + 2]]
					};

					if (z == 1) {
						int i = 0;
					}


					glm::vec3 face_normal = glm::cross(triangle[1] - triangle[0], triangle[2] - triangle[0]);
					face_normal = glm::normalize(face_normal); 
					//face_normal = -1 * face_normal;

					// need to figure out how to calculate the normals that face normal to the surface for a triangle
					// obv cross the 
					for (auto& pos : triangle) {
						Vertex v(pos, face_normal, { 0, 0 });
						vertices.push_back(v);
					}
					


					/*points.push_back(vert_list[vertex_sequence[i]]);
					points.push_back(vert_list[vertex_sequence[i + 1]]);
					points.push_back(vert_list[vertex_sequence[i + 2]]);*/

				}

				/*for (auto& point : points) {
					Vertex vertex;
					vertex.pos = point;
					vertex.normal = {0,0,0}

					world_mesh->addVertex()
				}*/

				

				for (auto& vertex : vertices) {
					world_mesh->addVertex(vertex);
					//fmt::printf("z=%d", z);
				}
			}
		}
	}
}


glm::vec3 interpolateEdgePoint(float isolevel, glm::ivec3 p1, glm::ivec3 p2, float p1_val, float p2_val) {
	
	glm::vec3 point;
	if (abs(isolevel - p1_val) < 0.00001) {
		return p1;
	}
	if (abs(isolevel - p2_val) < 0.00001) {
		return p2;
	}
	if (abs(p1_val - p2_val) < 0.00001) {
		return p1;
	}

	float mu = (isolevel - p1_val) / (p2_val - p1_val);
	point.x = p1.x + mu * (p2.x - p1.x);
	point.y = p1.y + mu * (p2.y - p1.y);
	point.z = p1.z + mu * (p2.z - p1.z);

	return point;
}




std::array<float, 8> fetchCell(util::Array3D<float>* field, int x, int y, int z) {
	std::array<float, 8> vals;

	for (int i = 0; i < 8; i++) {
		auto point_offset = cell_points[i];
		vals[i] = field->at(x + point_offset.x, y+point_offset.y, z+point_offset.z);
	}
	return vals;
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