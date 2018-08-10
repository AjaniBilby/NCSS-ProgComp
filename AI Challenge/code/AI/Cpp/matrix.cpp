#include "./matrix.hpp"

class Matrix{
	private:
		std::vector< std::vector<double> > data;

	public:
		unsigned long columns;
		unsigned long rows;

		Matrix(int width, int depth, double fill=0){
			this->columns = width;
			this->rows    = depth;

			this->data.resize(depth);
			for (int i=0; i<depth; i++){
				this->data[i].resize(width, fill);
			}
		};
		Matrix(){
			this->rows      = 0;
			this->columns   = 0;
			this->data.resize(0);
		};




		std::string toString(){
			std::string str = "Matrix";
			str += " (" + std::to_string(this->columns) + ", " + std::to_string(this->rows) + ")";

			str += "{\n";
			for (int y=0; y<this->rows; y++){
				str += "  ";

				for (int x=0; x<this->columns; x++){
					str += std::to_string(this->data[y][x]) + " ";
				}

				str += "\n";
			}
			str += "}";

			return str;
		}




		std::vector<double>& operator[](int y){
			return this->data[y];
		};

		Matrix dot(Matrix other){
			if (this->columns != other.rows){
				std::cerr << "Error: Matrix sizing miss match" << std::endl;
				std::cerr << "  "<<this->columns<<" != " << other.rows << std::endl;
				return *this;
			}

			Matrix out = Matrix(other.columns, this->rows);
			for (int y=0; y<out.rows; y++){
				for (int x=0; x<out.columns; x++){

					for (int i=0; i<other.rows; i++){
						out.data[y][x] += this->data[ y % this->rows ][ i % this->columns ] * other.data[ i % other.rows ][ x % other.columns ];
					}

				}
			}

			return out;
		};




		void resize(int width, int depth, double fill=0){
			this->rows    = depth;
			this->columns = width;

			this->data.resize(depth);
			for (int i=0; i<depth; i++){
				this->data[i].resize(width, fill);
			}
		}
};



bool CheckMatrixLibraryValidity(){
	Matrix a(3, 2);
	a[0][0] = 1;
	a[0][1] = 2;
	a[0][2] = 3;
	a[1][0] = 4;
	a[1][1] = 5;
	a[1][2] = 6;
	Matrix b(2, 3);
	b[0][0] = 7;
	b[0][1] = 8;
	b[1][0] = 9;
	b[1][1] = 10;
	b[2][0] = 11;
	b[2][1] = 12;
	Matrix c = a.dot(b);
	if (c[0][0] != 58){
		return false;
	}
	if (c[0][1] != 64){
		return false;
	}
	if (c[1][0] != 139){
		return false;
	}
	if (c[1][1] != 154){
		return false;
	}

	a = Matrix(3, 1);
	a[0][0] = 3;
	a[0][1] = 4;
	a[0][2] = 2;
	b = Matrix(4, 3);
	b[0][0] = 13;
	b[0][1] = 9;
	b[0][2] = 7;
	b[0][3] = 15;
	b[1][0] = 8;
	b[1][1] = 7;
	b[1][2] = 4;
	b[1][3] = 6;
	b[2][0] = 6;
	b[2][1] = 4;
	b[2][2] = 0;
	b[2][3] = 3;
	c = a.dot(b);
	if (c[0][0] != 83){
		return false;
	}
	if (c[0][1] != 63){
		return false;
	}
	if (c[0][2] != 37){
		return false;
	}
	if (c[0][3] != 75){
		return false;
	}


	return true;
}