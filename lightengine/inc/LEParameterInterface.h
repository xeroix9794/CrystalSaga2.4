#ifndef LEParameterInterface_H
#define LEParameterInterface_H

#include "LEEffPrerequisites.h"

enum LEParameterType {
	PT_BOOL,
	PT_REAL,
	PT_INT,
	PT_UNSIGNED_INT,
	PT_SHORT,
	PT_UNSIGNED_SHORT,
	PT_LONG,
	PT_UNSIGNED_LONG,
	PT_STRING,
	PT_VECTOR3,
	PT_MATRIX3,
	PT_MATRIX4,
	PT_QUATERNION,
	PT_COLOURVALUE
};

// parameter definition, used to support LEParameterInteface reflection mechanism
class LIGHTENGINE_API LEParameterDef {
public:
	String name;
	String description;
	LEParameterType paramType;
	LEParameterDef(const String& newName, const String& newDescription, LEParameterType newType)
		: name(newName), description(newDescription), paramType(newType) {}
};
typedef std::vector<LEParameterDef> LEParameterList;

// The parameter object gets / sets the abstract class of the parameter
class LIGHTENGINE_API LEParamCommand {
public:
	virtual String doGet(const void* target) const = 0;
	virtual void doSet(void* target, const String& val) = 0;

	virtual ~LEParamCommand() {
	}
};
typedef std::map<String, LEParamCommand* > ParamCommandMap;

class LIGHTENGINE_API LEParamDictionary {
	friend class LEParameterInterface;

protected:

	// Parameter definition
	LEParameterList m_ParamDefs;

	// A parameter object for get / set
	ParamCommandMap m_ParamCommands;

	// Returns the parameter object of the named parameter
	LEParamCommand* getParamCommand(const String& name) {
		ParamCommandMap::iterator i = m_ParamCommands.find(name);
		if (i != m_ParamCommands.end()) {
			return i->second;
		}
		else {
			return 0;
		}
	}

	const LEParamCommand* getParamCommand(const String& name) const {
		ParamCommandMap::const_iterator i = m_ParamCommands.find(name);
		if (i != m_ParamCommands.end()) {
			return i->second;
		}
		else {
			return 0;
		}
	}
public:
	LEParamDictionary() {}

	/*
	Increase the parameter definition for this class.
	@param The LEParameterDef object of the paramDef parameter
	@param paramCmd parameter The pointer to the derived class object is used to handle getting / setting.
	NB: This class does not destruct this pointer
	*/
	void addParameter(const LEParameterDef& paramDef, LEParamCommand* paramCmd) {
		m_ParamDefs.push_back(paramDef);
		m_ParamCommands[paramDef.name] = paramCmd;
	}

	const LEParameterList& getParameters(void) const {
		return m_ParamDefs;
	}
};

typedef std::map<String, LEParamDictionary> LEParamDictionaryMap;

/*
Defines a common interface that uses a reflection style to set custom parameters.
@remarks
This class holds the class static names used for subclasses - the parameter map dictionary. For more details please see the LEParamDictionary.
To use this class, each subclass must call createParamDictionary in the constructor
Build that class parameter dictionary.
*/
class LIGHTENGINE_API LEParameterInterface {
public:
	virtual ~StringInterface() {
	}

	LEParamDictionary* getParamDictionary(void) {
		LEParamDictionaryMap::iterator i = m_sDictionary.find(m_ParamDictName);
		if (i != m_sDictionary.end()) {
			return &(i->second);
		}
		else {
			return 0;
		}
	}

	const LEParamDictionary* getParamDictionary(void) const {
		LEParamDictionaryMap::const_iterator i = m_sDictionary.find(m_ParamDictName);
		if (i != m_sDictionary.end()) {
			return &(i->second);
		}
		else {
			return 0;
		}
	}

	const LEParameterList& getParameters(void) const;

	virtual bool setParameter(const String& name, const String& value);

	virtual void setParameterList(const NameValuePairList& paramList);

	virtual String getParameter(const String& name) const {
		
		// Get the dictionary
		const LEParamDictionary* dict = getParamDictionary();
		if (dict) {

			// Find the parameter object
			const LEParamCommand* cmd = dict->getParamCommand(name);
			if (cmd) {
				return cmd->doGet(this);
			}
		}

		// Failure
		return "";
	}

	virtual void copyParametersTo(LEParameterInterface* dest) const {

		// Get the dictionary
		const LEParamDictionary* dict = getParamDictionary();
		if (dict) {

			// Traverse their own parameters
			ParameterList::const_iterator i;
			for (i = dict->mParamDefs.begin(); i != dict->mParamDefs.end(); ++i) {
				dest->setParameter(i->name, getParameter(i->name));
			}
		}
	}

	static void cleanupDictionary();

protected:
	
	/*
	The internal method is used to create a class parameter dictionary
.
	@remarks
	This method will check the parameter dictionary for this class, and if it does not exist, create a .NB:
	For the name of the class (RTTI is not u
	@param
	className class name (dictionary used)
	@returns
	true if the dictionary is created, false if the dictionary already exists
	*/
	bool createParamDictionary(const String& className) {
		m_ParamDictName = className;
		if (m_sDictionary.find(className) == m_sDictionary.end()) {
			m_sDictionary[className] = ParamDictionary();
			return true;
		}
		return false;

	}

	// Parameter dictionary
	static LEParamDictionaryMap m_sDictionary;
	
	// The class name used to find (must be initialized by subclass)
	String m_ParamDictName;
};
#endif