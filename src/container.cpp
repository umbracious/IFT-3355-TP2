#include "container.h"

// @@@@@@ VOTRE CODE ICI
// - Parcourir l'arbre DEPTH FIRST SEARCH selon les conditions suivantes:
// 		- S'il s'agit d'une feuille, faites l'intersection avec la géométrie.
//		- Sinon, il s'agit d'un noeud altérieur.
//			- Faites l'intersection du rayon avec le AABB gauche et droite. 
//				- S'il y a intersection, ajouter le noeud à ceux à visiter. 
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool BVH::intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
	return true;
}

// @@@@@@ VOTRE CODE ICI
// - Parcourir tous les objets
// 		- Détecter l'intersection avec l'AABB
//			- Si intersection, détecter l'intersection avec la géométrie.
//				- Si intersection, mettre à jour les paramètres.
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool Naive::intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
	bool ray_hit = false;
	double closest = t_max;

	for (auto& object : objects){ // Loop through objects
		Intersection tmp; // Temp intersection
		if (object->intersect(ray, t_min, closest, &tmp)){ // Recursion
			ray_hit = true;
			closest = std::min(closest, tmp.depth); // Select new closest depth
			*hit = tmp;
		}
	}

	return ray_hit;
}
