#include "Octree.h"



Octree::Octree(int size, glm::vec3 pos, Terrain * t)
{
	_size = size;
	_position = pos;
	_terrain = t;
	/*_terrain->points.push_back(_position - glm::vec3(-5.0,0.0,));
	_terrain->points.push_back(_position - glm::vec3(-_size, 0.0, 0.0));
	_terrain->points.push_back(_position - glm::vec3(0.0, 0.0, -_size));
	_terrain->points.push_back(_position - glm::vec3(_size, 0.0, _size));
	_terrain->points.push_back(_position - glm::vec3(_size, 0.0, 0.0));
	_terrain->points.push_back(_position - glm::vec3(0.0, 0.0, 0.0));*/
}


Octree::~Octree()
{
}

int Octree::generate()
{
	glm::vec3 tmpPos;
	bool inside[8];
	int retour = 0;
	int nb = 0;
	glm::vec3(pos);
	float cubeSize = 2.0f;
	float div3D = 10.0;

	float amplitude = 50;

	//std::cout << _size << std::endl;
	
	double min_size = 6.0;
	if (_size <= cubeSize) {
		for (int i = 0; i < 8; i++) {
			pos = getCorner(i, _position, _size);
			if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * 50.0) {
				bool b = Perlin3D(pos.x / 10.0, pos.y / 10.0, pos.z / 10.0) < 0.7;
				if (b) nb++;
			}
		}

		if (nb < 8 && nb > 0) {
			int nb2(0), nb3(0), nb4(0);
			//bas
			glm::vec3 center2 = _position + glm::vec3(cubeSize, 0.0, 0.0);
			glm::vec3 center3 = _position + glm::vec3(cubeSize, 0.0, cubeSize);
			glm::vec3 center4 = _position + glm::vec3(0.0, 0.0, cubeSize);

			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center2, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb2++;
				}
			}

			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center3, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb3++;
				}
			}

			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center4, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb4++;
				}
			}

			if (nb > 0 && nb < 8 && nb2 >0 && nb2 < 8 && nb3 >0 && nb3 < 8 && nb4 >0 && nb4 < 8) {
				_terrain->points.push_back(voxelValue(_position));
				_terrain->points.push_back(voxelValue(center2));
				_terrain->points.push_back(voxelValue(center3));
				_terrain->points.push_back(voxelValue(_position));
				_terrain->points.push_back(voxelValue(center3));
				_terrain->points.push_back(voxelValue(center4));
			}



			//gauche
			nb2 = 0; nb3 = 0; nb4 = 0;
			center2 = _position + glm::vec3(cubeSize, 0.0, 0.0);
			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center2, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb2++;
				}
			}
			center3 = _position + glm::vec3(cubeSize, cubeSize, 0.0);
			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center3, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb3++;
				}
			}
			center4 = _position + glm::vec3(0.0, cubeSize, 0.0);
			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center4, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb4++;
				}
			}
			if (nb > 0 && nb < 8 && nb2 >0 && nb2 < 8 && nb3 >0 && nb3 < 8 && nb4 >0 && nb4 < 8) {
				_terrain->points.push_back(voxelValue(_position));
				_terrain->points.push_back(voxelValue(center2));
				_terrain->points.push_back(voxelValue(center3));
				_terrain->points.push_back(voxelValue(_position));
				_terrain->points.push_back(voxelValue(center3));
				_terrain->points.push_back(voxelValue(center4));
			}

			//droite
			nb2 = 0; nb3 = 0; nb4 = 0;
			center2 = _position + glm::vec3(0.0, 0.0, cubeSize);
			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center2, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb2++;
				}
			}
			center3 = _position + glm::vec3(0.0, cubeSize, cubeSize);
			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center3, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb3++;
				}
			}
			center4 = _position + glm::vec3(0.0, cubeSize, 0.0);
			for (int i = 0; i < 8; i++) {
				pos = getCorner(i, center4, cubeSize);
				if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
					bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
					b = true;
					if (b) nb4++;
				}
			}
			if (nb > 0 && nb < 8 && nb2 >0 && nb2 < 8 && nb3 >0 && nb3 < 8 && nb4 >0 && nb4 < 8) {
				_terrain->points.push_back(voxelValue(_position));
				_terrain->points.push_back(voxelValue(center2));
				_terrain->points.push_back(voxelValue(center3));
				_terrain->points.push_back(voxelValue(_position));
				_terrain->points.push_back(voxelValue(center3));
				_terrain->points.push_back(voxelValue(center4));
			}
			/*_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, -0.5f) * cubeSize);
			_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, -0.5f) * cubeSize);
			_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, -0.5f) * cubeSize);*/
		}/*
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, -0.5f));

		/*_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, 0.5f));

		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, 0.5f));

		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, 0.5f));

		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, -0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, -0.5f, -0.5f));

		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, -0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, 0.5f));
		_terrain->points.push_back(_position + glm::vec3(-0.5f, 0.5f, -0.5f));*/
	
		return 1;
	}

	_haveChilds = true;

	
	for (int i = 0; i < 8; i++) {
		pos = getCorner(i, _position, _size);
		if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
			bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7;
			b = true;
			if (b) nb++;
		}
	}
	if ((nb > 0 && nb < 8)/*|| (_size > 8  && _position.y > - 20 && _position.y < 90.0)*/) {
		Octree * e1 = new Octree(_size / 2, _position + glm::vec3(_size / 4.0, _size / 4.0, _size / 4.0), _terrain);
		e1->generate();
		delete(e1);
		Octree * e2 = new Octree(_size / 2, _position + glm::vec3(_size / 4.0, _size / 4.0, -_size / 4.0), _terrain);
		e2->generate();
		delete(e2);
		Octree *e3 = new Octree(_size / 2, _position + glm::vec3(-_size / 4.0, _size / 4.0, _size / 4.0), _terrain);
		e3->generate();
		delete(e3);
		Octree *e4 = new Octree(_size / 2, _position + glm::vec3(-_size / 4.0, _size / 4.0, -_size / 4.0), _terrain);
		e4->generate();
		delete(e4);
		Octree *e5 = new Octree(_size / 2, _position + glm::vec3(_size / 4.0, -_size / 4.0, _size / 4.0), _terrain);
		e5->generate();
		delete(e5);
		Octree *e6 = new Octree(_size / 2, _position + glm::vec3(_size / 4.0, -_size / 4.0, -_size / 4.0), _terrain);
		e6->generate();
		delete(e6);
		Octree *e7 = new Octree(_size / 2, _position + glm::vec3(-_size / 4.0, -_size / 4.0, _size / 4.0), _terrain);
		e7->generate();
		delete(e7);
		Octree *e8 = new Octree(_size / 2, _position + glm::vec3(-_size / 4.0, -_size / 4.0, -_size / 4.0), _terrain);
		e8->generate();
		delete(e8);
	}
	//fbm(pos.x / 30.0, pos.z / 30.0) * amplitude;
	
	// Les enfants :
	// bas gauche derrière
	
	// bas droite derrière
	
	// bas gauche devant

	// bas droite devant

	// haut gauche derriere
	

	// haut droite derriere
	

	// haut gauche devant
	

	// haut droite devant
	
	// Cube representation (top view)
	// bottom :		top :
	// |0|1|		|4|5|
	// |2|3|		|6|7|
	

	//generate needed childs
	

	return retour;

	//_enfants = new Octree[8];
}

void Octree::produceGeometry(Terrain * t)
{

}



glm::vec3 Octree::getCorner(int i, glm::vec3 position, float u_cubeSize)
{
	glm::vec3 pos;
	switch (i) {
	case 0:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, -u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 1:
		pos = position + glm::vec3(u_cubeSize / 2.0f, -u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 2:
		pos = position + glm::vec3(u_cubeSize / 2.0f, -u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	case 3:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, -u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	case 4:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 5:
		pos = position + glm::vec3(u_cubeSize / 2.0f, u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 6:
		pos = position + glm::vec3(u_cubeSize / 2.0f, u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	case 7:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	}
	return pos;
}

glm::vec3 Octree::voxelValue(glm::vec3 center)
{
	bool inside[8];
	double val[8];
	std::vector<glm::vec3> intersectionPt;
	std::vector<glm::vec3> intersectionNorm;
	double a(50), b(50), c(50), r(40);
	double cubeSize = 2.0;

	for (int i = 0; i < 8; i++) {
		glm::vec3 pos = getCorner(i, center, cubeSize);


		float div3D = 10.0;
		double p3D = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D);
		val[i] = fbm(pos.x / 30.0, pos.z / 30.0) * 50.0;
		inside[i] = pos.y < val[i];
		val[i] = val[i] - pos.y;

		if (p3D > 0.7) {
			inside[i] = p3D < 0.7;
			val[i] = p3D - 0.7;
		}
	}

	int indice1(0), indice2(1);
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;

		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 3; indice2 = 2;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 4; indice2 = 5;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 7; indice2 = 6;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 0; indice2 = 4;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 5; indice2 = 1;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 6; indice2 = 2;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 7; indice2 = 3;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 0; indice2 = 3;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 1; indice2 = 2;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 5; indice2 = 6;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 4; indice2 = 7;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}


	glm::vec3 retour = glm::vec3(0.0);

	glm::mat4 A = glm::mat4(0.0);
	glm::vec4 B = glm::vec4(0.0);



	for (glm::vec3 pt : intersectionPt)
		retour += pt;


	retour /= intersectionPt.size();


	if (retour.x > center.x + cubeSize || retour.x < center.x - cubeSize) retour.x = center.x;
	if (retour.z > center.z + cubeSize || retour.z < center.z - cubeSize) retour.z = center.z;
	//if (retour.y > center.y + cubeSize || retour.y < center.y - cubeSize) return center;


	return retour;
}
