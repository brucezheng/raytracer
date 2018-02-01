#include "material.h"
#include "../ui/TraceUI.h"
#include "light.h"
#include "ray.h"
extern TraceUI* traceUI;

#include <glm/gtx/io.hpp>
#include <iostream>
#include "../fileio/images.h"

using namespace std;
extern bool debugMode;

Material::~Material()
{
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene* scene, const ray& r, const isect& i) const
{
	glm::dvec3 result(0.0,0.0,0.0);

	glm::dvec3 n = i.getN(); // normal
	glm::dvec3 P = r.at(i); // intersection point (apparently we dont store this?) 
	glm::dvec3 v = -1.0 * r.getDirection(); // opposite of direction of ray
	
	glm::dvec3 P_nudge = r.at(i.getT() - 0.0000000001); // to avoid shadow probs

	glm::dvec3 ambient = scene->ambient() * ka(i);	

	for ( const auto& pLight : scene->getAllLights() )
	{	
		//setting up unit vectors
		glm::dvec3 l = pLight->getDirection(P);
		glm::dvec3 I_in = pLight->getColor() * pLight->distanceAttenuation(P);
		glm::dvec3 l_proj_n = n * glm::dot(l,n);
		glm::dvec3 ref = l_proj_n + (l_proj_n - l); // r is the refleciton of r to the surface normal
		
		// lighting eq
		glm::dvec3 contribution = kd(i) * std::max(glm::dot(l, n), 0.0);
		contribution += ks(i) * std::pow(std::max(glm::dot(v, ref), 0.0), shininess(i));
		contribution *= I_in;

		// SHADOW ATTENUATION
		ray shadowRay(P_nudge,pLight->getDirection(P_nudge),glm::dvec3(0.0,0.0,0.0),ray::SHADOW);
		contribution *= pLight->shadowAttenuation(shadowRay,P_nudge);
		//std::cout << contribution;

		result += contribution;
	}
	
	return result;
}

TextureMap::TextureMap(string filename)
{
	data = readImage(filename.c_str(), width, height);
	if (data.empty()) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("'.");
		throw TextureMapException(error);
	}
}

glm::dvec3 TextureMap::getMappedValue(const glm::dvec2& coord) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.

	return glm::dvec3(1, 1, 1);
}

glm::dvec3 TextureMap::getPixelAt(int x, int y) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.

	return glm::dvec3(1, 1, 1);
}

glm::dvec3 MaterialParameter::value(const isect& is) const
{
	if (0 != _textureMap)
		return _textureMap->getMappedValue(is.getUVCoordinates());
	else
		return _value;
}

double MaterialParameter::intensityValue(const isect& is) const
{
	if (0 != _textureMap) {
		glm::dvec3 value(
		        _textureMap->getMappedValue(is.getUVCoordinates()));
		return (0.299 * value[0]) + (0.587 * value[1]) +
		       (0.114 * value[2]);
	} else
		return (0.299 * _value[0]) + (0.587 * _value[1]) +
		       (0.114 * _value[2]);
}
