#pragma once
#include <SDL2/SDL.h>
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include "../headers/util.h"

#include <map>
#include <unordered_map>
#include <glm/glm.hpp>



namespace noctvox::world {
	class Chunk;
	class World;
	class Point;


	class World {
	private:
		int x_size;
		int y_size;
		int z_size;

		std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> loaded_chunk_map;

		

	public:
		
	};



	


	struct Point {
		float value;
		float red;
		float green;
		float blue;


	};

}
