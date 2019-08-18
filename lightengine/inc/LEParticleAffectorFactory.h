#ifndef LEParticleAffectorFactory_H
#define LEParticleAffectorFactory_H

#include "LEEffPrerequisites.h"

class LIGHTENGINE_API LEParticleAffectorFactory {
protected:
	std::vector<LEParticleAffector*> m_Affectors;

public:
	LEParticleAffectorFactory() {};
	virtual ~LEParticleAffectorFactory();

	// Returns the name of the plant, which identifies the type of particle impact factor created by this factory
	virtual string getName() const = 0;

	// Create a new impact factor object

	virtual LEParticleAffector* createAffector(LEParticleSystem* psys) = 0;

	// Destructor Influence Factor Object
	virtual void destroyAffector(LEParticleAffector* e);
};
#endif