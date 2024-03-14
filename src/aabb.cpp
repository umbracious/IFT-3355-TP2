#include "aabb.h" 

// @@@@@@ VOTRE CODE ICI
// Implémenter l'intersection d'un rayon avec un AABB dans l'intervalle décrit.
bool AABB::intersect(Ray ray, double t_min, double t_max)  {
       return true;
};

// @@@@@@ VOTRE CODE ICI
// Implémenter la fonction qui permet de trouver les 8 coins de notre AABB.
std::vector<double3> retrieve_corners(AABB aabb) {

	std::vector<double3> aabb_corners;

	aabb_corners.push_back(aabb.min); 									 // Min x, y, z
	aabb_corners.push_back(double3{aabb.max.x, aabb.min.y, aabb.min.z}); // Max x, min y, min z
	aabb_corners.push_back(double3{aabb.min.x, aabb.max.y, aabb.min.z}); // Min x, max y, min z
	aabb_corners.push_back(double3{aabb.max.x, aabb.max.y, aabb.min.z}); // Max x, max y, min z
	aabb_corners.push_back(double3{aabb.min.x, aabb.min.y, aabb.max.z}); // Min x, min y, max z
	aabb_corners.push_back(double3{aabb.max.x, aabb.min.y, aabb.max.z}); // Max x, min y, max z
	aabb_corners.push_back(double3{aabb.min.x, aabb.max.y, aabb.max.z}); // Min x, max y, max z
	aabb_corners.push_back(aabb.max); 									 // Max x, y, z

	return aabb_corners;


};

// @@@@@@ VOTRE CODE ICI
// Implémenter la fonction afin de créer un AABB qui englobe tous les points.
AABB construct_aabb(std::vector<double3> points) {
	double3 min_point = {-DBL_MAX,-DBL_MAX,-DBL_MAX};
	double3 max_point = {DBL_MAX,DBL_MAX,DBL_MAX};

	for (auto p : points) {
		min_point = std::min(min_point, p);
		max_point = std::max(max_point, p);
	}
	
	AABB aabb{min_point,max_point};
	return aabb;
};

AABB combine(AABB a, AABB b) {
	return AABB{min(a.min,b.min),max(a.max,b.max)};
};

bool compare(AABB a, AABB b, int axis){
	return a.min[axis] < b.min[axis];
};