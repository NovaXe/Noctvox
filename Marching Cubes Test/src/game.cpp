#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include "../headers/util.h"

#include "../headers/game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <gl/glew.h>
#include <gl/glu.h>

#include <string>


util::Array3D<int> point_field(17, 17, 17);
GLuint shader_id;



// world made up of an even number of points must have an odd number of psudo voxels




// http://paulbourke.net/geometry/polygonise/
// Gonna use that


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




void initSDL() {

}

void initGL() {

}




void gameLoop() {
	static bool is_running = true;
	while (is_running) {



	}
}

