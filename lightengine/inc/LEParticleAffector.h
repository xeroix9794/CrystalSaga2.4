#ifndef LEParticleAffector_H
#define LEParticleAffector_H

#include "LightEngineAPI.h"

class LEParticleSystem;
class LEParticle;

/* 
Implementing Particle Impact Factor Interface Abstract Classes.
@remarks
The particle influence factor is to modify the particles in the particle system during the lifetime of the particle. They are packaged into
Type, such as 'force force' influence factor, 'fader' influence factor;
The type will modify the particles in different ways, using different parameters.
@par
Because you can use many types of impact factors, the engine does not specify which available factors. it
Provides some intrinsic factors, as it also allows plugins or applications to extend the impact factor. This can be
By creating a subclass of ParticleAffector to extend the launch you need, of course, both
Create a subclass of ParticleAffectorFactory to create your new impact factor object. you are
ParticleSystemManager calls addAffectorFactory to register this factory, so that
Code or script file to create this new impact factor by name

Particle launchers are also used in the same way. When creating particle effects, the particle system is so large
Of the scalability, can be combined with the particle impact factor type and their parameters.
*/
class LIGHTENGINE_API LEParticleAffector {
public:
	LEParticleAffector(LEParticleSystem* mParticleSystem) : m_ParticleSystem(mParticleSystem) {}

	virtual ~LEParticleAffector();

	/*
	Let the impact factor initialize all newly created particles
	@remarks
	Here is the impact factor that has the opportunity to initialize the particles of his effects. The factor wants to initialize some
	Or so the particles, which depend on the influence factor itself
	@param
	The pParticle needs to initialize the pointer to the particle.
	*/
	virtual void _initParticle(LEParticle* pParticle) { /* by default do nothing */ }

	/*
	Allows the impact factor to act on all active particles
	@remarks
	Here the influencing factors have the opportunity to apply their data to the particles in the system.
	Want to use their effects to some or all of the particles, which rely on particle factors
	@param
	The pSystem acts as a pointer to the particle system.
	timeElapsed The time (in seconds) from the last call to the present.
	*/
	virtual void _affectParticles(LEParticleSystem* pSystem, float timeElapsed) = 0;

	/* 
	Returns the type of impact factor.
	@remarks
	This parameter determines the type of impact factor, so that you can use this parameter to create a shadow
	Ring factor
	*/
	const String &getType(void) const { return m_Type; }

protected:
	/*
	The internal method is used to set the basic parameter definition for the subclass
	@remarks
	Because StringInterface contains a parameter dictionary for each class, the subclass needs to call it to request it
	The base class also adds its parameters to the dictionary. Do not do this in the constructor, because the constructor
	The function does not run as a virtual function, it does not have the context of the virtual function
	@par
	Subclasses must call their own createParamDictionary before calling this method
	Method
	*/
	void addBaseParameters(void) {

		// Actually did not do anything - might do something in the future 
	}

	// The type of particle impact factor must be initialized by subclasses
	string m_Type;

	LEParticleSystem* m_ParticleSystem;
};
#endif