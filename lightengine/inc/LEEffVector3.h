#ifndef LEEffVector3_H
#define LEEffVector3_H

#include "LightEngineAPI.h"

#include "drDirectX.h"
#include "LEEffectMath.h"
#include "LEEffQuaternion.h"

class LEEffVector3 {
public:
	D3DXVECTOR3 m_vSelf;

	LEEffVector3() : m_vSelf(0, 0, 0) {}
	LEEffVector3(float x, float y, float z) : m_vSelf(x, y, z) {}
	LEEffVector3(const LEEffVector3& v) : m_vSelf(v.m_vSelf) {}
	LEEffVector3(const D3DXVECTOR3& dxV) : m_vSelf(dxV) {}

	D3DXVECTOR3& GetDXValue() { return m_vSelf; }

	// Mathematical operation
	inline LEEffVector3 operator + (const LEEffVector3& rkVector) const {
		LEEffVector3 kSum;
		D3DXVec3Add(&(kSum.m_vSelf), &m_vSelf, &(rkVector.m_vSelf));
		return kSum;
	}

	inline LEEffVector3& operator *= (float fScalar) {
		D3DXVec3Scale(&m_vSelf, &m_vSelf, fScalar);
		return *this;
	}

	inline LEEffVector3& operator *= (const LEEffVector3& rkVector) {
		m_vSelf.x *= rkVector.m_vSelf.x;
		m_vSelf.y *= rkVector.m_vSelf.y;
		m_vSelf.z *= rkVector.m_vSelf.z;
		return *this;
	}

	inline LEEffVector3 crossProduct(const LEEffVector3& rkVector) const {
		D3DXVECTOR3 kCross;
		D3DXVec3Cross(&kCross, &m_vSelf, &(rkVector.m_vSelf));
		return kCross;
	}

	inline float squaredLength() const {
		return D3DXVec3LengthSq(&m_vSelf);
	}

	/*
	Unit vector.
	@note
	This function does not crash due to a zero vector, but if it is a zero vector it does not change his element
	@returns The previous length of the vector
	*/
	inline float normalise() {
		float fLength = D3DXVec3Length(&m_vSelf);

		// For zero vectors can also be used, but do not change any value
		if (fLength > 1e-06f) {
			D3DXVec3Normalize(&m_vSelf, &m_vSelf);
		}
		return fLength;
	}


	/* 
	Generate a vertical vector.
	@remarks
	This method will return a vector which is perpendicular to this
	vector. There are an infinite number of possibilities but this
	method will guarantee to generate one of them. If you need more
	control you should use the Quaternion class.
	*/
	inline LEEffVector3 perpendicular(void) const {
		static const float fSquareZero = float(1e-06 * 1e-06);
		LEEffVector3 perp = this->crossProduct(LEEffVector3::UNIT_X);

		// Check length length
		if (perp.squaredLength() < fSquareZero) {

			// this axis is the Y axis, so change the axis.
			perp = this->crossProduct(LEEffVector3::UNIT_Y);
		}

		return perp;
	}

	inline bool operator == (const LEEffVector3& rkVector) const {
		return (m_vSelf.x == rkVector.m_vSelf.x && m_vSelf.y == rkVector.m_vSelf.y && m_vSelf.z == rkVector.m_vSelf.z);
	}

	/* 
	Generates a random vector at a given maximum angle.
	@remarks
	This method assumes that the seeds of the random number have been generated
	@param
	angle The maximum offset angle given
	any vector of any vertical vector
	@returns
	A vector that deviates the random angle of the vector. This random vector is not unitized
	*/
	inline LEEffVector3 randomDeviant(const LERadian& angle, const LEEffVector3& up = LEEffVector3::ZERO) const {
		LEEffVector3 newUp;
		if (up == LEEffVector3::ZERO) {

			// Create an upward vector
			newUp = this->perpendicular();
		}
		else {
			newUp = up;
		}

		// Rotate the up vector around the direction vector to the random angle
		LEEffQuaternion q;
		q.FromAngleAxis(LERadian(LEEffectMath::UnitRandom() * LEEffectMath::TWO_PI), *this);
		newUp = q * newUp;

		// Finally rotate this by given angle around randomised up
		q.FromAngleAxis(angle, newUp);
		return q * (*this);
	}

	// Special value
	static const LEEffVector3 ZERO;
	static const LEEffVector3 UNIT_X;
	static const LEEffVector3 UNIT_Y;
	static const LEEffVector3 UNIT_Z;
	static const LEEffVector3 NEGATIVE_UNIT_X;
	static const LEEffVector3 NEGATIVE_UNIT_Y;
	static const LEEffVector3 NEGATIVE_UNIT_Z;
	static const LEEffVector3 UNIT_SCALE;
};
#endif