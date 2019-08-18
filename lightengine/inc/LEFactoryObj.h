#ifndef LEFactoryObj_H
#define LEFactoryObj_H

// Abstraction of the factory class itself does not have any functionality, but the derived class can add functionality.
template< typename T > class FactoryObj {
public:
	virtual ~FactoryObj() {
	};
	
	virtual const string& getType() const = 0;
	virtual T* createInstance(const string& name) = 0;
	virtual void destroyInstance(T*) = 0;
};

#endif