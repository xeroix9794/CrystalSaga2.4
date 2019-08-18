#ifndef LEEffectMath_H
#define LEEffectMath_H

#include "LightEngineAPI.h"

#include <string>
#include <math.h>

// Front statement
class LEDegree;

/* 
Give the angle in radians as a unit of wrapping
@remarks
The radians and angle values can be converted to each other, and the conversion is done automatically
*/
class LERadian {
	float m_fRad;

public:
	explicit LERadian(float r = 0) : m_fRad(r) {}
	LERadian(const LEDegree& d);
	const LERadian& operator = (const float& f) { m_fRad = f; return *this; }
	const LERadian& operator = (const LERadian& r) { m_fRad = r.m_fRad; return *this; }
	const LERadian& operator = (const LEDegree& d);

	float valueDegrees() const;
	float valueRadians() const { return m_fRad; }
	float valueAngleUnits() const;

	LERadian operator + (const LERadian& r) const { return LERadian(m_fRad + r.m_fRad); }
	LERadian operator + (const LEDegree& d) const;
	LERadian& operator += (const LERadian& r) { m_fRad += r.m_fRad; return *this; }
	LERadian& operator += (const LEDegree& d);
	LERadian operator - () { return LERadian(-m_fRad); }
	LERadian operator - (const LERadian& r) const { return LERadian(m_fRad - r.m_fRad); }
	LERadian operator - (const LEDegree& d) const;
	LERadian& operator -= (const LERadian& r) { m_fRad -= r.m_fRad; return *this; }
	LERadian& operator -= (const LEDegree& d);
	LERadian operator * (float f) const { return LERadian(m_fRad * f); }
	LERadian operator * (const LERadian& f) const { return LERadian(m_fRad * f.m_fRad); }
	LERadian& operator *= (float f) { m_fRad *= f; return *this; }
	LERadian operator / (float f) const { return LERadian(m_fRad / f); }
	LERadian& operator /= (float f) { m_fRad /= f; return *this; }

	bool operator <  (const LERadian& r) const { return m_fRad < r.m_fRad; }
	bool operator <= (const LERadian& r) const { return m_fRad <= r.m_fRad; }
	bool operator == (const LERadian& r) const { return m_fRad == r.m_fRad; }
	bool operator != (const LERadian& r) const { return m_fRad != r.m_fRad; }
	bool operator >= (const LERadian& r) const { return m_fRad >= r.m_fRad; }
	bool operator >  (const LERadian& r) const { return m_fRad > r.m_fRad; }
};

/*
Give the angle to the angle of the package class.
@remarks
The radians and angle values can be converted to each other, and the conversion is done automatically
*/
class LEDegree {
	float m_fDeg;	// if you get an error here - make sure to define/typedef 'Real' first

public:
	explicit LEDegree(float d = 0) : m_fDeg(d) {}
	LEDegree(const LERadian& r) : m_fDeg(r.valueDegrees()) {}
	const LEDegree& operator = (const float& f) { m_fDeg = f; return *this; }
	const LEDegree& operator = (const LEDegree& d) { m_fDeg = d.m_fDeg; return *this; }
	const LEDegree& operator = (const LERadian& r) { m_fDeg = r.valueDegrees(); return *this; }

	float valueDegrees() const { return m_fDeg; }
	float valueRadians() const;
	float valueAngleUnits() const;

	LEDegree operator + (const LEDegree& d) const { return LEDegree(m_fDeg + d.m_fDeg); }
	LEDegree operator + (const LERadian& r) const { return LEDegree(m_fDeg + r.valueDegrees()); }
	LEDegree& operator += (const LEDegree& d) { m_fDeg += d.m_fDeg; return *this; }
	LEDegree& operator += (const LERadian& r) { m_fDeg += r.valueDegrees(); return *this; }
	LEDegree operator - () { return LEDegree(-m_fDeg); }
	LEDegree operator - (const LEDegree& d) const { return LEDegree(m_fDeg - d.m_fDeg); }
	LEDegree operator - (const LERadian& r) const { return LEDegree(m_fDeg - r.valueDegrees()); }
	LEDegree& operator -= (const LEDegree& d) { m_fDeg -= d.m_fDeg; return *this; }
	LEDegree& operator -= (const LERadian& r) { m_fDeg -= r.valueDegrees(); return *this; }
	LEDegree operator * (float f) const { return LEDegree(m_fDeg * f); }
	LEDegree operator * (const LEDegree& f) const { return LEDegree(m_fDeg * f.m_fDeg); }
	LEDegree& operator *= (float f) { m_fDeg *= f; return *this; }
	LEDegree operator / (float f) const { return LEDegree(m_fDeg / f); }
	LEDegree& operator /= (float f) { m_fDeg /= f; return *this; }

	bool operator <  (const LEDegree& d) const { return m_fDeg < d.m_fDeg; }
	bool operator <= (const LEDegree& d) const { return m_fDeg <= d.m_fDeg; }
	bool operator == (const LEDegree& d) const { return m_fDeg == d.m_fDeg; }
	bool operator != (const LEDegree& d) const { return m_fDeg != d.m_fDeg; }
	bool operator >= (const LEDegree& d) const { return m_fDeg >= d.m_fDeg; }
	bool operator >  (const LEDegree& d) const { return m_fDeg > d.m_fDeg; }
};

// These functions can not be defined in the LERadian class, because these functions require the definition of the LEDegree class
inline LERadian::LERadian(const LEDegree& d) : m_fRad(d.valueRadians()) {
}

inline const LERadian& LERadian::operator =(const LEDegree& d) {
	m_fRad = d.valueRadians(); 
	return *this;
}

inline LERadian LERadian::operator +(const LEDegree& d) const {
	return LERadian(m_fRad + d.valueRadians());
}

inline LERadian& LERadian::operator +=(const LEDegree& d) {
	m_fRad += d.valueRadians(); 
	return *this;
}

inline LERadian LERadian::operator -(const LEDegree& d) const {
	return LERadian(m_fRad - d.valueRadians());
}

inline LERadian& LERadian::operator -=(const LEDegree& d) {
	m_fRad -= d.valueRadians(); 
	return *this;
}


/*
This class provides access to public math functions.
@remarks
Most math functions are aliases of the C run-time library functions. Here the alias will make the future more
Optimized functions become possible, or according to faster runtime libraries or other custom math libraries
@note
<br> Here is the extract from MgcMath.h
<a href="http://www.magic-software.com"> Wild Magic </a>.
*/
class LIGHTENGINE_API LEEffectMath {
public:

	/*
	Angle unit. This function allows the angle units to be able to use both the above angles and radians.
	This function will be used automatically when parsing the file. These uses can view the Angle class
	*/
	
	enum AngleUnit {
		AU_DEGREE,
		AU_RADIAN
	};

protected:

	// API internal use of the angle unit
	static AngleUnit m_sAngleUnit;

	// The trigonometric function table is determined by the constructor
	static int m_TrigTableSize;

	// Radian value -> minimum representation radian in trigonometric function table (m_TrigTableSize / 2 * PI)
	static float m_TrigTableFactor;
	static float* m_SinTable;
	static float* m_TanTable;

	// Construct a trigonometric function table (private)
	void buildTrigTables();

	static float SinTable(float fValue);
	static float TanTable(float fValue);

public:

	/*
	The default constructor.
	@param
	trigTableSize Sets the size of the internal trigonometric function table (optional)
	*/
	LEEffectMath(unsigned int trigTableSize = 4096);

	// Default destructor
	~LEEffectMath();

	static const float POS_INFINITY;
	static const float NEG_INFINITY;
	static const float PI;
	static const float TWO_PI;
	static const float HALF_PI;
	static const float fDeg2Rad;
	static const float fRad2Deg;

	// Angle radian correlation
	static inline float DegreesToRadians(float degrees) { return degrees * fDeg2Rad; }
	static inline float RadiansToDegrees(float radians) { return radians * fRad2Deg; }


	static void setAngleUnit(AngleUnit unit);
	static AngleUnit getAngleUnit(void);
	static float AngleUnitsToRadians(float units);
	static float RadiansToAngleUnits(float radians);
	static float AngleUnitsToDegrees(float units);
	static float DegreesToAngleUnits(float degrees);

	// Random number correlation
	static float UnitRandom();
	static float RangeRandom(float fLow, float fHigh);

	// Trigonometric function correlation
	/*
	Sine function
	@param
		fValue: The radian value of the corner
		useTables: If it is true, use the lookup table rather than through the calculation, this will be faster but the accuracy is reduced.
	*/
	static inline float Sin(const LERadian& fValue, bool useTables = false) {
		return (!useTables) ? float(sin(fValue.valueRadians())) : SinTable(fValue.valueRadians());
	}

	/* 
	Sine function
	@param
		fValue: The arc value of the angle
		useTables: If it is true, use a lookup table instead of a calculation, which will be faster but less accurate.
	*/
	static inline float Sin(float fValue, bool useTables = false) {
		return (!useTables) ? float(sin(fValue)) : SinTable(fValue);
	}
	
	/*
	Cosine function.
	@param
	fValue: The arc value of the angle
	useTables: If it is true, use a lookup table instead of a calculation, which will be faster but less accurate.
	*/
	static inline float Cos(const LERadian& fValue, bool useTables = false) {
		return (!useTables) ? float(cos(fValue.valueRadians())) : SinTable(fValue.valueRadians() + HALF_PI);
	}
	
	/* 
	Cosine function.
	@param
	fValue: The arc value of the angle
	useTables: If it is true, use a lookup table instead of a calculation, which will be faster but less accurate.
	*/
	static inline float Cos(float fValue, bool useTables = false) {
		return (!useTables) ? float(cos(fValue)) : SinTable(fValue + HALF_PI);
	}
	/* 
	Tangent function.
	@param
		fValue: The radian value of the corner
		useTables: If it is true, use the lookup table rather than through the calculation, this will be faster but the accuracy is reduced.
	*/
	static inline float Tan(const LERadian& fValue, bool useTables = false) {
		return (!useTables) ? float(tan(fValue.valueRadians())) : TanTable(fValue.valueRadians());
	}

	/* Tangent function.
	@param
		fValue: The radian value of the corner
		useTables: If it is true, use the lookup table rather than through the calculation, this will be faster but the accuracy is reduced.
	*/
	static inline float Tan(float fValue, bool useTables = false) {
		return (!useTables) ? float(tan(fValue)) : TanTable(fValue);
	}
};
#endif
