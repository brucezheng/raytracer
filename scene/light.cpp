#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	ray r2(r); // bc const lol
	isect i;

	if(scene->intersect(r2, i)) {
		return i.getMaterial().kt(i);
	}	
	else
		return glm::dvec3(1.0,1.0,1.0);
}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3& P) const
{
	double dist = glm::length(P-position);
	double atten = 1.0 / (constantTerm + linearTerm * dist + quadraticTerm * (dist * dist));
	return std::min(1.0,atten);
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	ray _r(r); // bc const lol
	isect i_in;
	double dist = glm::length(p-position);

	// hits an object
	if(scene->intersect(_r, i_in) && i_in.getT() < dist) {
		_r.setPosition(_r.at(i_in.getT() + 0.00001));
		isect i_out;
		// looks for the point where the raw exits the current object
		if(scene->intersect(_r, i_out)) {
			//calculate attenuation
			double travel_dist = i_out.getT();
			glm::dvec3 trans = i_in.getMaterial().kt(i_in);
			glm::dvec3 atten(std::pow(trans[0],travel_dist),
					 std::pow(trans[1],travel_dist),
					 std::pow(trans[2],travel_dist));
				
			// now look for more attenuation from other objects between here and the light
			_r.setPosition(_r.at(i_in.getT() + 0.00001));
			atten *= this->shadowAttenuation(_r, _r.getPosition()); //
			return atten;		
		} else {
			// if we never exit the object, assume light is inside the obj
			return glm::dvec3(0.0,0.0,0.0);
		}
	}	
	else
		return glm::dvec3(1.0,1.0,1.0);
}

#define VERBOSE 0

