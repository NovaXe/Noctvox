#pragma once
#include <SDL2/SDL.h>
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include "../headers/util.h"

#include <map>
#include <unordered_map>
#include <glm/glm.hpp>


namespace noctvox::world {

	class Chunk {
	private:
		util::Array3D<Point> scalar_data;


	public:
		std::array<Point, 4> getCell(glm::ivec3 position);
		std::array<Point, 4> getCell(int x_pos, int y_pos, int z_pos);
		


	};
}