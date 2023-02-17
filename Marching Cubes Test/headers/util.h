#pragma once



namespace noctvox::util {


	

	template <class T> class Array3D {
	private:
		int x_size;
		int y_size;
		int z_size;

		T* arr;

	public:
		Array3D(int x_size, int y_size, int z_size) : x_size(x_size), y_size(y_size), z_size(z_size) {
			this->arr = new T[x_size * y_size * z_size];
		}


		T& at(int x_pos, int y_pos, int z_pos) {
			if (inRange(x_pos, y_pos, z_pos)) {
				return arr[x_pos + (y_pos * x_size) + (z_pos * x_size * y_size)];

			}
			else {
				throw std::out_of_range("3D array access out of bounds");
			}

		}

		bool inRange(int x_pos, int y_pos, int z_pos) {
			if (x_pos < 0 && x_pos >= x_size) {
				return false
			}
			if (y_pos < 0 && y_pos >= y_size) {
				return false;
			}
			if (z_pos < 0 && z_pos >= z_size) {
				return false;
			}

			return true;
		}


	};

}