/* $Rev: 250 $ */
#include "Scene.h"

#include "Colour.h"
#include "Display.h"
#include "utility.h"

Scene::Scene() : backgroundColour(0,0,0), ambientLight(0,0,0), maxRayDepth(3), renderWidth(800), renderHeight(600), filename("render.png"), camera_(), objects_(), lights_() {

}

Scene::~Scene() {

}

void Scene::render() const {
	Display display("Render", renderWidth, renderHeight, Colour(128,128,128));
	
	std::cout << "Rendering a scene with " << objects_.size() << " objects" << std::endl;

	double halfPixel = 2.0/(2*renderWidth);

	for (unsigned int y = 0; y < renderHeight; ++y) {
		for (unsigned int x = 0; x < renderWidth; ++x) {
			double cx = (x - 0.5*renderWidth)*2.0/renderWidth + halfPixel;
			double cy = (y - 0.5*renderHeight)*2.0/renderWidth + halfPixel;
			Ray ray = camera_->castRay(cx,cy);
			display.set(x, y, computeColour(ray, maxRayDepth));
		}
		display.refresh();
	}
	display.save(filename);
	display.pause(5);
}

RayIntersection Scene::intersect(const Ray& ray) const {
	RayIntersection firstHit;
	firstHit.distance = infinity;	
	for (auto& obj : objects_) {
		std::vector<RayIntersection> hits = obj->intersect(ray);
		for (auto& hit : hits) {
			if (hit.distance > epsilon && hit.distance < firstHit.distance) {
				firstHit = hit;
			}
		}
	}
	for (auto& obj: objects_) {
	  auto hits = obj->intersect(ray);
	}
	return firstHit;
}

Colour Scene::computeColour(const Ray& viewRay, unsigned int rayDepth) const {
	RayIntersection hitPoint = intersect(viewRay);
	if (hitPoint.distance == infinity) {
		return backgroundColour;
	}

	Colour hitColour = ambientLight * hitPoint.material.ambientColour;
		
	Material& mat = hitPoint.material;
	for (auto& light : lights_) {
            // Check if we can see this light
           
            
            //add in shadows for particular light source
            //object in shadow if there is object between that point and the viewpoint
            // p=ph+t(pl-ph) 

            //shadows
            Ray shadowRay;

            Vector v = light->location - hitPoint.point;
            Vector l = v/v.norm(); //normalise
            
            shadowRay.point = hitPoint.point;
            shadowRay.direction = Direction(l); 
            RayIntersection shadowIntersect = intersect(shadowRay);

            //diffuse
            Vector lightVector = light->location - hitPoint.point;
            double norm = lightVector.norm();
      
            double dotProductDiff = hitPoint.normal.dot(lightVector/norm)/hitPoint.normal.norm();

            //specular
            Vector e = -viewRay.direction/viewRay.direction.norm(); // to make it a unit vector
            
            Vector normal = hitPoint.normal/hitPoint.normal.norm();
            Vector r = 2*normal*(e.dot(normal))-e;
            
            double dotProductSpec = pow(e.dot(r), mat.specularExponent); // n value from phongs intensity output
            
            //mirror
            
            //if distance to shadow intersection == INFINITY
            //I.E there is no intersection, then add in other lighting
            //If distance to shadow intersection != INFINITY
            //Then there is an intersection between the object and light source
            //So we just leave it with the ambient light (shadow)
            if(shadowIntersect.distance == infinity){
                hitColour += light->colour * light->getIntensityAt(hitPoint.point) * mat.diffuseColour * dotProductDiff;
                hitColour += light->colour * light->getIntensityAt(hitPoint.point) * mat.specularColour * dotProductSpec;
                
            }
	}

	hitColour.clip();

	return hitColour;
}

bool Scene::hasCamera() const {
	return bool(camera_);
}
