#include "Stdafx.h"
#include "LECamera.h"

LECamera::LECamera() {
	m_fRoll = 0.0f;
	InitPosition(0.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f);
	_snprintf_s(strCameraInfo, 64, _TRUNCATE, "camera : (%4.02f %4.02f %4.02f)__(%4.02f %4.02f %4.02f)", m_EyePos.x, m_EyePos.y, m_EyePos.z, m_RefPos.x, m_RefPos.y, m_RefPos.z);
}

LECamera::~LECamera() {}

VOID LECamera::Move(DWORD dwMoveType) {
	float fStep = 0.2f;
	D3DXVECTOR3 Move(0, 0, 0);

	switch (dwMoveType) {
	case MOVE_LEFT: {
		D3DXVec3Cross(&Move, &(m_RefPos - m_EyePos), &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		D3DXVec3Normalize(&Move, &Move);
		Move *= fStep;
		m_RefPos += Move;
		m_EyePos += Move;
		break;
	}
	case MOVE_RIGHT: {
		D3DXVec3Cross(&Move, &(m_RefPos - m_EyePos), &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		D3DXVec3Normalize(&Move, &Move);
		Move *= fStep;
		m_RefPos -= Move;
		m_EyePos -= Move;
		break;
	}
	case MOVE_FORWARD: {
		D3DXVec3Normalize(&Move, &(m_RefPos - m_EyePos));
		Move *= fStep;
		m_RefPos += Move;
		m_EyePos += Move;
		break;
	}
	case MOVE_BACKWARD: {
		D3DXVec3Normalize(&Move, &(m_RefPos - m_EyePos));
		Move *= fStep;
		m_RefPos -= Move;
		m_EyePos -= Move;
		break;
	}
	case MOVE_UP: {
		Move = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		Move *= fStep;
		m_RefPos += Move;
		m_EyePos += Move;
		break;
	}
	case MOVE_DOWN: {
		Move = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		Move *= fStep;
		m_RefPos -= Move;
		m_EyePos -= Move;
		break;
	}
	case ROTATE_VEER: {
		break;
	}
	case ROTATE_REVERSE: {
		break;
	}
	case ROLL1: {
		m_fRoll += 5.0f;
		break;
	}
	case ROLL2: {
		m_fRoll -= 5.0f;
		break;
	}
	}
	_snprintf_s(strCameraInfo, 64, _TRUNCATE, "camera : (%4.02f %4.02f %4.02f)__(%4.02f %4.02f %4.02f)", m_EyePos.x, m_EyePos.y, m_EyePos.z, m_RefPos.x, m_RefPos.y, m_RefPos.z);
}

void LECamera::MoveForward(float fStep, BOOL bHang) {
	D3DXVECTOR3 Move(0, 0, 0);
	if (bHang) {
		D3DXVec3Normalize(&Move, &(D3DXVECTOR3(m_RefPos.x, m_RefPos.y, 0.0f) - D3DXVECTOR3(m_EyePos.x, m_EyePos.y, 0.0f)));
	}
	else {
		D3DXVec3Normalize(&Move, &(m_RefPos - m_EyePos));
	}
	Move *= fStep;
	m_RefPos += Move;
	m_EyePos += Move;
}

void LECamera::MoveRight(float fStep, BOOL bHang) {
	D3DXVECTOR3 Move(0, 0, 0);
	D3DXVec3Cross(&Move, &(m_RefPos - m_EyePos), &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	D3DXVec3Normalize(&Move, &Move);
	Move *= fStep;
	if (bHang) {
		Move.z = 0;
	}
	m_RefPos += Move;
	m_EyePos += Move;
}

void LECamera::Turn(float fStep, D3DXVECTOR3 *pFocusVec) {
	D3DXVECTOR3 Move(0, 0, 0);
	if (!pFocusVec) {
		D3DXVec3Cross(&Move, &(m_RefPos - m_EyePos), &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		D3DXVec3Normalize(&Move, &Move);
		Move *= fStep;
		m_RefPos += Move;
	}
	else {
		m_RefPos = *pFocusVec;
		D3DXVECTOR3 vv = m_EyePos - *pFocusVec;
		D3DXVec3Normalize(&vv, &vv);
		D3DXVec3Cross(&Move, &D3DXVECTOR3(vv.x, vv.y, 0.0f), &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		D3DXVec3Normalize(&Move, &Move);
		Move *= fStep;
		m_EyePos += Move;
	}
}

void LECamera::FrameMove(DWORD dwTailTime) {
}
