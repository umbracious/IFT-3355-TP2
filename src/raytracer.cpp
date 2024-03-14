#include "raytracer.h"

void Raytracer::render(const Scene& scene, Frame* output)
{       
    // Crée le z_buffer.
    double *z_buffer = new double[scene.resolution[0] * scene.resolution[1]];
    for(int i = 0; i < scene.resolution[0] * scene.resolution[1]; i++) {
        z_buffer[i] = scene.camera.z_far; //Anciennement DBL_MAX. À remplacer avec la valeur de scene.camera.z_far
    }

	// @@@@@@ VOTRE CODE ICI
	// Calculez les paramètres de la caméra pour les rayons.
	
	// Base vectors for basis change
	double3 forward = normalize(scene.camera.center - scene.camera.position);
	double3 right = normalize(cross(scene.camera.up, forward));
	double3 up = normalize(cross(right, forward)); // guarantees 90 degree angle for up
	double3 pos = scene.camera.position;

	// Viewport paramters
	double FOVy_rads = deg2rad(scene.camera.fovy);
	double vp_height = tan(FOVy_rads/2)*2; // Assumes distance between camera origin and viewport center is normalized
	double vp_width = vp_height * scene.camera.aspect; // Assuming aspect ratio is width:height
	double3 top_left_corner = scene.camera.center + scene.camera.up*(vp_height/2) 
		+ cross(scene.camera.center, scene.camera.up)*(vp_width/2); // Left vector is cross product between up and center vectors


    // Itère sur tous les pixels de l'image.
    for(int y = 0; y < scene.resolution[1]; y++) {
		if (y % 40){
			std::cout << "\rScanlines completed: " << y << "/" << scene.resolution[1] << '\r';
		}

        for(int x = 0; x < scene.resolution[0]; x++) {

			int avg_z_depth = 0;
			double3 avg_ray_color{0,0,0};
			
			for(int iray = 0; iray < scene.samples_per_pixel; iray++) {
				// Génère le rayon approprié pour ce pixel.
				Ray ray;
				// Initialise la profondeur de récursivité du rayon.
				int ray_depth = 0;
				// Initialize la couleur du rayon
				double3 ray_color{0,0,0};

				// @@@@@@ VOTRE CODE ICI
				// Mettez en place le rayon primaire en utilisant les paramètres de la caméra.
				ray.origin = scene.camera.position;

				// Lancez le rayon de manière uniformément aléatoire à l'intérieur du pixel dans la zone délimité par jitter_radius.
				double3 pixel_pos{-scene.resolution[0]/2 + x + 0.5, scene.resolution[1]/2 - y - 0.5, length(scene.camera.center-scene.camera.position)}; // Pixel center position
				pixel_pos += double3{-scene.jitter_radius + rand_double()*scene.jitter_radius*2, -scene.jitter_radius + rand_double()*scene.jitter_radius*2, 0}; // Apply jitter to pixel
				pixel_pos = {pixel_pos[0]/scene.resolution[0]*vp_width, pixel_pos[1]/scene.resolution[1]*vp_height, pixel_pos[2]}; // Stretch to plane

				// Basis change
				double4x4 cam_to_world_matrix{
					{right[0],up[0],forward[0],pos[0]},
					{right[1],up[1],forward[1],pos[1]},
					{right[2],up[2],forward[2],pos[2]},
					{0,0,0,1}};
				double4x4 world_to_cam_matrix = inverse(cam_to_world_matrix);				
				double4 tmp = mul(world_to_cam_matrix, {pixel_pos[0],pixel_pos[1],pixel_pos[2],1});
				pixel_pos = {tmp[0],tmp[1],tmp[2]};

				
				ray.direction = normalize(pixel_pos-ray.origin); // Set ray direction (normalized)

				// Initiliaze ray depth
				double z_depth = scene.camera.z_far;

				// Faites la moyenne des différentes couleurs obtenues suite à la récursion.
				trace(scene, ray, ray_depth, &ray_color, &z_depth); // Recursion
				avg_ray_color += ray_color;
				avg_z_depth += z_depth;
			}

			avg_z_depth = avg_z_depth / scene.samples_per_pixel;
			avg_ray_color = avg_ray_color / scene.samples_per_pixel;

			// Test de profondeur
			if(avg_z_depth >= scene.camera.z_near && avg_z_depth <= scene.camera.z_far && 
				avg_z_depth < z_buffer[x + y*scene.resolution[0]]) {
				z_buffer[x + y*scene.resolution[0]] = avg_z_depth;

				// Met à jour la couleur de l'image (et sa profondeur)
				output->set_color_pixel(x, y, avg_ray_color);
				output->set_depth_pixel(x, y, (avg_z_depth - scene.camera.z_near) / 
										(scene.camera.z_far-scene.camera.z_near));
			}
        }
    }

    delete[] z_buffer;
}

// @@@@@@ VOTRE CODE ICI
// Veuillez remplir les objectifs suivants:
// 		- Détermine si le rayon intersecte la géométrie.
//      	- Calculer la contribution associée à la réflexion.
//			- Calculer la contribution associée à la réfraction.
//			- Mettre à jour la couleur avec le shading + 
//			  Ajouter réflexion selon material.reflection +
//			  Ajouter réfraction selon material.refraction 
//            pour la couleur de sortie.
//          - Mettre à jour la nouvelle profondeure.
void Raytracer::trace(const Scene& scene,
					  Ray ray, int ray_depth,
					  double3* out_color, double* out_z_depth)
{
	Intersection hit;
	// Fait appel à l'un des containers spécifiées.
	if(scene.container->intersect(ray,EPSILON,*out_z_depth,&hit)) {		
		Material& material = ResourceManager::Instance()->materials[hit.key_material];

		// @@@@@@ VOTRE CODE ICI
		// Déterminer la couleur associée à la réflection d'un rayon de manière récursive.
		
		// @@@@@@ VOTRE CODE ICI
		// Déterminer la couleur associée à la réfraction d'un rayon de manière récursive.
		// 
		// Assumez que l'extérieur/l'air a un indice de réfraction de 1.
		//
		// Toutes les géométries sont des surfaces et non pas de volumes.

		*out_color = shade(scene,hit);
		*out_z_depth = hit.depth;
	}
}

// @@@@@@ VOTRE CODE ICI
// Veuillez remplir les objectifs suivants:
// 		* Calculer la contribution des lumières dans la scène.
//			- Itérer sur toutes les lumières.
//				- Inclure la contribution spéculaire selon le modèle de Blinn en incluant la composante métallique.
//	          	- Inclure la contribution diffuse. (Faites attention au produit scalare. >= 0)
//   	  	- Inclure la contribution ambiante
//      * Calculer si le point est dans l'ombre
//			- Itérer sur tous les objets et détecter si le rayon entre l'intersection et la lumière est occludé.
//				- Ne pas considérer les points plus loins que la lumière.
//			- Par la suite, intégrer la pénombre dans votre calcul
//		* Déterminer la couleur du point d'intersection.
//        	- Si texture est présente, prende la couleur à la coordonnées uv
//			- Si aucune texture, prendre la couleur associé au matériel.

double3 Raytracer::shade(const Scene& scene, Intersection hit)
{
	// Material& material = ResourceManager::Instance()->materials[hit.key_material]; lorsque vous serez rendu à la partie texture.
	
	return double3{0,0,0};
}
