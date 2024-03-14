#include "object.h"

// Fonction retournant soit la valeur v0 ou v1 selon le signe.
int rsign(double value, double v0, double v1) {
	return (int(std::signbit(value)) * (v1-v0)) + v0;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection d'une sphère.
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Sphere::local_intersect(Ray ray, 
							 double t_min, double t_max, 
							 Intersection *hit) 
{	

	// Interesction formula taken from: https://math.stackexchange.com/questions/1939423/calculate-if-vector-intersects-sphere
	// Q = P - C = P because C = (0,0,0)
	double a = length2(ray.direction);
	double b = 2 * dot(ray.direction, ray.origin);
	double c = length2(ray.origin) - radius*radius;
	double discriminant = b*b - 4*a*c;

	if (length(ray.origin)<=radius){ // Ray starts from inside sphere
		return false;
	}

	if (discriminant<0){ // No intersection
		return false;
	}

	else if (discriminant == 0){ // 1 intersection point
		// Quadratic formula
		double t = (-b + sqrt(discriminant))/(2*a);
		if (t > t_min && t < t_max){
			hit->position = ray.origin + ray.direction*t;
			hit->depth = t;
			hit->normal = normalize(hit->position);
		}
		return true;
	}

	else{ // 2 intersection points
		// Quadratic formula
		double t1 = (-b + sqrt(discriminant))/(2*a); 
		double t2 = (-b - sqrt(discriminant))/(2*a);

		// Set hit parameters
		if (t1 > t_min && t1 < t_max){
			hit->position = ray.origin + ray.direction*t1;
			hit->depth = t1;
			hit->normal = normalize(hit->position);
		}

		// Only do this if t1 fails as t1 always smaller than t2
		else if (t2 > t_min && t1 < t_max){
			hit->position = ray.origin + ray.direction*t2;
			hit->depth = t2;
			hit->normal = normalize(hit->position);
		}

		return true;
	}
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour la sphère.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire (comme ici).
AABB Sphere::compute_aabb() {
	return Object::compute_aabb();
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un quad (rectangle).
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Quad::local_intersect(Ray ray, 
							double t_min, double t_max, 
							Intersection *hit)
{
	// Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html

	double3 normal{0,0,1}; // Z+
	double denom = dot(normal, ray.direction);

	if (denom > EPSILON){ // Ray and square not perpendicular
		double t = dot(ray.origin, normal)/denom;

		if (t>t_min && t>t_max){
			double3 p = ray.origin + t*ray.direction; // Intersection on plane
			if (abs(p[0])<half_size && abs(p[1])<half_size){ // Intersects
				hit->position =p;
				hit->depth = t;
				hit->normal = normal;
				return true;
			}
		}
	}
	
	return false;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le quad (rectangle).
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Quad::compute_aabb() {
	return Object::compute_aabb();
	//return Object::compute_aabb();
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un cylindre.
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Cylinder::local_intersect(Ray ray, 
							   double t_min, double t_max, 
							   Intersection *hit)
{	// Source = https://stackoverflow.com/questions/73866852/ray-cylinder-intersection-formula

	double3 C = {0,half_height,0};
	double3 V = {0,-1,0};

	// Cylinder cap params
	double3 normal{0,1,0};
	double denom = dot(normal, ray.direction);
	
	// Cylinder side params
	double a = length2(ray.direction) - pow(dot(ray.direction,V),2);
	double b = 2*(dot(ray.direction,ray.origin) - dot(ray.direction,V)*dot(ray.origin,V));
	double c = length2(ray.origin) - pow(dot(ray.origin,V),2) - pow(radius,2);
	double discriminant = b*b - 4*a*c;

	bool intersects = false;
	double t=t_max;

	if (abs(ray.origin[1])<=half_height && sqrt(pow(ray.origin[0],2)+pow(ray.origin[2],2))<=radius){ // Ray origin inside cylinder
		return false;
	}

	if (denom>EPSILON){ // Intersection with cylinder caps
		// Top cap
		double3 CR0 = C - ray.origin;
		t = dot(CR0, normal)/denom;

		if (t>t_min && t<t_max){
			intersects = true;
			hit->position = ray.origin + ray.direction*t;
			hit->depth = t;
			hit->normal = normal;
		}

		//Bottom cap
		CR0 = -C - ray.origin;
		double tmp_t = dot(CR0, -normal)/denom;

		if (tmp_t>t_min && tmp_t<t_max && tmp_t<t){
			t = tmp_t;
			intersects = true;
			hit->position = ray.origin + ray.direction*t;
			hit->depth = t;
			hit->normal = -normal;
		}
	}

	// Cylinder side
	if (discriminant<0){ // No intersections
		return intersects;
	}

	else if (discriminant == 0){ // 1 intersection point
		// Quadratic formula
		double tmp_t = (-b + sqrt(discriminant))/(2*a);
		double3 hit_pos = ray.origin + ray.direction*tmp_t;

		if (abs(hit_pos[1])<=half_height){ // Hit is on cylinder
			if (tmp_t > t_min && tmp_t < t_max && tmp_t<t){
				t = tmp_t;
				intersects = true;
				hit->position = ray.origin + ray.direction*t;
				hit->depth = t;
				normal = {hit_pos[0],0,hit_pos[2]};
				hit->normal = normalize(normal);
			}
		}

		
	}

	else if (discriminant > 0){ // 2 intersection points
		// Quadratic formula
		double t1 = (-b + sqrt(discriminant))/(2*a); 
		double t2 = (-b - sqrt(discriminant))/(2*a);

		// t1
		double3 hit_pos = ray.origin + ray.direction*t1;
		if (abs(hit_pos[1])<=half_height){ // Hit is on cylinder
			if (t1 > t_min && t1 < t_max && t1<t){
				t = t1;
				intersects = true;
				hit->position = ray.origin + ray.direction*t;
				hit->depth = t;
				normal = {hit_pos[0],0,hit_pos[2]};
				hit->normal = normalize(normal);
			}
		}

		// t2
		hit_pos = ray.origin + ray.direction*t2;
		if (abs(hit_pos[1])<=half_height){ // Hit is on cylinder
			if (t2 > t_min && t2 < t_max && t2<t){
				t = t2;
				intersects = true;
				hit->position = ray.origin + ray.direction*t;
				hit->depth = t;
				normal = {hit_pos[0],0,hit_pos[2]};
				hit->normal = normalize(normal);
			}
		}
	}
    return intersects;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le cylindre.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire (comme ici).
AABB Cylinder::compute_aabb() {
	return Object::compute_aabb();
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un mesh.
//
// Référez-vous au PDF pour la paramétrisation pour les coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
//
bool Mesh::local_intersect(Ray ray,  
						   double t_min, double t_max, 
						   Intersection* hit)
{
	bool hit_bool = false;
	double min_dist = DBL_MAX;

	// Loop through triangles and set the intersection depth to the nearest one
	// See container.cpp
	for (auto& triangle: triangles){
		Intersection tmp;
		if (intersect_triangle(ray, t_min, min_dist, triangle, &tmp)){
			if (tmp.depth<min_dist){
				min_dist = tmp.depth;
				hit_bool = true;
				*hit = tmp;
			}
		}
	}

	hit->depth = min_dist;
	return hit_bool;

}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un triangle.
// S'il y a intersection, remplissez hit avec l'information sur la normale et les coordonnées texture.
bool Mesh::intersect_triangle(Ray  ray, 
							  double t_min, double t_max,
							  Triangle const tri,
							  Intersection *hit)
{
	// Extrait chaque position de sommet des données du maillage.
	double3 const &p0 = positions[tri[0].pi]; // ou Sommet A (Pour faciliter les explications)
	double3 const &p1 = positions[tri[1].pi]; // ou Sommet B
	double3 const &p2 = positions[tri[2].pi]; // ou Sommet C

	// Triangle en question. Respectez la convention suivante pour vos variables.
	//
	//     A
	//    / \
	//   /   \
	//  B --> C
	//
	// Respectez la règle de la main droite pour la normale.

	// @@@@@@ VOTRE CODE ICI
	// Décidez si le rayon intersecte le triangle (p0,p1,p2).
	// Si c'est le cas, remplissez la structure hit avec les informations
	// de l'intersection et renvoyez true.
	// Pour plus de d'informations sur la géométrie, référez-vous à la classe dans object.hpp.
	//
	// NOTE : hit.depth est la profondeur de l'intersection actuellement la plus proche,
	// donc n'acceptez pas les intersections qui occurent plus loin que cette valeur.

	// Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html

	// Compute normal
	double3 p0p1 = p1 - p0;
	double3 p0p2 = p2 - p0;
	double3 normal = cross(p0p1, p0p2);
	double area2 = length(normal);

	// Check if ray and plane are parallel
	double n_dot_raydir = dot(normal, ray.direction);
	if (fabs(n_dot_raydir) < EPSILON){
		return false; // Parallel
	}

	// Calculate d and t
	double d = -dot(normal, p0);
	double t = -(dot(normal, ray.origin) + d) / n_dot_raydir;

	// Check if triangle is behind ray
	if (t<0) return false;

	// Calculate intersection point
	double3 P = ray.origin + t*ray.direction;

	// Inside-Out tests
	double3 C;

	// Edge 0
	double3 edge0 = p1 - p0;
	double3 pp0 = P - p0;
	C = cross(edge0, pp0);
	if (dot(normal,C)<0) return false; // P is on the right side

	// Edge 1
	double3 edge1 = p2 - p1;
	double3 pp1 = P - p1;
	C = cross(edge1, pp1);
	if (dot(normal,C)<0) return false; // P is on the right side

	// Edge 2
	double3 edge2 = p0 - p2;
	double3 pp2 = P - p2;
	C = cross(edge2, pp2);
	if (dot(normal,C)<0) return false;

	// Ray intersects with triangle
	if (t > t_min && t < t_max){
		hit->position = ray.origin + ray.direction*t;
		hit->depth = t;
		hit->normal = normal;
		return true;
	}

	return false; // t out of range 
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le Mesh.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Mesh::compute_aabb() {
	return Object::compute_aabb();
}