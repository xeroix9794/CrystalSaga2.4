#include "StdAfx.h"

#include "GlobalInc.h"
#include "LEModelEff.h"

#include "i_effect.h"
#include "LERender.h"
#include "drPredefinition.h"
#include "CPerformance.h"

I_Effect::I_Effect(void) {
	m_pDev = NULL;

	ReleaseAll();

	_bBillBoard = false;
	_bSizeSame = true;
	_bAngleSame = true;
	_bPosSame = true;
	_bColorSame = true;

	_eSrcBlend = D3DBLEND_SRCALPHA;
	_eDestBlend = D3DBLEND_INVSRCALPHA;

	_iUseParam = 0;
	m_ilast = m_inext = 0;
	m_flerp = 0;

	_bRotaLoop = false;
	_vRotaLoop = D3DXVECTOR4(0, 0, 0, 0);

	_bAlpha = true;
	_bRotaBoard = true;
	m_pCModel = 0;
}

I_Effect::~I_Effect(void) {
}

void I_Effect::DestroyTobMesh(CLEResManger* resMgr) {
	if (m_pCModel) {
		if (!IsTobMesh(m_strModelName)) {
			resMgr->DeleteMesh(*m_pCModel);
		}
		else {
			resMgr->DeleteTobMesh(*m_pCModel);
		}
	}
}


//£¡ 
#ifdef USE_RENDER
void I_Effect::Init(LERender* pDev, EFFECT_TYPE  eType, WPARAM wParam, LPARAM lParam)
#else
void I_Effect::Init(LPDIRECT3DDEVICE8 pDev, EFFECT_TYPE eType, WPARAM wParam, LPARAM lParam)
#endif

{
	m_pDev = pDev;
	_eEffectType = eType;
	_fLength = 5.0f;					// 2 seconds
	_wFrameCount = (WORD)wParam;		// Frame

	_vecFrameTime.resize(_wFrameCount);

	_vecFrameSize.resize(_wFrameCount);
	_vecFrameAngle.resize(_wFrameCount);
	_vecFramePos.resize(_wFrameCount);
	_vecFrameColor.resize(_wFrameCount);

	_iUseParam = 0;
	_CylinderParam.resize(_wFrameCount);

	for (WORD n = 0; n < _wFrameCount; n++) {
		_vecFrameTime[n] = _fLength / _wFrameCount;

		_vecFrameSize[n] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		_vecFrameAngle[n] = D3DXVECTOR3(0, 0, 0);
		_vecFramePos[n] = D3DXVECTOR3(0, 0, 0);
		_vecFrameColor[n] = D3DCOLOR_ARGB(255, 255, 255, 255);
	}

	// Select the VS type
	_bBillBoard = (lParam == 0 ? false : true);
	if (_bBillBoard) {
		_iVSIndex = 2;
	}

	if (_eEffectType == EFFECT_MODEL) {
		_iVSIndex = 1;
	}

	_vecFrameTime[0] = 1.75f;
	_vecFrameTime[1] = 1.25f;
	_vecFrameTime[2] = 2.00f;

	_vecFrameColor[0] = D3DCOLOR_ARGB(0, 255, 255, 255);
	_vecFrameColor[1] = D3DCOLOR_ARGB(0, 255, 255, 255);
	_vecFrameColor[2] = D3DCOLOR_ARGB(255, 255, 255, 255);

	m_pCModel = (CEffectModel*)lParam;
	m_strModelName = m_pCModel->m_strName;

	m_CTexCoordlist.GetCoordFromModel(m_pCModel);
	m_CTextruelist.GetTextureFromModel(m_pCModel);
}

void I_Effect::SetBillBoard(bool bBoard) {
	_bBillBoard = bBoard;
	if (_bBillBoard) {
		_iVSIndex = 2;
	}

	if (_eEffectType == EFFECT_MODEL) {
		_iVSIndex = 1;
	}
}

void I_Effect::Reset() {
	m_CTexCoordlist.Reset();
	m_CTextruelist.Reset();
}

void I_Effect::ReleaseAll() {
	_eEffectType = EFFECT_NONE;
	_fLength = 0.0f;
	_wFrameCount = 0;

	_vecFrameTime.clear();
	_vecFrameSize.clear();
	_vecFrameAngle.clear();
	_vecFramePos.clear();
	_vecFrameColor.clear();

	m_CTexCoordlist.Clear();
	m_CTextruelist.Clear();

	_CylinderParam.clear();
	m_pCModel = NULL;
	m_strModelName = "";

	m_strEffectName = "";
	_SpmatBBoard = NULL;

	_iVSIndex = 0;
	_pdwVShader = 0;
}

void I_Effect::BindingResInit(CLEResManger *m_CResMagr) {
	if (_eEffectType == EFFECT_FRAMETEX) {

		// TO REMOVE
	}
	else if (m_pCModel) {
		m_CTexCoordlist.GetCoordFromModel(m_pCModel);
		m_CTextruelist.GetTextureFromModel(m_pCModel);
		if (IsCylinderMesh(m_pCModel->m_strName)) {
			_iUseParam = 0;
			for (int n = 0; n < _wFrameCount; ++n) {
				_CylinderParam[n].iSegments = m_nSegments;
				_CylinderParam[n].fTopRadius = m_rRadius;
				_CylinderParam[n].fBottomRadius = m_rBotRadius;
				_CylinderParam[n].fHei = m_rHeight;
				_CylinderParam[n].Create();
			}
		}
	}
	_pdwVShader = m_CResMagr->GetVShaderByID(_iVSIndex);
}

int I_Effect::BoundingRes(CLEResManger	*m_CResMagr, char* pszParentName) {

	// Return 0 to normal load, 1 for missing signature, and 2 for missing model. 3 said less shade
	int t_iID = 0;

	// Load the texture
	if (_eEffectType == EFFECT_FRAMETEX) {
		for (WORD n = 0; n < m_CTexFrame.m_wTexCount; ++n) {
			m_CTexFrame.m_vecTexs[n] = m_CResMagr->GetTextureByNamedr(m_CTexFrame.m_vecTexName[n]);
			if (!m_CTexFrame.m_vecTexs[n]) {
				char pszMsg[64];
				_snprintf_s(pszMsg, 64, _TRUNCATE, "Exception file [%s] Error loading postcode [%s]", pszParentName, m_CTextruelist.m_vecTexName.c_str());
				LG("ERROR", "msg%s", pszMsg);
				return 1;
			}
		}
	}
	else {
		m_CTextruelist.m_pTex = m_CResMagr->GetTextureByNamedr(m_CTextruelist.m_vecTexName);
		if (!m_CTextruelist.m_pTex) {
			char pszMsg[64];
			_snprintf_s(pszMsg, 64, _TRUNCATE, "Exception file [%s] Error loading postcode [%s]", pszParentName, m_CTextruelist.m_vecTexName.c_str());
			LG("ERROR", "msg%s", pszMsg);
			return 1;
		}
		m_CTextruelist.m_lpCurTex = m_CTextruelist.m_pTex->GetTex();
	}


	_pdwVShader = m_CResMagr->GetVShaderByID(_iVSIndex);
	_SpmatBBoard = m_CResMagr->GetBBoardMat();

	// Load the model
	if (m_pCModel && m_pCModel->m_strName == m_strModelName) {
		return 0;
	}

	if (m_pCModel) {
		if (!IsTobMesh(m_strModelName)) {
			m_CResMagr->DeleteMesh(*m_pCModel);
			m_pCModel = 0;
		}
		else {
			m_CResMagr->DeleteTobMesh(*m_pCModel);
			m_pCModel = 0;
		}
	}

	if (!_bBillBoard && IsTobMesh(m_strModelName)) {
		if (!m_pCModel) {
			m_pCModel = m_CResMagr->NewTobMesh();
			if (!m_pCModel->CreateTob(m_strModelName, m_nSegments, m_rHeight, m_rRadius, m_rBotRadius)) {
				char pszMsg[64];
				_snprintf_s(pszMsg, 64, _TRUNCATE, "Exception File [%s] Missing Variable Model [%s]", pszParentName, m_strModelName.c_str());
				LG("ERROR", "msg%s", pszMsg);
				return 2;
			}
		}
	}
	else {
		m_pCModel = m_CResMagr->GetMeshByName(m_strModelName);
		if (!m_pCModel) {
			char pszMsg[64];
			_snprintf_s(pszMsg, 64, _TRUNCATE, "Effects file [%s] Error loading model [%s]", pszParentName, m_CTextruelist.m_vecTexName.c_str());
			LG("ERROR", "msg%s", pszMsg);
			return 2;
		}

		if (_bBillBoard) {
			m_strModelName = MESH_PLANERECT;
		}
		else {
			if (_eEffectType == EFFECT_FRAMETEX) {
				m_CTexFrame.GetCoordFromModel(m_pCModel);
			}
		}
	}

	return 0;
}

void I_Effect::SetModel(CEffectModel*	pCModel) {
	if (m_pCModel && !IsTobMesh(m_pCModel->m_strName)) {
		m_pCModel->SetUsing(false);
		m_pCModel = 0;
	}
	m_pCModel = pCModel;
	m_strModelName = m_pCModel->m_strName;
}

void I_Effect::PlayModel() {
	if (!m_pCModel) {
		return;
	}

	if (m_pCModel->IsItem()) {
		m_pCModel->PlayDefaultAnimation();
	}
}

void I_Effect::ResetModel() {
	if (m_pCModel->IsItem()) {
		m_pCModel->SetUsing(false);
		m_pCModel = 0;
	}
}

void I_Effect::DeleteItem(CLEResManger* pResMgr) {
	if (m_pCModel) {
		if (!IsTobMesh(m_strModelName)) {
			pResMgr->DeleteMesh(*m_pCModel);
			m_pCModel = 0;
		}
		else {
			pResMgr->DeleteTobMesh(*m_pCModel);
			m_pCModel = 0;
		}
	}
}

void I_Effect::GetRes(CLEResManger *pCResMagr, std::vector<INT>& vecTex, std::vector<INT>& vecModel) {
	int t_iID = 0;
	std::vector<INT>::iterator it;
	if (_eEffectType == EFFECT_FRAMETEX) {
		for (WORD n = 0; n < m_CTexFrame.m_wTexCount; ++n) {
			t_iID = pCResMagr->GetTextureID(m_CTexFrame.m_vecTexName[n]);
			if (t_iID != -1) {
				it = std::find(vecTex.begin(), vecTex.end(), t_iID);
				if (it == vecTex.end()) {
					vecTex.push_back(t_iID);
				}
			}
		}
	}
	else {
		t_iID = pCResMagr->GetTextureID(m_CTextruelist.m_vecTexName);
		if (t_iID != -1) {
			it = std::find(vecTex.begin(), vecTex.end(), t_iID);
			if (it == vecTex.end()) {
				vecTex.push_back(t_iID);
			}
		}
	}

	if (_bBillBoard) {

		// TO REMOVE: Edit the condition.
	}
	else {
		if (IsTobMesh(m_strModelName)) {

			// TO REMOVE: Edit the condition.
		}
		else {
			t_iID = pCResMagr->GetMeshID(m_strModelName);
			if (t_iID >= 7) {
				it = std::find(vecModel.begin(), vecModel.end(), t_iID);
				if (it == vecModel.end()) {
					vecModel.push_back(t_iID);
				}
			}
		}
	}
}

void I_Effect::ChangeModel(CEffectModel* pCModel, CLEResManger *pCResMagr) {
	if (!pCModel) {
		return;
	}

	if (_bBillBoard) {
		if (pCModel->m_strName != MESH_PLANERECT) {
			return;
		}
	}

	if (m_pCModel && !IsTobMesh(m_pCModel->m_strName)) {
		pCResMagr->DeleteMesh(*m_pCModel);
		m_pCModel = 0;
	}

	if (IsTobMesh(pCModel->m_strName)) {
		if (IsTobMesh(m_pCModel->m_strName)) {
			pCResMagr->DeleteTobMesh(*pCModel);
		}

		m_pCModel = pCResMagr->NewTobMesh();
		m_pCModel->CreateTob(pCModel->m_strName, pCModel->m_nSegments, pCModel->m_rHeight, pCModel->m_rRadius, pCModel->m_rBotRadius);
		m_strModelName = pCModel->m_strName;

		m_nSegments = pCModel->m_nSegments;
		m_rHeight = pCModel->m_rHeight;
		m_rRadius = pCModel->m_rRadius;
		m_rBotRadius = pCModel->m_rBotRadius;
		if (IsCylinderMesh(pCModel->m_strName)) {
			_iUseParam = 0;
			_CylinderParam.resize(_wFrameCount);
			for (int n = 0; n < _wFrameCount; ++n) {
				_CylinderParam[n].iSegments = m_nSegments;
				_CylinderParam[n].fTopRadius = m_rRadius;
				_CylinderParam[n].fBottomRadius = m_rBotRadius;
				_CylinderParam[n].fHei = m_rHeight;
				_CylinderParam[n].Create();
			}
		}
		return;
	}
	else {
		m_pCModel = pCModel;
		m_strModelName = pCModel->m_strName;
		if (_eEffectType == EFFECT_FRAMETEX) {
			m_CTexFrame.GetCoordFromModel(m_pCModel);
		}
		else {
			m_CTextruelist.GetTextureFromModel(m_pCModel);
		}
	}
}

// Save the effect to the file
bool I_Effect::SaveToFile(FILE* pFile) {
	char t_pszName[32];
	lstrcpy(t_pszName, m_strEffectName.c_str());
	fwrite(t_pszName, sizeof(char), 32, pFile);

	int t_temp;

	// Types of
	t_temp = (int)_eEffectType;
	fwrite(&t_temp, sizeof(int), 1, pFile);

	t_temp = (int)_eSrcBlend;
	fwrite(&t_temp, sizeof(int), 1, pFile);

	t_temp = (int)_eDestBlend;
	fwrite(&t_temp, sizeof(int), 1, pFile);

	// Save the model transformation
	// Length of time
	fwrite(&_fLength, sizeof(float), 1, pFile);

	// Number of frames
	fwrite(&_wFrameCount, sizeof(WORD), 1, pFile);

	// Every frame of time
	WORD n = 0;
	for (n = 0; n < _wFrameCount; n++) {
		fwrite(&_vecFrameTime[n], sizeof(float), 1, pFile);
	}

	// Each frame size
	for (n = 0; n < _wFrameCount; n++) {
		fwrite(&_vecFrameSize[n], sizeof(D3DXVECTOR3), 1, pFile);
	}

	// Each frame angle
	for (n = 0; n < _wFrameCount; n++) {
		fwrite(&_vecFrameAngle[n], sizeof(D3DXVECTOR3), 1, pFile);
	}

	// Each frame position
	for (n = 0; n < _wFrameCount; n++) {
		fwrite(&_vecFramePos[n], sizeof(D3DXVECTOR3), 1, pFile);
	}

	// Each frame color
	for (n = 0; n < _wFrameCount; n++) {
		fwrite(&_vecFrameColor[n], sizeof(D3DXCOLOR), 1, pFile);
	}

	// Save coordinate transformation
	// Number of vertices
	fwrite(&m_CTexCoordlist.m_wVerCount, sizeof(WORD), 1, pFile);

	// The number of coordinates
	fwrite(&m_CTexCoordlist.m_wCoordCount, sizeof(WORD), 1, pFile);

	// Each frame coordinate transformation time
	fwrite(&m_CTexCoordlist.m_fFrameTime, sizeof(float), 1, pFile);
	for (n = 0; n < m_CTexCoordlist.m_wCoordCount; n++) {

		// TODO
		std::vector<D3DXVECTOR2>::pointer pt = &m_CTexCoordlist.m_vecCoordList[n][0];

		// fwrite(m_CTexCoordlist.m_vecCoordList[n]._Myfirst, sizeof(D3DXVECTOR2), m_CTexCoordlist.m_wVerCount, pFile);
		fwrite(pt, sizeof(D3DXVECTOR2), m_CTexCoordlist.m_wVerCount, pFile);
	}

	// Save texture frame transform
	// m_CTextruelist.GetTextureFromModel(m_pCModel);
	fwrite(&m_CTextruelist.m_wTexCount, sizeof(WORD), 1, pFile);

	// Each frame coordinate transformation time
	fwrite(&m_CTextruelist.m_fFrameTime, sizeof(float), 1, pFile);

	lstrcpy(t_pszName, m_CTextruelist.m_vecTexName.c_str());
	fwrite(t_pszName, sizeof(char), 32, pFile);
	for (n = 0; n < m_CTextruelist.m_wTexCount; n++) {

		// TODO
		// fwrite(m_CTextruelist.m_vecTexList[n]._Myfirst, sizeof(D3DXVECTOR2), m_CTexCoordlist.m_wVerCount, pFile);
		std::vector<D3DXVECTOR2>::pointer pt = &m_CTextruelist.m_vecTexList[n][0];
		fwrite(pt, sizeof(D3DXVECTOR2), m_CTexCoordlist.m_wVerCount, pFile);
	}

	// Save the model name
	if (m_pCModel) {
		lstrcpy(t_pszName, m_pCModel->m_strName.c_str());
	}
	else {
		lstrcpy(t_pszName, "None");
	}

	fwrite(t_pszName, sizeof(char), 32, pFile);
	fwrite(&_bBillBoard, sizeof(bool), 1, pFile);
	fwrite(&_iVSIndex, sizeof(int), 1, pFile);

	// Save the transformable model
	if (m_pCModel) {
		fwrite(&m_pCModel->m_nSegments, sizeof(int), 1, pFile);
		fwrite(&m_pCModel->m_rHeight, sizeof(float), 1, pFile);
		fwrite(&m_pCModel->m_rRadius, sizeof(float), 1, pFile);
		fwrite(&m_pCModel->m_rBotRadius, sizeof(float), 1, pFile);
	}

	fwrite(&m_CTexFrame.m_wTexCount, sizeof(WORD), 1, pFile);
	fwrite(&m_CTexFrame.m_fFrameTime, sizeof(float), 1, pFile);
	for (n = 0; n < m_CTexFrame.m_wTexCount; ++n) {
		lstrcpy(t_pszName, m_CTexFrame.m_vecTexName[n].c_str());
		fwrite(t_pszName, sizeof(char), 32, pFile);
	}
	fwrite(&m_CTexFrame.m_fFrameTime, sizeof(float), 1, pFile);

	fwrite(&_iUseParam, sizeof(int), 1, pFile);
	if (_iUseParam > 0) {
		for (int n = 0; n < _wFrameCount; ++n) {
			fwrite(&_CylinderParam[n].iSegments, sizeof(int), 1, pFile);
			fwrite(&_CylinderParam[n].fHei, sizeof(float), 1, pFile);
			fwrite(&_CylinderParam[n].fTopRadius, sizeof(float), 1, pFile);
			fwrite(&_CylinderParam[n].fBottomRadius, sizeof(float), 1, pFile);
		}
	}
	fwrite(&_bRotaLoop, sizeof(bool), 1, pFile);
	fwrite(&_vRotaLoop, sizeof(D3DXVECTOR4), 1, pFile);
	fwrite(&_bAlpha, sizeof(bool), 1, pFile);
	fwrite(&_bRotaBoard, sizeof(bool), 1, pFile);

	return true;
}

// Load effects from files 
bool I_Effect::LoadFromFile(FILE* pFile, DWORD dwVersion) {
	ReleaseAll();
	char t_pszName[32];
	fread(t_pszName, sizeof(char), 32, pFile);
	m_strEffectName = t_pszName;

	int t_temp;

	// Types of
	fread(&t_temp, sizeof(int), 1, pFile);
	_eEffectType = (EFFECT_TYPE)t_temp;

	fread(&t_temp, sizeof(int), 1, pFile);
	_eSrcBlend = (D3DBLEND)t_temp;

	fread(&t_temp, sizeof(int), 1, pFile);
	_eDestBlend = (D3DBLEND)t_temp;

	// Read model transformation
	// Length of time
	fread(&_fLength, sizeof(float), 1, pFile);

	// Number of frames
	fread(&_wFrameCount, sizeof(WORD), 1, pFile);

	// Every frame of time
	_vecFrameTime.resize(_wFrameCount);

	WORD n = 0;
	for (n = 0; n < _wFrameCount; n++) {
		fread(&_vecFrameTime[n], sizeof(float), 1, pFile);
	}

	// Each frame size
	_vecFrameSize.resize(_wFrameCount);
	for (n = 0; n < _wFrameCount; n++) {
		fread(&_vecFrameSize[n], sizeof(D3DXVECTOR3), 1, pFile);
	}

	// Each frame angle
	_vecFrameAngle.resize(_wFrameCount);
	for (n = 0; n < _wFrameCount; n++) {
		fread(&_vecFrameAngle[n], sizeof(D3DXVECTOR3), 1, pFile);
	}

	// Each frame position
	_vecFramePos.resize(_wFrameCount);
	for (n = 0; n < _wFrameCount; n++) {
		fread(&_vecFramePos[n], sizeof(D3DXVECTOR3), 1, pFile);
	}

	// Each frame color
	_vecFrameColor.resize(_wFrameCount);
	for (n = 0; n < _wFrameCount; n++) {
		fread(&_vecFrameColor[n], sizeof(D3DXCOLOR), 1, pFile);
	}

	// Save coordinate transformation
	// Number of vertices
	fread(&m_CTexCoordlist.m_wVerCount, sizeof(WORD), 1, pFile);

	// The number of coordinates
	fread(&m_CTexCoordlist.m_wCoordCount, sizeof(WORD), 1, pFile);

	// Each frame coordinate transformation time
	fread(&m_CTexCoordlist.m_fFrameTime, sizeof(float), 1, pFile);
	m_CTexCoordlist.m_vecCoordList.resize(m_CTexCoordlist.m_wCoordCount);
	for (n = 0; n < m_CTexCoordlist.m_wCoordCount; n++) {
		m_CTexCoordlist.m_vecCoordList[n].resize(m_CTexCoordlist.m_wVerCount);
		
		// fread(m_CTexCoordlist.m_vecCoordList[n]._Myfirst, sizeof(D3DXVECTOR2),m_CTexCoordlist.m_wVerCount,pFile);
		// TODO
		std::vector<D3DXVECTOR2>::pointer pt = &m_CTexCoordlist.m_vecCoordList[n][0];

		fread(pt, sizeof(D3DXVECTOR2), m_CTexCoordlist.m_wVerCount, pFile);
	}

	// Save texture frame map transform
	// Number of Texture
	fread(&m_CTextruelist.m_wTexCount, sizeof(WORD), 1, pFile);

	// Each frame coordinate transformation time
	fread(&m_CTextruelist.m_fFrameTime, sizeof(float), 1, pFile);

	fread(t_pszName, sizeof(char), 32, pFile);

	// All converted to lowercase
	char psname[32];
	memset(psname, 0, 32);
	char *pszDataName;
	_strlwr_s(pszDataName = _strdup(t_pszName), strlen(t_pszName) + 1);

	if ((strstr(pszDataName, ".dds") == NULL) && strstr(pszDataName, ".tga") == NULL) {
		m_CTextruelist.m_vecTexName = pszDataName;
	}
	else {
		int len = lstrlen(pszDataName);
		memcpy(psname, pszDataName, len - 4);
		m_CTextruelist.m_vecTexName = psname;
	}
	SAFE_DELETE_ARRAY(pszDataName);

	m_CTextruelist.m_vecTexList.resize(m_CTextruelist.m_wTexCount);
	for (n = 0; n < m_CTextruelist.m_wTexCount; n++) {
		m_CTextruelist.m_vecTexList[n].resize(m_CTexCoordlist.m_wVerCount);

		// fread(m_CTextruelist.m_vecTexList[n]._Myfirst, sizeof(D3DXVECTOR2),m_CTexCoordlist.m_wVerCount,pFile);
		// TODO
		std::vector<D3DXVECTOR2>::pointer pt = &m_CTextruelist.m_vecTexList[n][0];
		fread(pt, sizeof(D3DXVECTOR2), m_CTexCoordlist.m_wVerCount, pFile);
	}

	fread(t_pszName, sizeof(char), 32, pFile);
	m_strModelName = t_pszName;

	fread(&_bBillBoard, sizeof(bool), 1, pFile);
	fread(&_iVSIndex, sizeof(int), 1, pFile);

	if (dwVersion > 1) {

		// Read the transformable model
		fread(&m_nSegments, sizeof(int), 1, pFile);
		fread(&m_rHeight, sizeof(float), 1, pFile);
		fread(&m_rRadius, sizeof(float), 1, pFile);
		fread(&m_rBotRadius, sizeof(float), 1, pFile);
	}

	if (dwVersion > 2) {
		fread(&m_CTexFrame.m_wTexCount, sizeof(WORD), 1, pFile);
		fread(&m_CTexFrame.m_fFrameTime, sizeof(float), 1, pFile);

		m_CTexFrame.m_vecTexName.resize(m_CTexFrame.m_wTexCount);
		m_CTexFrame.m_vecTexs.resize(m_CTexFrame.m_wTexCount);

		for (n = 0; n < m_CTexFrame.m_wTexCount; ++n) {
			fread(t_pszName, sizeof(char), 32, pFile);
			m_CTexFrame.m_vecTexName[n] = t_pszName;
		}

		fread(&m_CTexFrame.m_fFrameTime, sizeof(float), 1, pFile);
	}
	_iUseParam = 0;
	_CylinderParam.resize(_wFrameCount);

	if (dwVersion > 3) {
		fread(&_iUseParam, sizeof(int), 1, pFile);
		if (_iUseParam > 0) {
			for (int n = 0; n < _wFrameCount; ++n) {
				fread(&_CylinderParam[n].iSegments, sizeof(int), 1, pFile);
				fread(&_CylinderParam[n].fHei, sizeof(float), 1, pFile);
				fread(&_CylinderParam[n].fTopRadius, sizeof(float), 1, pFile);
				fread(&_CylinderParam[n].fBottomRadius, sizeof(float), 1, pFile);
				_CylinderParam[n].Create();
			}
		}
		else {
			if (IsCylinderMesh(m_strModelName)) {
				for (int n = 0; n < _wFrameCount; ++n) {
					_CylinderParam[n].iSegments = m_nSegments;
					_CylinderParam[n].fTopRadius = m_rRadius;
					_CylinderParam[n].fBottomRadius = m_rBotRadius;
					_CylinderParam[n].fHei = m_rHeight;
					_CylinderParam[n].Create();
				}
			}
		}
	}
	else {
		if (IsCylinderMesh(m_strModelName)) {
			_iUseParam = 0;
			for (int n = 0; n < _wFrameCount; ++n) {
				_CylinderParam[n].iSegments = m_nSegments;
				_CylinderParam[n].fTopRadius = m_rRadius;
				_CylinderParam[n].fBottomRadius = m_rBotRadius;
				_CylinderParam[n].fHei = m_rHeight;
				_CylinderParam[n].Create();
			}
		}
	}

	if (dwVersion > 4) {
		fread(&_bRotaLoop, sizeof(bool), 1, pFile);
		fread(&_vRotaLoop, sizeof(D3DXVECTOR4), 1, pFile);
	}

	if (dwVersion > 5) {
		fread(&_bAlpha, sizeof(bool), 1, pFile);
	}
	
	if (dwVersion > 6) {
		fread(&_bRotaBoard, sizeof(bool), 1, pFile);
	}

	IsSame();
	return true;
}

void I_Effect::CopyEffect(I_Effect* pEff) {
	m_pDev = pEff->m_pDev;;

	// Texture coordinate list
	m_CTexCoordlist.Copy(&pEff->m_CTexCoordlist);
	
	// Texture list
	m_CTextruelist.Copy(&pEff->m_CTextruelist);
	
	// Texture frame
	m_CTexFrame.Copy(&pEff->m_CTexFrame);

	if (m_pCModel && !IsTobMesh(m_pCModel->m_strName)) {
		m_pCModel->SetUsing(false);
		m_pCModel = 0;
	}
	m_pCModel = 0;
	m_strModelName = pEff->m_strModelName;

	m_nSegments = pEff->m_nSegments;
	m_rHeight = pEff->m_rHeight;
	m_rRadius = pEff->m_rRadius;
	m_rBotRadius = pEff->m_rBotRadius;

	m_strEffectName = pEff->m_strEffectName;
	_eEffectType = pEff->_eEffectType;
	_fLength = pEff->_fLength;
	_wFrameCount = pEff->_wFrameCount;

	_vecFrameTime.resize(_wFrameCount);
	_vecFrameSize.resize(_wFrameCount);
	_vecFrameAngle.resize(_wFrameCount);
	_vecFramePos.resize(_wFrameCount);
	_vecFrameColor.resize(_wFrameCount);

	_vecFrameTime = pEff->_vecFrameTime;
	_vecFrameSize = pEff->_vecFrameSize;
	_vecFrameAngle = pEff->_vecFrameAngle;
	_vecFramePos = pEff->_vecFramePos;
	_vecFrameColor = pEff->_vecFrameColor;

	_iUseParam = pEff->_iUseParam;
	_CylinderParam.resize(_wFrameCount);
	if (_iUseParam > 0) {
		_CylinderParam = pEff->_CylinderParam;
		for (int n = 0; n < _wFrameCount; ++n) {
			_CylinderParam[n].Create();
		}
	}
	else {
		if (IsCylinderMesh(m_strModelName)) {
			for (int n = 0; n < _wFrameCount; ++n) {
				_CylinderParam[n].iSegments = m_nSegments;
				_CylinderParam[n].fTopRadius = m_rRadius;
				_CylinderParam[n].fBottomRadius = m_rBotRadius;
				_CylinderParam[n].fHei = m_rHeight;
				_CylinderParam[n].Create();
			}
		}
	}

	_bBillBoard = pEff->_bBillBoard;
	_bRotaLoop = pEff->_bRotaLoop;
	_vRotaLoop = pEff->_vRotaLoop;
	_bRotaBoard = pEff->_bRotaBoard;
	_bAlpha = pEff->_bAlpha;

	_iVSIndex = pEff->_iVSIndex;
	_pdwVShader = NULL;

	_bSizeSame = pEff->_bSizeSame;
	_bAngleSame = pEff->_bAngleSame;
	_bPosSame = pEff->_bPosSame;
	_bColorSame = pEff->_bColorSame;

	_eSrcBlend = pEff->_eSrcBlend;
	_eDestBlend = pEff->_eDestBlend;
}

bool I_Effect::IsChangeably() {
	if (m_pCModel) {
		return m_pCModel->IsChangeably();
	}
	return true;
}

void I_Effect::IsSame() {
	_bSizeSame = true;
	_bAngleSame = true;
	_bPosSame = true;
	_bColorSame = true;

	int	 m;
	for (int n = 1; n < _wFrameCount; n++) {
		m = n - 1;
		if (_vecFrameSize[n] != _vecFrameSize[m]) {
			_bSizeSame = false;
		}

		if (_vecFrameAngle[n] != _vecFrameAngle[m]) {
			_bAngleSame = false;
		}

		if (_vecFramePos[n] != _vecFramePos[m]) {
			_bPosSame = false;
		}

		if (_vecFrameColor[n] != _vecFrameColor[m]) {
 			_bColorSame = false;
		}
	}
}

void I_Effect::SpliteTexture(int iRow, int iCol) {
	if (_eEffectType == EFFECT_FRAMETEX) {

		// TO REMOVE: change this condition.
	}
	else {
		m_CTextruelist.CreateSpliteTexture(iRow, iCol);
	}
}

void I_Effect::SetTextureTime(float ftime) {
	if (_eEffectType == EFFECT_FRAMETEX) {
		m_CTexFrame.m_fFrameTime = ftime;
	}
	else {
		m_CTextruelist.m_fFrameTime = ftime;
	}
}

void I_Effect::SetTexture() {
	if (IsItem()) {
		if (!m_pCModel) {
			return;
		}
		drITex* tex = NULL;
		drITex* tex2 = NULL;

		if (_eEffectType == EFFECT_FRAMETEX) {
			if (m_CTexFrame.m_lpCurTex && m_CTexFrame.m_lpCurTex->IsLoadingOK()) {
				m_pCModel->ResetItemTexture(0, m_CTexFrame.m_lpCurTex, &tex);
				m_pCModel->ResetItemTexture(1, m_CTexFrame.m_lpCurTex, &tex2);
			}
		}
		else {
			if (m_CTextruelist.m_pTex && m_CTextruelist.m_pTex->IsLoadingOK()) {
				m_pCModel->ResetItemTexture(0, m_CTextruelist.m_pTex, &tex);
				m_pCModel->ResetItemTexture(1, m_CTextruelist.m_pTex, &tex2);
			}
			else {
				return;
			}
		}
	}
	else {
		if (_eEffectType == EFFECT_FRAMETEX) {
			if (m_CTexFrame.m_lpCurTex && m_CTexFrame.m_lpCurTex->IsLoadingOK()) {
				m_pDev->SetTexture(0, m_CTexFrame.m_lpCurTex->GetTex());
			}
		}
		else {
			if (m_CTextruelist.m_pTex && m_CTextruelist.m_pTex->IsLoadingOK()) {
				m_pDev->SetTexture(0, m_CTextruelist.m_pTex->GetTex());
			}
			else {
				return;
			}
		}
	}
}

void I_Effect::SetVertexShader() {
	m_pDev->SetVertexShader(*_pdwVShader);
}

void I_Effect::Render() {
	m_pDev->SetRenderState(D3DRS_SRCBLEND, _eSrcBlend);
	m_pDev->SetRenderState(D3DRS_DESTBLEND, _eDestBlend);
	if (m_pCModel) {
		if (IsUseParam()) {
			m_pCModel->RenderTob(&_CylinderParam[m_ilast], &_CylinderParam[m_inext], m_flerp);
		}
		else {
			m_pCModel->RenderModel();
		}
	}
}

CEffectModel::CEffectModel() {
	m_pDev = NULL;
	m_strName = "";

#ifdef USE_MGR
	_drMesh = NULL;
#else
	_lpIB = NULL;
	_lpVB = NULL;
#endif

	m_pRes = NULL;
	m_vEffVer = 0;

	_dwVerCount = 0;
	_dwFaceCount = 0;

	m_bChangeably = false;
	m_oldtex = NULL;
	m_bItem = false;
}

bool CEffectModel::Copy(const CEffectModel& rhs) {
	ReleaseModel();
	LESceneItem::Copy(&rhs);
	InitDevice(rhs.m_pDev, rhs.m_pRes);
	GetObject()->GetPrimitive()->SetState(STATE_TRANSPARENT, 0);
	m_strName = rhs.m_strName;
	return true;
}

#ifdef USE_RENDER
CEffectModel::CEffectModel(LERender*  pDev, drIResourceMgr*	pRes)
#else
CEffectModel::CEffectModel(LPDIRECT3DDEVICE8  pDev, drIResourceMgr*	pRes)
#endif
{
	m_pDev = pDev;
	m_strName = "";

#ifdef USE_MGR
	_drMesh = NULL;
#else
	_lpIB = NULL;
	_lpVB = NULL;
#endif

	m_pRes = pRes;
	m_vEffVer = 0;

	_dwVerCount = 0;
	_dwFaceCount = 0;
	m_bChangeably = false;
	m_oldtex = NULL;
	m_oldtex2 = NULL;

	m_bItem = false;

	m_iID = -1;
}

CEffectModel::~CEffectModel() {
	ReleaseModel();
}

#ifdef USE_RENDER
void CEffectModel::InitDevice(LERender*	pDev, drIResourceMgr*	pRes)
#else
void CEffectModel::InitDevice(LPDIRECT3DDEVICE8  pDev, drIResourceMgr*	pRes)
#endif
{
	m_pDev = pDev;
	m_pRes = pRes;
}

#ifdef USE_RENDER
LERender* CEffectModel::GetDev()
#else
LPDIRECT3DDEVICE8 CEffectModel::GetDev()
#endif
{
	return m_pDev;
}

void CEffectModel::ReleaseModel() {
#ifdef USE_MGR
	if (!_drMesh) {
		if (m_bItem) {
			drITex*	oldtex;
			this->ResetItemTexture(0, m_oldtex, &oldtex);
			this->ResetItemTexture(1, m_oldtex2, &oldtex);
			Destroy();
		}
	}
	SAFE_RELEASE(_drMesh);
#else
	if (!_lpVB) {
		if (m_bItem) {
			drITex*	oldtex;
			this->ResetItemTexture(0, m_oldtex, &oldtex);
			this->ResetItemTexture(1, m_oldtex2, &oldtex);
			Destroy();
		}
	}
	SAFE_RELEASE(_lpIB);
	SAFE_RELEASE(_lpVB);
#endif

	SAFE_DELETE_ARRAY(m_vEffVer);
	m_bItem = false;
	m_iID = -1;
}

bool CEffectModel::CreateTriangle() {

	// If it is a good start to lock, you need to call g_mesh0-> LoadVideoMemory ();
	// because our vb was created at the time of the render
	_dwVerCount = 3;
	_dwFaceCount = 1;

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_VER_FVF;
	mi.pt_type = D3DPT_TRIANGLEFAN;
	mi.subset_num = 1;
	mi.vertex_num = 3;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);
	mi.vertex_seq[0] = drVector3(-0.5f, 0, 0);
	mi.vertex_seq[1] = drVector3(0, 0, 0.5f);
	mi.vertex_seq[2] = drVector3(0.5f, 0, 0);
	mi.blend_seq[0].weight[0] = 0;
	mi.blend_seq[1].weight[0] = 1;
	mi.blend_seq[2].weight[0] = 2;
	mi.vercol_seq[0] = 0xffffffff;
	mi.vercol_seq[1] = 0xffffffff;
	mi.vercol_seq[2] = 0xffffffff;
	mi.texcoord0_seq[0] = drVector2(0.0f, 1.0f);
	mi.texcoord0_seq[1] = drVector2(0.5f, 0);
	mi.texcoord0_seq[2] = drVector2(1.0f, 1.0f);
	drSubsetInfo_Construct(&mi.subset_seq[0], 1, 0, 3, 0);

	if (DR_FAILED(_drMesh->LoadSystemMemory(&mi))) {
 		return 0;
	}

	if (DR_FAILED(_drMesh->LoadVideoMemory())) {
		return 0;
	}

	_lpSVB = _drMesh->GetLockableStreamVB();
	_lpSIB = _drMesh->GetLockableStreamIB();

#else
	SEFFECT_VERTEX t_SEffVer[3];
	t_SEffVer[0].m_SPos = D3DXVECTOR3(-0.5f, 0, 0);
	t_SEffVer[0].m_fIdx = 0;
	t_SEffVer[0].m_dwDiffuse = 0xffffffff;
	t_SEffVer[0].m_SUV = D3DXVECTOR2(0, 1.0f);

	t_SEffVer[1].m_SPos = D3DXVECTOR3(0, 0, 0.5f);
	t_SEffVer[1].m_fIdx = 1;
	t_SEffVer[1].m_dwDiffuse = 0xffffffff;
	t_SEffVer[1].m_SUV = D3DXVECTOR2(0.5f, 0);

	t_SEffVer[2].m_SPos = D3DXVECTOR3(0.5f, 0, 0);
	t_SEffVer[2].m_fIdx = 2;
	t_SEffVer[2].m_dwDiffuse = 0xffffffff;
	t_SEffVer[2].m_SUV = D3DXVECTOR2(1.0f, 1.0f);

	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_VERTEX)* _dwVerCount,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_VER_FVF,
		D3DPOOL_DEFAULT, &_lpVB);
	
	if (FAILED(hr)) {
		return false;
	}

	SEFFECT_VERTEX *pVertex;
	_lpVB->Lock(0, 0, (BYTE**)&pVertex, D3DLOCK_NOOVERWRITE);
	memcpy(pVertex, t_SEffVer, sizeof(SEFFECT_VERTEX) * _dwVerCount);
	_lpVB->Unlock();

	hr = m_pDev->CreateIndexBuffer(sizeof(WORD)* _dwVerCount, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &_lpIB);
	if (FAILED(hr)) {
		return false;
	}

	WORD t_wIndex[3] = { 0, 1, 2, };
	WORD* t_pwIndex;
	_lpIB->Lock(0, 0, (BYTE**)&t_pwIndex, 0);
	memcpy(t_pwIndex, t_wIndex, sizeof(WORD) * _dwVerCount);
	_lpIB->Unlock();
#endif

	m_strName = MESH_TRI;
	return true;
}

bool CEffectModel::CreatePlaneTriangle() {
	_dwVerCount = 3;
	_dwFaceCount = 1;

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_VER_FVF;
	mi.pt_type = D3DPT_TRIANGLEFAN;
	mi.subset_num = 1;
	mi.vertex_num = 3;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);
	mi.vertex_seq[0] = drVector3(-0.5f, 0.5f, 0);
	mi.vertex_seq[1] = drVector3(0, -0.5f, 0);
	mi.vertex_seq[2] = drVector3(0.5f, 0.5f, 0);
	mi.blend_seq[0].weight[0] = 0;
	mi.blend_seq[1].weight[0] = 1;
	mi.blend_seq[2].weight[0] = 2;
	mi.vercol_seq[0] = 0xffffffff;
	mi.vercol_seq[1] = 0xffffffff;
	mi.vercol_seq[2] = 0xffffffff;
	mi.texcoord0_seq[0] = drVector2(0.0f, 1.0f);
	mi.texcoord0_seq[1] = drVector2(0.5f, 0);
	mi.texcoord0_seq[2] = drVector2(1.0f, 1.0f);
	drSubsetInfo_Construct(&mi.subset_seq[0], 1, 0, 3, 0);


	if (DR_FAILED(_drMesh->LoadSystemMemory(&mi))) {
		return 0;
	}

	if (DR_FAILED(_drMesh->LoadVideoMemory())) {
		return 0;
	}

	_lpSVB = _drMesh->GetLockableStreamVB();
	_lpSIB = _drMesh->GetLockableStreamIB();

#else
	SEFFECT_VERTEX t_SEffVer[3];
	t_SEffVer[0].m_SPos = D3DXVECTOR3(-0.5f, 0.5f, 0);
	t_SEffVer[0].m_fIdx = 0;
	t_SEffVer[0].m_dwDiffuse = 0xffffffff;
	t_SEffVer[0].m_SUV = D3DXVECTOR2(0, 1.0f);

	t_SEffVer[1].m_SPos = D3DXVECTOR3(0, -0.5f, 0);
	t_SEffVer[1].m_fIdx = 1;
	t_SEffVer[1].m_dwDiffuse = 0xffffffff;
	t_SEffVer[1].m_SUV = D3DXVECTOR2(0.5f, 0);

	t_SEffVer[2].m_SPos = D3DXVECTOR3(0.5f, 0.5f, 0);
	t_SEffVer[2].m_fIdx = 2;
	t_SEffVer[2].m_dwDiffuse = 0xffffffff;
	t_SEffVer[2].m_SUV = D3DXVECTOR2(1.0f, 1.0f);

	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_VERTEX)* _dwVerCount,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_VER_FVF,
		D3DPOOL_DEFAULT, &_lpVB);
	
	if (FAILED(hr)) {
		return false;
	}

	SEFFECT_VERTEX *pVertex;
	_lpVB->Lock(0, 0, (BYTE**)&pVertex, D3DLOCK_NOOVERWRITE);
	memcpy(pVertex, t_SEffVer, sizeof(SEFFECT_VERTEX) * _dwVerCount);
	_lpVB->Unlock();

	hr = m_pDev->CreateIndexBuffer(sizeof(WORD)* _dwVerCount, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &_lpIB);
	if (FAILED(hr)) {
		return false;
	}

	WORD t_wIndex[3] = { 0, 1, 2, };
	WORD*  t_pwIndex;
	_lpIB->Lock(0, 0, (BYTE**)&t_pwIndex, 0);
	memcpy(t_pwIndex, t_wIndex, sizeof(WORD) * _dwVerCount);
	_lpIB->Unlock();
#endif

	m_strName = MESH_PLANETRI;
	return true;
}

bool CEffectModel::CreateRect() {
	_dwVerCount = 4;
	_dwFaceCount = 2;

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_VER_FVF;
	mi.pt_type = D3DPT_TRIANGLEFAN;
	mi.subset_num = 1;
	mi.vertex_num = 4;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);
	mi.vertex_seq[0] = drVector3(-0.5f, 0, 0);
	mi.vertex_seq[1] = drVector3(-0.5f, 0, 1.0f);
	mi.vertex_seq[2] = drVector3(0.5f, 0, 1.0f);
	mi.vertex_seq[3] = drVector3(0.5f, 0, 0);

	mi.blend_seq[0].weight[0] = 0;
	mi.blend_seq[1].weight[0] = 1;
	mi.blend_seq[2].weight[0] = 2;
	mi.blend_seq[3].weight[0] = 3;

	mi.vercol_seq[0] = 0xffffffff;
	mi.vercol_seq[1] = 0xffffffff;
	mi.vercol_seq[2] = 0xffffffff;
	mi.vercol_seq[3] = 0xffffffff;

	mi.texcoord0_seq[0] = drVector2(0.0f, 1.0f);
	mi.texcoord0_seq[1] = drVector2(0.0f, 0);
	mi.texcoord0_seq[2] = drVector2(1.0f, 0.0f);
	mi.texcoord0_seq[3] = drVector2(1.0f, 1.0f);

	drSubsetInfo_Construct(&mi.subset_seq[0], 2, 0, 4, 0);

	_drMesh->LoadSystemMemory(&mi);
	_drMesh->LoadVideoMemory();
	_lpSVB = _drMesh->GetLockableStreamVB();
	_lpSIB = _drMesh->GetLockableStreamIB();
#else
	SEFFECT_VERTEX t_SEffVer[4];
	t_SEffVer[0].m_SPos = D3DXVECTOR3(-0.5f, 0, 0);
	t_SEffVer[0].m_fIdx = 0;
	t_SEffVer[0].m_dwDiffuse = 0xffffffff;
	t_SEffVer[0].m_SUV = D3DXVECTOR2(0, 1.0f);

	t_SEffVer[1].m_SPos = D3DXVECTOR3(-0.5f, 0, 1.0f);
	t_SEffVer[1].m_fIdx = 1;
	t_SEffVer[1].m_dwDiffuse = 0xffffffff;
	t_SEffVer[1].m_SUV = D3DXVECTOR2(0, 0);

	t_SEffVer[2].m_SPos = D3DXVECTOR3(0.5f, 0, 1.0f);
	t_SEffVer[2].m_fIdx = 2;
	t_SEffVer[2].m_dwDiffuse = 0xffffffff;
	t_SEffVer[2].m_SUV = D3DXVECTOR2(1.0f, 0.0f);

	t_SEffVer[3].m_SPos = D3DXVECTOR3(0.5f, 0, 0);
	t_SEffVer[3].m_fIdx = 3;
	t_SEffVer[3].m_dwDiffuse = 0xffffffff;
	t_SEffVer[3].m_SUV = D3DXVECTOR2(1.0f, 1.0f);

	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_VERTEX) * _dwVerCount,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_VER_FVF,
		D3DPOOL_DEFAULT, &_lpVB);
	
	if (FAILED(hr)) {
		return false;
	}

	SEFFECT_VERTEX *pVertex;
	_lpVB->Lock(0, 0, (BYTE**)&pVertex, D3DLOCK_NOOVERWRITE);
	memcpy(pVertex, t_SEffVer, sizeof(SEFFECT_VERTEX) * _dwVerCount);
	_lpVB->Unlock();

	hr = m_pDev->CreateIndexBuffer(sizeof(WORD) * 4, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &_lpIB);
	
	if (FAILED(hr)) {
		return false;
	}

	WORD t_wIndex[4] = { 0, 1, 2, 3, };
	WORD*  t_pwIndex;
	_lpIB->Lock(0, 0, (BYTE**)&t_pwIndex, 0);
	memcpy(t_pwIndex, t_wIndex, sizeof(WORD) * 4);
	_lpIB->Unlock();
#endif

	m_strName = MESH_RECT;
	return true;
}

bool CEffectModel::CreateRectZ() {
	_dwVerCount = 4;
	_dwFaceCount = 2;

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_VER_FVF;
	mi.pt_type = D3DPT_TRIANGLEFAN;
	mi.subset_num = 1;
	mi.vertex_num = 4;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);
	mi.vertex_seq[0] = drVector3(0, 0, 0);
	mi.vertex_seq[1] = drVector3(0, 0, 1);
	mi.vertex_seq[2] = drVector3(0, 1, 1);
	mi.vertex_seq[3] = drVector3(0, 1, 0);

	mi.blend_seq[0].weight[0] = 0;
	mi.blend_seq[1].weight[0] = 1;
	mi.blend_seq[2].weight[0] = 2;
	mi.blend_seq[3].weight[0] = 3;

	mi.vercol_seq[0] = 0xffffffff;
	mi.vercol_seq[1] = 0xffffffff;
	mi.vercol_seq[2] = 0xffffffff;
	mi.vercol_seq[3] = 0xffffffff;

	mi.texcoord0_seq[0] = drVector2(0.0f, 1.0f);
	mi.texcoord0_seq[1] = drVector2(0.0f, 0);
	mi.texcoord0_seq[2] = drVector2(1.0f, 0.0f);
	mi.texcoord0_seq[3] = drVector2(1.0f, 1.0f);

	drSubsetInfo_Construct(&mi.subset_seq[0], 2, 0, 4, 0);

	_drMesh->LoadSystemMemory(&mi);
	_drMesh->LoadVideoMemory();
	_lpSVB = _drMesh->GetLockableStreamVB();
	_lpSIB = _drMesh->GetLockableStreamIB();
#else
	SEFFECT_VERTEX t_SEffVer[4];
	t_SEffVer[0].m_SPos = D3DXVECTOR3(0, 0, 0);
	t_SEffVer[0].m_fIdx = 0;
	t_SEffVer[0].m_dwDiffuse = 0xffffffff;
	t_SEffVer[0].m_SUV = D3DXVECTOR2(0, 1.0f);

	t_SEffVer[1].m_SPos = D3DXVECTOR3(0, 0, 1);
	t_SEffVer[1].m_fIdx = 1;
	t_SEffVer[1].m_dwDiffuse = 0xffffffff;
	t_SEffVer[1].m_SUV = D3DXVECTOR2(0, 0);

	t_SEffVer[2].m_SPos = D3DXVECTOR3(0, 1, 1);
	t_SEffVer[2].m_fIdx = 2;
	t_SEffVer[2].m_dwDiffuse = 0xffffffff;
	t_SEffVer[2].m_SUV = D3DXVECTOR2(1.0f, 0.0f);

	t_SEffVer[3].m_SPos = D3DXVECTOR3(0, 1, 0);
	t_SEffVer[3].m_fIdx = 3;
	t_SEffVer[3].m_dwDiffuse = 0xffffffff;
	t_SEffVer[3].m_SUV = D3DXVECTOR2(1.0f, 1.0f);

	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_VERTEX) * _dwVerCount,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_VER_FVF,
		D3DPOOL_DEFAULT, &_lpVB);

	if (FAILED(hr)) {
		return false;
	}

	SEFFECT_VERTEX *pVertex;
	_lpVB->Lock(0, 0, (BYTE**)&pVertex, D3DLOCK_NOOVERWRITE);
	memcpy(pVertex, t_SEffVer, sizeof(SEFFECT_VERTEX) * _dwVerCount);
	_lpVB->Unlock();

	hr = m_pDev->CreateIndexBuffer(sizeof(WORD) * 4,
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED,
		&_lpIB);
	
	if (FAILED(hr)) {
		return false;
	}

	WORD t_wIndex[4] = { 0, 1, 2, 3, };
	WORD* t_pwIndex;
	_lpIB->Lock(0, 0, (BYTE**)&t_pwIndex, 0);
	memcpy(t_pwIndex, t_wIndex, sizeof(WORD) * 4);
	_lpIB->Unlock();
#endif

	m_strName = MESH_RECTZ;
	return true;
}

bool CEffectModel::CreatePlaneRect() {
	_dwVerCount = 4;
	_dwFaceCount = 2;

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_VER_FVF;
	mi.pt_type = D3DPT_TRIANGLEFAN;
	mi.subset_num = 1;
	mi.vertex_num = 4;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);
	mi.vertex_seq[0] = drVector3(-0.5f, -0.5f, 0);
	mi.vertex_seq[1] = drVector3(-0.5f, 0.5f, 0);
	mi.vertex_seq[2] = drVector3(0.5f, 0.5f, 0);
	mi.vertex_seq[3] = drVector3(0.5f, -0.5f, 0);

	mi.blend_seq[0].weight[0] = 0;
	mi.blend_seq[1].weight[0] = 1;
	mi.blend_seq[2].weight[0] = 2;
	mi.blend_seq[3].weight[0] = 3;

	mi.vercol_seq[0] = 0xffffffff;
	mi.vercol_seq[1] = 0xffffffff;
	mi.vercol_seq[2] = 0xffffffff;
	mi.vercol_seq[3] = 0xffffffff;

	mi.texcoord0_seq[0] = drVector2(0.0f, 1.0f);
	mi.texcoord0_seq[1] = drVector2(0.0f, 0);
	mi.texcoord0_seq[2] = drVector2(1.0f, 0.0f);
	mi.texcoord0_seq[3] = drVector2(1.0f, 1.0f);

	drSubsetInfo_Construct(&mi.subset_seq[0], 2, 0, 4, 0);

	_drMesh->LoadSystemMemory(&mi);
	_drMesh->LoadVideoMemory();
	_lpSVB = _drMesh->GetLockableStreamVB();
	_lpSIB = _drMesh->GetLockableStreamIB();
#else
	SEFFECT_VERTEX t_SEffVer[4];
	t_SEffVer[0].m_SPos = D3DXVECTOR3(-0.5f, -0.5f, 0);
	t_SEffVer[0].m_fIdx = 0;
	t_SEffVer[0].m_dwDiffuse = 0xffffffff;
	t_SEffVer[0].m_SUV = D3DXVECTOR2(0.0f, 1.0f);

	t_SEffVer[1].m_SPos = D3DXVECTOR3(-0.5f, 0.5f, 0);
	t_SEffVer[1].m_fIdx = 1;
	t_SEffVer[1].m_dwDiffuse = 0xffffffff;
	t_SEffVer[1].m_SUV = D3DXVECTOR2(0.0f, 0);

	t_SEffVer[2].m_SPos = D3DXVECTOR3(0.5f, 0.5f, 0);
	t_SEffVer[2].m_fIdx = 2;
	t_SEffVer[2].m_dwDiffuse = 0xffffffff;
	t_SEffVer[2].m_SUV = D3DXVECTOR2(1.0f, 0.0f);

	t_SEffVer[3].m_SPos = D3DXVECTOR3(0.5f, -0.5f, 0);
	t_SEffVer[3].m_fIdx = 3;
	t_SEffVer[3].m_dwDiffuse = 0xffffffff;
	t_SEffVer[3].m_SUV = D3DXVECTOR2(1.0f, 1.0f);

	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_VERTEX) * _dwVerCount,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_VER_FVF,
		D3DPOOL_DEFAULT, &_lpVB);

	if (FAILED(hr)) {
		return false;
	}

	SEFFECT_VERTEX *pVertex;
	_lpVB->Lock(0, 0, (BYTE**)&pVertex, D3DLOCK_NOOVERWRITE);
	memcpy(pVertex, t_SEffVer, sizeof(SEFFECT_VERTEX) * _dwVerCount);
	_lpVB->Unlock();

	hr = m_pDev->CreateIndexBuffer(sizeof(WORD) * 4,
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED,
		&_lpIB);
	
	if (FAILED(hr)) {
		return false;
	}

	WORD t_wIndex[4] = { 0, 1, 2, 3, };
	WORD*  t_pwIndex;
	_lpIB->Lock(0, 0, (BYTE**)&t_pwIndex, 0);
	memcpy(t_pwIndex, t_wIndex, sizeof(WORD) * 4);
	_lpIB->Unlock();
#endif

	m_strName = MESH_PLANERECT;
	return true;
}


bool CEffectModel::CreateCone(int nSeg, float fHei, float fRadius) {
	m_nSegments = nSeg;
	m_rHeight = fHei;
	m_rRadius = 0;
	m_rBotRadius = fRadius;

	_dwVerCount = m_nSegments * 3;
	_dwFaceCount = m_nSegments * 2;

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_VER_FVF;
	mi.pt_type = D3DPT_TRIANGLESTRIP;
	mi.subset_num = 1;
	mi.vertex_num = _dwVerCount;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);

	int nCurrentSegment;
	int idx = 0;

	float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
	float rSegmentLength = 1.0f / (float)m_nSegments;
	float ny0 = (90.0f - (float)D3DXToDegree(atan(m_rHeight / m_rBotRadius))) / 90.0f;
	for (nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++) {
		float x0 = m_rBotRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		float z0 = m_rBotRadius * cosf(nCurrentSegment * rDeltaSegAngle);

		mi.vertex_seq[idx].x = 0.0f;
		mi.vertex_seq[idx].z = m_rHeight;
		mi.vertex_seq[idx].y = 0.0f;
		mi.texcoord0_seq[idx].x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		mi.texcoord0_seq[idx].y = 0.0f;
		idx++;

		mi.vertex_seq[idx].x = x0;
		mi.vertex_seq[idx].z = 0.0f;
		mi.vertex_seq[idx].y = z0;
		mi.texcoord0_seq[idx].x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		mi.texcoord0_seq[idx].y = 1.5f;
		idx++;
	}

	for (WORD n = 0; n < _dwVerCount; n++) {
		mi.blend_seq[n].weight[0] = n;
		mi.vercol_seq[n] = 0xffffffff;
	}

	drSubsetInfo_Construct(&mi.subset_seq[0], _dwFaceCount, 0, _dwVerCount, 0);

	if (DR_FAILED(_drMesh->LoadSystemMemory(&mi))) {
		return 0;
	}

	if (DR_FAILED(_drMesh->LoadVideoMemory())) {
		return 0;
	}

	_lpSVB = _drMesh->GetLockableStreamVB();
#else
	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_VERTEX)*_dwVerCount,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_VER_FVF,
		D3DPOOL_DEFAULT, &_lpVB);

	if (FAILED(hr)) {
		return false;
	}

	SEFFECT_VERTEX* pVertex;
	int nCurrentSegment;

	// Lock the vertex buffer
	if (FAILED(_lpVB->Lock(0, 0, (BYTE**)&pVertex, 0))) {
		return false;
	}

	float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
	float rSegmentLength = 1.0f / (float)m_nSegments;
	float ny0 = (90.0f - (float)D3DXToDegree(atan(m_rHeight / m_rBotRadius))) / 90.0f;

	// For each segment, add a triangle to the sides triangle list
	for (nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++) {
		float x0 = m_rBotRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		float z0 = m_rBotRadius * cosf(nCurrentSegment * rDeltaSegAngle);

		pVertex->m_SPos.x = 0.0f;
		pVertex->m_SPos.z = m_rHeight;
		pVertex->m_SPos.y = 0.0f;
		pVertex->m_SUV.x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		pVertex->m_SUV.y = 0.0f;
		pVertex->m_dwDiffuse = 0xffffffff;
		pVertex++;

		pVertex->m_SPos.x = x0;
		pVertex->m_SPos.z = 0.0f;
		pVertex->m_SPos.y = z0;
		pVertex->m_SUV.x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		pVertex->m_SUV.y = 1.5f;
		pVertex->m_dwDiffuse = 0xffffffff;
		pVertex++;
	}

	if (FAILED(_lpVB->Unlock())) {
		return false;
	}
#endif

	m_strName = MESH_CONE;
	m_bChangeably = true;
	return true;
}

bool CEffectModel::CreateCylinder(int nSeg, float fHei, float fTopRadius, float fBottomRadius) {
	m_nSegments = nSeg;
	m_rHeight = fHei;
	m_rRadius = fTopRadius;
	m_rBotRadius = fBottomRadius;

	_dwVerCount = m_nSegments * 3;
	_dwFaceCount = m_nSegments * 2;

	m_vEffVer = new SEFFECT_VERTEX[_dwVerCount];

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_VER_FVF;
	mi.pt_type = D3DPT_TRIANGLESTRIP;
	mi.subset_num = 1;
	mi.vertex_num = _dwVerCount;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);

	int nCurrentSegment;
	int idx = 0;

	float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
	float rSegmentLength = 1.0f / (float)m_nSegments;
	float ny0 = (90.0f - (float)D3DXToDegree(atan(m_rHeight / m_rRadius))) / 90.0f;
	for (nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++) {
		float x0 = m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		float z0 = m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);

		mi.vertex_seq[idx].x = x0;
		mi.vertex_seq[idx].z = m_rHeight;
		mi.vertex_seq[idx].y = z0;

		m_vEffVer[idx].m_SPos = (D3DXVECTOR3)mi.vertex_seq[idx];

		mi.texcoord0_seq[idx].x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		mi.texcoord0_seq[idx].y = 0.0f;

		m_vEffVer[idx].m_SUV = (D3DXVECTOR2)mi.texcoord0_seq[idx];

		idx++;

		x0 = m_rBotRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		z0 = m_rBotRadius * cosf(nCurrentSegment * rDeltaSegAngle);
		mi.vertex_seq[idx].x = x0;
		mi.vertex_seq[idx].z = 0.0f;
		mi.vertex_seq[idx].y = z0;

		m_vEffVer[idx].m_SPos = (D3DXVECTOR3)mi.vertex_seq[idx];

		mi.texcoord0_seq[idx].x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		mi.texcoord0_seq[idx].y = 1.0f;

		m_vEffVer[idx].m_SUV = (D3DXVECTOR2)mi.texcoord0_seq[idx];

		idx++;
	}

	for (WORD n = 0; n < _dwVerCount; n++) {
		mi.blend_seq[n].weight[0] = n;
		mi.vercol_seq[n] = 0xffffffff;

		m_vEffVer[n].m_fIdx = n;
		m_vEffVer[n].m_dwDiffuse = 0xffffffff;
	}

	drSubsetInfo_Construct(&mi.subset_seq[0], _dwFaceCount, 0, _dwVerCount, 0);

	if (DR_FAILED(_drMesh->LoadSystemMemory(&mi))) {
		return 0;
	}

	if (DR_FAILED(_drMesh->LoadVideoMemory())) {
		return 0;
	}

	_lpSVB = _drMesh->GetLockableStreamVB();
#else
	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_VERTEX)*_dwVerCount,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_VER_FVF,
		D3DPOOL_DEFAULT, &_lpVB);

	if (FAILED(hr)) {
		return false;
	}


	SEFFECT_VERTEX* pVertex;
	int nCurrentSegment;


	// Lock the vertex buffer
	if (FAILED(_lpVB->Lock(0, 0, (BYTE**)&pVertex, 0))) {
		return false;
	}

	float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
	float rSegmentLength = 1.0f / (float)m_nSegments;
	float ny0 = (90.0f - (float)D3DXToDegree(atan(m_rHeight / m_rRadius))) / 90.0f;

	//For each segment, add a triangle to the sides triangle list
	for (nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++) {
		float x0 = m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		float z0 = m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);

		pVertex->m_SPos.x = x0;
		pVertex->m_SPos.z = m_rHeight;
		pVertex->m_SPos.y = z0;
		pVertex->m_SUV.x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		pVertex->m_SUV.y = 0.0f;
		pVertex->m_dwDiffuse = 0xffffffff;
		pVertex++;

		x0 = m_rBotRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		z0 = m_rBotRadius * cosf(nCurrentSegment * rDeltaSegAngle);
		pVertex->m_SPos.x = x0;
		pVertex->m_SPos.z = 0.0f;
		pVertex->m_SPos.y = z0;
		pVertex->m_SUV.x = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		pVertex->m_SUV.y = 1.0f;
		pVertex->m_dwDiffuse = 0xffffffff;
		pVertex++;
	}

	if (FAILED(_lpVB->Unlock())) {
		return false;
	}
#endif

	m_strName = MESH_CYLINDER;
	m_bChangeably = true;
	return true;
}

bool CEffectModel::CreateShadeModel(WORD wVerNum, WORD wFaceNum, int iGridCrossNum, bool usesoft) {
	_dwVerCount = wVerNum;
	_dwFaceCount = wFaceNum;

#ifdef USE_MGR
	m_pRes->CreateMesh(&_drMesh);
	_drMesh->SetStreamType(STREAM_LOCKABLE);

	drMeshInfo mi;
	mi.fvf = EFFECT_SHADE_FVF;
	mi.pt_type = D3DPT_TRIANGLELIST;
	mi.subset_num = 1;
	mi.vertex_num = wVerNum;
	mi.vertex_seq = DR_NEW(drVector3[mi.vertex_num]);
	mi.blend_seq = DR_NEW(drBlendInfo[mi.vertex_num]);
	mi.vercol_seq = DR_NEW(DWORD[mi.vertex_num]);
	mi.texcoord0_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.texcoord1_seq = DR_NEW(drVector2[mi.vertex_num]);
	mi.subset_seq = DR_NEW(drSubsetInfo[mi.subset_num]);
	mi.index_num = wFaceNum * 6;
	mi.index_seq = DR_NEW(DWORD[mi.index_num]);
	
	// Set the VS constant starting at 9
	int nIndex = 9;
	for (DWORD n = 0; n < mi.vertex_num; n++) {
		mi.vercol_seq[n] = 0xffffffff;

		// Must all be zero
		mi.vertex_seq[n] = drVector3(0, 0, 0);
		mi.texcoord0_seq[n] = drVector2(0, 0);
		mi.texcoord1_seq[n].x = (float)nIndex;
		nIndex++;
		mi.texcoord1_seq[n].y = (float)nIndex;
		nIndex++;
	}

	nIndex = 0;
	for (int nY = 0; nY < iGridCrossNum; nY++) {
		for (int nX = 0; nX < iGridCrossNum; nX++) {
			mi.index_seq[nIndex++] = nX + nY * (iGridCrossNum + 1);
			mi.index_seq[nIndex++] = (nX + 1) + nY * (iGridCrossNum + 1);
			mi.index_seq[nIndex++] = nX + (nY + 1) * (iGridCrossNum + 1);

			mi.index_seq[nIndex++] = nX + (nY + 1) * (iGridCrossNum + 1);
			mi.index_seq[nIndex++] = (nX + 1) + nY * (iGridCrossNum + 1);
			mi.index_seq[nIndex++] = (nX + 1) + (nY + 1) * (iGridCrossNum + 1);
		}
	}
	drSubsetInfo_Construct(&mi.subset_seq[0], wFaceNum, 0, wVerNum, 0);

	if (DR_FAILED(_drMesh->LoadSystemMemory(&mi))) {
		LG("error", "msg shadow model load system memory failed");
	}

	if (DR_FAILED(_drMesh->LoadVideoMemory())) {
		LG("error", "msg shadow model loading memory failed");
	}

	if (usesoft) {
		_lpSVB = _drMesh->GetLockableStreamVB();
		_lpSIB = _drMesh->GetLockableStreamIB();
	}
	else {
		_lpSVB = NULL;
		_lpSIB = NULL;
	}
#else
	HRESULT hr;
	hr = m_pDev->CreateVertexBuffer(sizeof(SEFFECT_SHADE_VERTEX) * MAX_SHADER_VERNUM,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		EFFECT_SHADE_FVF,
		D3DPOOL_DEFAULT, &_lpVB);

	if (FAILED(hr)) {
		return false;
	}

	SEFFECT_SHADE_VERTEX *pVertex;
	_lpVB->Lock(0, 0, (BYTE**)&pVertex, D3DLOCK_NOOVERWRITE);


	int nIndex = 9;
	for (int n = 0; n < MAX_SHADER_VERNUM; n++) {
		pVertex[n].m_dwDiffuse = 0xffffffff;
		pVertex[n].m_SPos = D3DXVECTOR3(0, 0, 0);
		pVertex[n].m_SUV = D3DXVECTOR2(0, 0);
		pVertex[n].m_SUV2.x = (float)nIndex;
		nIndex++;
		pVertex[n].m_SUV2.y = (float)nIndex;
		nIndex++;
	}

	_lpVB->Unlock();
#endif
	return true;
}

bool CEffectModel::LoadModel(const char* pszName) {
	m_strName = pszName;
	if (Load(pszName, 1) == 0) {
		m_oldtex = this->GetPrimitive()->GetMtlTexAgent(0)->GetTex(0);
		if (this->GetPrimitive()->GetMtlTexAgent(1)) {
			m_oldtex2 = this->GetPrimitive()->GetMtlTexAgent(1)->GetTex(0);
		}
		else {
			m_oldtex2 = NULL;
		}
		m_bItem = true;
	}
	else {
		m_oldtex = NULL;
		m_oldtex2 = NULL;
		m_bItem = false;
	}
	return true;
}

void CEffectModel::FrameMove(DWORD dwDailTime) {

#ifdef USE_MGR
	if (!_drMesh) {
		LESceneItem::FrameMove();
	}
#else
	if (!_lpVB) {
		LESceneItem::FrameMove();
	}
#endif
}

void CEffectModel::Begin() {

	// Use the vs command instead

#ifdef USE_MGR
	if (_drMesh) {
		_drMesh->BeginSet();
	}
#endif
}

void CEffectModel::SetRenderNum(WORD wVer, WORD wFace) {

#ifdef USE_MGR
	drMeshInfo* mi = _drMesh->GetMeshInfo();
	drSubsetInfo_Construct(&mi->subset_seq[0], wFace, 0, wVer, 0);
#endif

}

void CEffectModel::RenderModel() {

#ifdef USE_MGR
	if (!_drMesh) {
		this->GetPrimitive()->RenderSubset(0);
		if (this->GetPrimitive()->GetMtlTexAgent(1)) {
			m_pDev->SetRenderStateForced(D3DRS_ALPHABLENDENABLE, TRUE);
			this->GetPrimitive()->RenderSubset(1);
		}
	}
#else
	if (!_lpVB) {
		LESceneItem::Render();
	}
#endif
	else {

#ifdef USE_MGR
		if (DR_FAILED(_drMesh->DrawSubset(0))) {
			LG("error", "CEffectModel %s", (TCHAR*)m_strName.c_str());
		}
#else
		m_pDev->SetStreamSource(0, _lpVB, sizeof(SEFFECT_VERTEX));
		if (m_strName == MESH_CONE) {
			m_pDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, _dwFaceCount);
			return;
		}

		if (m_strName == MESH_CYLINDER) {
			m_pDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, _dwFaceCount);
			return;
		}

		m_pDev->SetIndices(_lpIB, 0);
		m_pDev->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, _dwVerCount, 0, _dwFaceCount);
#endif
	}
}

void CEffectModel::End() {

#ifdef USE_MGR
	if (_drMesh) {
		_drMesh->EndSet();
	}
#endif

}

void CEffectModel::RenderTob(ModelParam* last, ModelParam* next, float lerp) {
	for (WORD n = 0; n < _dwVerCount; ++n) {
		D3DXVec3Lerp(&m_vEffVer[n].m_SPos, &last->vecVer[n], &next->vecVer[n], lerp);
	}
	m_pDev->SetVertexShader(EFFECT_VER_FVF);
	m_pDev->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, _dwFaceCount, m_vEffVer, sizeof(SEFFECT_VERTEX));
}

void ModelParam::Create() {
	DWORD dwVerCount = iSegments * 3;
	vecVer.resize(dwVerCount);
	int nCurrentSegment;
	int idx = 0;
	float rDeltaSegAngle = (2.0f * D3DX_PI / iSegments);
	float rSegmentLength = 1.0f / (float)iSegments;
	float ny0 = (90.0f - (float)D3DXToDegree(atan(fHei / fTopRadius))) / 90.0f;
	for (nCurrentSegment = 0; nCurrentSegment <= iSegments; nCurrentSegment++) {
		float x0 = fTopRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		float z0 = fTopRadius * cosf(nCurrentSegment * rDeltaSegAngle);

		vecVer[idx].x = x0;
		vecVer[idx].z = fHei;
		vecVer[idx].y = z0;

		idx++;

		x0 = fBottomRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		z0 = fBottomRadius * cosf(nCurrentSegment * rDeltaSegAngle);
		vecVer[idx].x = x0;
		vecVer[idx].z = 0.0f;
		vecVer[idx].y = z0;

		idx++;
	}
}

CTexCoordList::CTexCoordList() {
	Clear();
}

CTexCoordList::~CTexCoordList() {
}

void CTexCoordList::Clear() {
	m_wVerCount = 0;
	
	// Texture coordinate number
	m_wCoordCount = 0;
	
	// Texture coordinate transformation time
	m_fFrameTime = 0.0f;

	// Texture coordinate transformation sequence
	m_vecCoordList.clear();
}

void CTexCoordList::CreateTranslateCoord() {
	m_fFrameTime = 4.0f;

	m_wVerCount = 4;
	m_wCoordCount = 2;
	m_vecCoordList.resize(m_wCoordCount);
	D3DXVECTOR2 t_SVer[4];
	m_vecCoordList[0].resize(m_wVerCount);
	m_vecCoordList[1].resize(m_wVerCount);
	t_SVer[0] = D3DXVECTOR2(0, 0.1f);
	t_SVer[1] = D3DXVECTOR2(0, 0.0f);
	t_SVer[2] = D3DXVECTOR2(1.0f, 0.0f);
	t_SVer[3] = D3DXVECTOR2(1.0f, 0.1f);

	WORD i = 0;
	for (i = 0; i < m_wVerCount; ++i) {
		m_vecCoordList[0][i] = t_SVer[i];
	}

	t_SVer[0] = D3DXVECTOR2(0, 1.2f);
	t_SVer[1] = D3DXVECTOR2(0, 0.0f);
	t_SVer[2] = D3DXVECTOR2(1.0f, 0.0f);
	t_SVer[3] = D3DXVECTOR2(1.0f, 1.2f);

	for (i = 0; i < m_wVerCount; ++i) {
		m_vecCoordList[1][i] = t_SVer[i];
	}
}

void CTexCoordList::GetCoordFromModel(CEffectModel *pCModel) {
	if (!pCModel->IsBoard()) {
		return;
	}

	m_wVerCount = (WORD)pCModel->GetVerCount();
	m_wCoordCount = 1;
	m_fFrameTime = 3.0f;
	m_vecCoordList.clear();
	m_vecCoordList.resize(m_wCoordCount);

	SEFFECT_VERTEX *pVertex;
	pCModel->Lock((BYTE**)&pVertex);

	for (WORD i = 0; i < m_wCoordCount; i++) {
		m_vecCoordList[i].resize(m_wVerCount);
		for (DWORD n = 0; n < m_wVerCount; n++) {
			m_vecCoordList[i][n] = pVertex[n].m_SUV;
		}
	}

	pCModel->Unlock();
}

void CTexCoordList::Reset() {
}

void CTexCoordList::GetCurCoord(S_BVECTOR<D3DXVECTOR2>& vecOutCoord, WORD& wCurIndex, float &fCurTime, float fDailTime) {
	if (m_wCoordCount == 1) {
		vecOutCoord.clear();
		for (int n = 0; n < m_wVerCount; ++n) {
			vecOutCoord.push_back(m_vecCoordList[0][n]);
		}
		return;
	}

	WORD t_wNextIndex;
	float t_fLerp;
	fCurTime += fDailTime;
	if (fCurTime > m_fFrameTime) {
		wCurIndex++;
		fCurTime = 0.0f;
	}

	if (wCurIndex >= m_wCoordCount) {
		wCurIndex = 0;
	}

	if (wCurIndex == m_wCoordCount - 1) {
		t_wNextIndex = wCurIndex;
	}
	else {
		t_wNextIndex = wCurIndex + 1;
	}
	t_fLerp = fCurTime / m_fFrameTime;

	for (WORD n = 0; n < m_wVerCount; ++n) {
		D3DXVec2Lerp(vecOutCoord[n], &m_vecCoordList[wCurIndex][n], &m_vecCoordList[t_wNextIndex][n], t_fLerp);
	}
}

CTexList::CTexList() {
	Clear();
}

CTexList::~CTexList() {
	m_vecTexList.clear();
}

void CTexList::Clear() {
	m_wTexCount = 0;
	m_fFrameTime = 0.1f;

	m_vecTexName = "";
	m_vecTexList.clear();

	m_lpCurTex = NULL;
}

void CTexList::SetTextureName(const s_string&  pszName) {
	m_vecTexName = pszName;
}

void CTexList::Reset() {
}

// This method only applies to four vertex patches
void CTexList::CreateSpliteTexture(int iRow, int iColnum) {
	m_wTexCount = iRow * iColnum;
	float fw = 1.0f / iRow;
	float fh = 1.0f / iColnum;

	D3DXVECTOR2 suv[4];
	suv[0] = D3DXVECTOR2(0, 1.0f);
	suv[1] = D3DXVECTOR2(0, 0);
	suv[2] = D3DXVECTOR2(1.0f, 0.0f);
	suv[3] = D3DXVECTOR2(1.0f, 1.0f);

	m_vecTexList.clear();
	m_vecTexList.resize(m_wTexCount);
	for (WORD h = 0; h < iColnum; h++) {
		for (WORD w = 0; w < iRow; w++) {
			m_vecTexList[w + h * iRow].resize(4);

			m_vecTexList[w + h * iRow][0].x = w * fw + suv[0].x;
			m_vecTexList[w + h * iRow][0].y = h * fh + fh;

			m_vecTexList[w + h * iRow][1].x = w * fw + suv[1].x;
			m_vecTexList[w + h * iRow][1].y = h * fh;

			m_vecTexList[w + h * iRow][2].x = m_vecTexList[w + h * iRow][1].x + fw;
			m_vecTexList[w + h * iRow][2].y = m_vecTexList[w + h * iRow][1].y;

			m_vecTexList[w + h * iRow][3].x = m_vecTexList[w + h * iRow][0].x + fw;
			m_vecTexList[w + h * iRow][3].y = m_vecTexList[w + h * iRow][0].y;
		}
	}
}

void CTexList::GetTextureFromModel(CEffectModel *pCModel) {
	if (!pCModel) {
		return;
	}
	
	if (!pCModel->IsBoard()) {
		return;
	}

	WORD t_wVerCount = (WORD)pCModel->GetVerCount();
	m_wTexCount = 1;
	m_vecTexList.clear();
	m_vecTexList.resize(m_wTexCount);

	SEFFECT_VERTEX *pVertex;
	pCModel->Lock((BYTE**)&pVertex);

	for (WORD i = 0; i < m_wTexCount; i++) {
		m_vecTexList[i].resize(t_wVerCount);
		for (WORD n = 0; n < t_wVerCount; n++) {
			m_vecTexList[i][n] = pVertex[n].m_SUV;
		}
	}

	pCModel->Unlock();
}

void CTexList::GetCurTexture(S_BVECTOR<D3DXVECTOR2>& coord, WORD&  wCurIndex, float& fCurTime, float fDailTime) {
	if (m_wTexCount == 1) {
		for (WORD i = 0; i < (WORD)coord.size(); ++i) {
			*coord[i] = m_vecTexList[0][i];
		}
		return;
	}

	fCurTime += fDailTime;
	if (fCurTime > m_fFrameTime) {
		wCurIndex++;
		fCurTime = 0.0f;
	}

	if (wCurIndex >= m_wTexCount) {
		wCurIndex = 0;
	}

	for (WORD i = 0; i < coord.size(); ++i) {
		*coord[i] = m_vecTexList[wCurIndex][i];
	}
}

void CTexList::Remove() {
	if (m_wTexCount <= 1) {
		return;
	}
	m_vecTexList.pop_back();
	m_wTexCount--;
}

CTexFrame::CTexFrame() {
	m_wTexCount = 0;
	m_fFrameTime = 0.1f;

	m_vecTexName.clear();
	m_vecTexs.clear();

	m_lpCurTex = NULL;
}

CTexFrame::~CTexFrame() {
	m_vecTexName.clear();
	m_vecTexs.clear();
	m_vecCoord.clear();
}

void CTexFrame::GetCoordFromModel(CEffectModel *pCModel) {
	if (!pCModel->IsBoard()) {
		return;
	}

	WORD t_wVerCount = (WORD)pCModel->GetVerCount();
	m_vecCoord.clear();
	m_vecCoord.resize(t_wVerCount);

	SEFFECT_VERTEX *pVertex;
	pCModel->Lock((BYTE**)&pVertex);
	for (WORD n = 0; n < t_wVerCount; n++) {
		m_vecCoord[n] = pVertex[n].m_SUV;
	}
	pCModel->Unlock();
}

void CTexFrame::AddTexture(const s_string&  pszName) {
	m_vecTexName.push_back(pszName);
	m_wTexCount++;
	m_vecTexs.resize(m_wTexCount);
}

drITex*	CTexFrame::GetCurTexture(WORD&  wCurIndex, float& fCurTime, float fDailTime) {
	if (m_wTexCount == 0) {
		return m_lpCurTex = NULL;
	}

	if (m_wTexCount == 1) {
		return m_lpCurTex = m_vecTexs[0];
	}

	fCurTime += fDailTime;
	if (fCurTime > m_fFrameTime) {
		wCurIndex++;
		fCurTime = 0.0f;
	}

	if (wCurIndex >= m_wTexCount) {
		wCurIndex = 0;
	}

	return m_lpCurTex = m_vecTexs[wCurIndex];
}

void CTexFrame::Remove() {
	m_wTexCount = 0;
	m_fFrameTime = 0.1f;

	m_vecTexName.clear();
	m_vecTexs.clear();

	m_lpCurTex = NULL;
}

CEffectFont::CEffectFont() {
	_strText = "0123456789+-";
	_iTextNum = 12;
	m_vecTexName = "number";
	_strBackBmp = "backnumber";
	_lpBackTex = NULL;
	_bUseBack = FALSE;
	_iTextureID = -1;
}

CEffectFont::~CEffectFont() {
}

#ifdef USE_RENDER 
bool CEffectFont::CreateEffectFont(LERender*  pDev, CLEResManger*pCResMagr, int iTexID, D3DXCOLOR dwColor, bool bUseBack, bool bmain)
#else
bool CEffectFont::CreateEffectFont(LPDIRECT3DDEVICE8  pDev, CLEResManger*pCResMagr, int iTexID, D3DXCOLOR dwColor, bool bUseBack, bool bmain)
#endif
{
	m_pRes = pCResMagr->m_pSysGraphics->GetResourceMgr();

	m_pDev = pDev;
	_bUseBack = bUseBack;
	_dwColor = dwColor;
	_iTextureID = iTexID;
	s_string str[] = {
		"addblood",
		"subblood",
		"addsp",
		"subsp",
		"addblood",
		"subblood",
		 "bao",
		 "submiss",
	};

	char psName[64];
	if (!bmain) {
		_snprintf_s(psName, 64, _TRUNCATE, "%s", str[_iTextureID].c_str());
	}
	else {
		_snprintf_s(psName, 64, _TRUNCATE, "%s2", str[_iTextureID].c_str());
	}

	m_vecTexName = psName;

	int id = pCResMagr->GetTextureID(m_vecTexName);
	if (id < 0) {
		LG("error", "msgCEffectFont Texture %s not found", m_vecTexName.c_str());
		m_lpCurTex = NULL;
		m_pTex = NULL;
	}
	else {
		m_pTex = pCResMagr->GetTextureByIDdr(id);
		if (m_pTex) {
			m_lpCurTex = m_pTex->GetTex();
		}
		else {
			m_lpCurTex = NULL;
		}
	}

	id = pCResMagr->GetTextureID(_strBackBmp);
	if (id < 0) {
		LG("error", "msgCEffectFont Texture %s not found", _strBackBmp.c_str());
		_lpBackTex = NULL;
	}
	else {

#ifdef USE_RENDER
		_lpBackTex = pCResMagr->GetTextureByIDdr(id);
#else
		_lpBackTex = pCResMagr->GetTextureByID(id);
#endif

	}

	float fx = 0.5f;
	float fy = 0.7f;
	if (_iTextureID == 7) {
		_iTextNum = 1;
		_vecCurText.push_back(0);
		fx = 2.0f;
		fy = 1.0f;
	}
	else {
		_iTextNum = 12;
	}

	CreateSpliteTexture(_iTextNum, 1);

	_dwVerCount = _iTextNum * 2 * 3;
	_dwFaceCount = _iTextNum * 2;

	t_SEffVer[0].m_SPos = D3DXVECTOR3(-fx, -fy, 0);
	t_SEffVer[0].m_fIdx = 0;
	t_SEffVer[0].m_dwDiffuse = 0xffffffff;
	t_SEffVer[0].m_SUV = D3DXVECTOR2(0.0f, 1.0f);

	t_SEffVer[1].m_SPos = D3DXVECTOR3(-fx, fy, 0);
	t_SEffVer[1].m_fIdx = 1;
	t_SEffVer[1].m_dwDiffuse = 0xffffffff;
	t_SEffVer[1].m_SUV = D3DXVECTOR2(0.0f, 0);

	t_SEffVer[2].m_SPos = D3DXVECTOR3(fx, fy, 0);
	t_SEffVer[2].m_fIdx = 2;
	t_SEffVer[2].m_dwDiffuse = 0xffffffff;
	t_SEffVer[2].m_SUV = D3DXVECTOR2(1.0f, 0.0f);

	t_SEffVer[3].m_SPos = D3DXVECTOR3(fx, -fy, 0);
	t_SEffVer[3].m_fIdx = 3;
	t_SEffVer[3].m_dwDiffuse = 0xffffffff;
	t_SEffVer[3].m_SUV = D3DXVECTOR2(1.0f, 1.0f);

	m_vEffVer = new SEFFECT_VERTEX[_dwVerCount];
	for (int m = 0; m < _iTextNum; m++) {
		for (int n = 0; n < 4; n++) {
			m_vEffVer[m * 4 + n].m_SPos.x = t_SEffVer[n].m_SPos.x + m;
			m_vEffVer[m * 4 + n].m_SPos.y = t_SEffVer[n].m_SPos.y;
			m_vEffVer[m * 4 + n].m_SPos.z = 0;
			m_vEffVer[m * 4 + n].m_fIdx = float(n);
			m_vEffVer[m * 4 + n].m_dwDiffuse = 0xffffffff;
			m_vEffVer[m * 4 + n].m_SUV = m_vecTexList[m][n];
		}
	}

	m_strName = "FONTEFFECT";
	return true;
}

void CEffectFont::SetRenderText(char* pszText) {
	if (_iTextureID == 7) {
		return;
	}

	int pos;
	char pszt[3];
	int len = lstrlen(pszText);
	_vecCurText.clear();

	for (int m = 0; m < len; m++) {
		if (pszText[m] & 0x80) {
			pszt[0] = pszText[m];
			pszt[1] = pszText[m + 1];
			pszt[2] = '\0';
		}
		else {
			pszt[0] = pszText[m];
			pszt[1] = '\0';
			pszt[2] = '\0';
		}

		pos = (int)_strText.find(pszt);
		if (pos != -1) {
			_vecCurText.push_back(pos);
			for (int n = 0; n < 4; n++) {
				m_vEffVer[m * 4 + n].m_SUV = m_vecTexList[pos][n];
			}
		}
	}

	t_SEffVer[0].m_SPos.x -= 4.0f;
	t_SEffVer[0].m_SPos.y -= 2.0f;
	t_SEffVer[1].m_SPos.x -= 4.0f;
	t_SEffVer[1].m_SPos.y += 2.0f;
	t_SEffVer[2].m_SPos.x += len + 2.5f;
	t_SEffVer[2].m_SPos.y += 2.0f;
	t_SEffVer[3].m_SPos.x += len + 2.5f;
	t_SEffVer[3].m_SPos.y -= 2.0f;
}

void CEffectFont::RenderEffectFontBack(D3DXMATRIX* pmat) {
	m_pDev->SetVertexShader(EFFECT_VER_FVF);

#ifdef USE_RENDER
	if (_lpBackTex && _lpBackTex->IsLoadingOK()) {
		m_pDev->SetTexture(0, _lpBackTex->GetTex());
	}
	else {
		return;
	}
#else
	m_pDev->SetTexture(0, _lpBackTex);
#endif

#ifdef USE_RENDER
	m_pDev->SetTransformWorld(pmat);
	if (FAILED(m_pDev->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &t_SEffVer, sizeof(SEFFECT_VERTEX)))) {
		LG("error", "msgCEffectFont draw");
	}
#else
	m_pDev->SetTransform(D3DTS_WORLDMATRIX(0), pmat);
	m_pDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &t_SEffVer, sizeof(SEFFECT_VERTEX));
#endif
}

void CEffectFont::RenderEffectFont(D3DXMATRIX* pmat) {
	if (_bUseBack) {
		RenderEffectFontBack(pmat);
	}

#ifdef USE_RENDER
	m_pDev->SetTransformWorld(pmat);
#else
	m_pDev->SetTransform(D3DTS_WORLDMATRIX(0), pmat);
#endif

	if (m_pTex && m_pTex->IsLoadingOK()) {
		m_pDev->SetTexture(0, m_pTex->GetTex());
	}
	else {
		return;
	}

	for (int n = 0; n < (WORD)_vecCurText.size(); n++) {

#ifdef USE_RENDER
		if (FAILED(m_pDev->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &m_vEffVer[n * 4], sizeof(SEFFECT_VERTEX)))) {
			LG("error", "msgCEffectFont draw2");
		}
#else
		m_pDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &m_vEffVer[n * 4], sizeof(SEFFECT_VERTEX));
#endif

	}
}

CScriptFile CScriptFile::m_ctScript;
CScriptFile::CScriptFile() {
	
	// File pointer
	m_fp = NULL;
	
	// Whether it is read mode
	m_bRead = true;
	m_bOpen = false;
	m_SCharacterAction = NULL;
	InitAction(_TEXT("scripts/txt/CharacterAction.tx"));
}

CScriptFile::~CScriptFile() {
	if (m_fp != NULL) {
		CloseFile();
	}
	Free();
}

bool CScriptFile::OpenFileRead(const char *filename) {
	fopen_s(&m_fp, filename, "rt");
	if (m_fp == NULL) {
		IP("CIniFile::OpenFileRead() Error (0) : Could not open file for read : <%s>\n", filename);
		return false;
	}
	m_bRead = true;
	m_bReadMem = false;
	m_bOpen = true;
	return true;
}

// Close the file
void CScriptFile::CloseFile() {
	if (m_bOpen) {
		if (!this->m_bReadMem) {
			fclose(m_fp);
			m_fp = NULL;
			m_bRead = true;
		}
		m_bOpen = false;
	}
}

bool CScriptFile::ReadSection(char *section) {
	char rval;
	int nCount;
	bool bFound;
	bool bBegin;
	char Section[_MAX_STRING];
	if (!m_bReadMem) {
		if (m_fp == NULL) {
			return false;
		}
	}

	if (this->m_bReadMem) {
		this->m_iP = 0;
	}
	else {
		if (fseek(m_fp, 0, SEEK_SET)) {
			return false;
		}
	}

	bFound = false;
	bBegin = false;
	nCount = 0;
	if (m_bReadMem) {
		while (bFound == false) {
			if (m_iP == this->m_iSize) {
				return false;
			}

			rval = this->m_pData[m_iP];
			m_iP++;

			if (rval == '[') {
				bBegin = true;
				continue;
			}

			if (rval == ']') {
				Section[nCount] = 0;
				nCount = 0;
				bBegin = false;
				if (_stricmp(section, "") == 0) {
					strncpy_s(section, strlen(Section) + 1, Section, _TRUNCATE);
					bFound = true;
				}
				else {
					if (strcmp(Section, section) == 0) {
						bFound = true;
					}
				}
				continue;
			}

			if (bBegin == true && nCount < _MAX_STRING) {
				Section[nCount] = rval;
				nCount++;
			}
		}

		return true;
	}

	while (bFound == false) {
		if ((rval = (char)fgetc(m_fp)) == EOF) {
			IP("CIniFile::ReadSection() Error(0) : Reach the end of the file in reading section <%s>\n", section);
			return false;
		}

		if (rval == '[') {
			bBegin = true;
			continue;
		}

		if (rval == ']') {
			Section[nCount] = 0;
			nCount = 0;
			bBegin = false;
			if (_stricmp(section, "") == 0) {
				strncpy_s(section, strlen(Section) + 1, Section, _TRUNCATE);
				bFound = true;
			}
			else {
				if (_stricmp(Section, section) == 0) {
					bFound = true;
				}
			}
			continue;
		}

		if (bBegin == true && nCount < _MAX_STRING) {
			Section[nCount] = rval;
			nCount++;
		}
	}

	return true;
}

bool CScriptFile::ReadLine(const char *name, char *value, int nSize) {
	int rval;
	int nCount;
	bool bFound;
	char Name[_MAX_STRING];

	if (!this->m_bReadMem) {

		// TO REMOVE
	}

	nCount = 0;
	bFound = false;
	bool bno = false;

	if (this->m_bReadMem) {
		while (bFound == false) {
			if (m_iP == this->m_iSize) {
				IP("Reach the end of file ");
				return false;
			}

			rval = this->m_pData[m_iP];
			m_iP++;

			if (rval == '=') {
				Name[nCount] = 0;
				nCount = 0;
				if (strcmp(Name, name) == 0) {
					bFound = true;
				}
				else {
					nCount = 0;
					bno = true;
				}
				continue;
			}

			if (rval == 13) {
				continue;
			}

			if (rval == '\n') {
				if (bno == true) {
					nCount = 0;
				}
				continue;
			}

			if (nCount < _MAX_STRING) {
				Name[nCount] = (char)rval;
				nCount++;
			}
		}

		nCount = 0;
		bFound = false;
		while (bFound == false) {
			rval = this->m_pData[m_iP];
			m_iP++;
			if (rval == EOF || rval == '\n' || rval == 13) {
				value[nCount] = 0;
				bFound = true;
				continue;
			}

			if (nCount < nSize - 1) {
				value[nCount] = (char)rval;
				nCount++;
			}
		}
		return true;
	}
	
	while (bFound == false) {
		if ((rval = fgetc(m_fp)) == EOF) {
			IP("CIniFile::ReadLine() Error(0) : Reach the end of the file in reading named <%s>\n", name);
			return false;
		}

		if (rval == '=') {
			Name[nCount] = 0;
			nCount = 0;
			if (_stricmp(Name, name) == 0) {
				bFound = true;
			}
			else {
				nCount = 0;
				bno = true;
			}
			continue;
		}

		if (rval == '\n') {
			if (bno == true) {
				nCount = 0;
			}
			continue;
		}

		if (nCount < _MAX_STRING) {
			Name[nCount] = (char)rval;
			nCount++;
		}
	}

	nCount = 0;
	bFound = false;
	while (bFound == false) {
		rval = fgetc(m_fp);
		if (rval == EOF || rval == '\n') {
			value[nCount] = 0;
			bFound = true;
			continue;
		}

		if (nCount < nSize - 1) {
			value[nCount] = (char)rval;
			nCount++;
		}
	}

	return true;
}

bool CScriptFile::ReadLine(const char *name, char* value) {
	char Value[_MAX_STRING];
	if (ReadLine(name, Value, _MAX_STRING) == false) {
		return false;
	}
	*value = Value[0];
	return true;
}


bool CScriptFile::ReadLine(const char *name, int *value) {
	char Value[_MAX_STRING];
	if (ReadLine(name, Value, _MAX_STRING) == false) {
		return false;
	}
	*value = atoi(Value);
	return true;
}

bool CScriptFile::ReadLine(const char *name, float *value) {
	char Value[_MAX_STRING];
	if (ReadLine(name, Value, _MAX_STRING) == false) {
		return false;
	}
	*value = (float)atof(Value);
	return true;
}

bool CScriptFile::ReadLine(const char *name, double *value) {
	char Value[_MAX_STRING];
	if (ReadLine(name, Value, _MAX_STRING) == false) {
		return false;
	}
	*value = atof(Value);
	return true;
}

bool CScriptFile::InitAction(const char *ptcsFileName) {
	bool bRet = true;
	_TCHAR tcsLine[1024], tcsTemp[1024];
	short iCurType, iCurActNO, iCurKeyFrame;
	long lIndex, lOldIndex, lFilePos;
	FILE *fFile = NULL;

	m_iMaxCharacterType = 0;
	m_iActualCharacterType = 0;
	m_SCharacterAction = NULL;

	_tfopen_s(&fFile, ptcsFileName, _TEXT("rb"));
	if (fFile == NULL) {
		LG("error", "msgLoad Txt File [%s] Fail!\n", ptcsFileName);
		bRet = false;
		goto end;
	}

	// Get the maximum value of the action type (m_iMaxCharacterType),
	// and the "effective number of the action type" (m_iActualCharacterType);

	while (!feof(fFile)) {
		lIndex = 0;
		_fgetts(tcsLine, 1023, fFile);
		StringSkipCompartmentT(tcsLine, &lIndex, " ", 1);

		// Carriage return, line feed
		if (tcsLine[lIndex] == 0x0a || tcsLine[lIndex] == 0x0d) {
			continue;
		}

		// Annotations
		if (tcsLine[lIndex] == _TEXT('/') && tcsLine[lIndex + 1] == _TEXT('/')) {
			continue;
		}

		// tab (the start symbol for the "action of an action type" line in this text)
		if (tcsLine[lIndex] == _TEXT('\t')) {
			continue;
		}

		m_iActualCharacterType++;
		StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5);
		if (m_iMaxCharacterType < _ttoi(tcsTemp)) {
			m_iMaxCharacterType = _ttoi(tcsTemp);
		}
	}

	if (m_iActualCharacterType < 1) {
		bRet = true;
		goto end;
	}

	m_SCharacterAction = new (SChaAction[m_iMaxCharacterType]);
	if (m_SCharacterAction == NULL) {
		m_iMaxCharacterType = 0;
		bRet = false;
		goto end;
	}
	memset((void *)m_SCharacterAction, 0, sizeof(SChaAction) * m_iMaxCharacterType);

	// Get single role information
	fseek(fFile, 0, SEEK_SET);
	while (!feof(fFile)) {
		lIndex = 0;
		_fgetts(tcsLine, 1023, fFile);
		StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" "), 1);

		// Carriage return, line feed
		if (tcsLine[lIndex] == 0x0a || tcsLine[lIndex] == 0x0d) {
			continue;
		}

		// Annotations
		if (tcsLine[lIndex] == _TEXT('/') && tcsLine[lIndex + 1] == _TEXT('/')) {
			continue;
		}

		// tab key (the start symbol of an action line used in this text for "role type")
		if (tcsLine[lIndex] == _TEXT('\t')) {
			continue;
		}

		StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5);
		iCurType = _ttoi(tcsTemp) - 1;
		if (iCurType < 0) {
 			continue;
		}

		// The number of actions of the character
		m_SCharacterAction[iCurType].m_SActionInfo = NULL;
		m_SCharacterAction[iCurType].m_iActualActionNum = 0;
		m_SCharacterAction[iCurType].m_iCharacterType = iCurType + 1;
		m_SCharacterAction[iCurType].m_iMaxActionNum = 0;
		lFilePos = ftell(fFile);
		while (!feof(fFile)) {
			lIndex = 0;
			_fgetts(tcsLine, 1023, fFile);
			StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" "), 1);

			// Carriage return, line feed
			if (tcsLine[lIndex] == 0x0a || tcsLine[lIndex] == 0x0d) {
				continue;
			}

			// Annotations
			if (tcsLine[lIndex] == _TEXT('/') && tcsLine[lIndex + 1] == _TEXT('/')) {
				continue;
			}

			// tab key
			if (tcsLine[lIndex] == _TEXT('\t')) {
				m_SCharacterAction[iCurType].m_iActualActionNum++;
				StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" ,\x09"), 3);
				StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5);

				// Temporary code, used to prompt to update the file version.
				if (_ttoi(tcsTemp) < 1) {
					MessageBox(NULL, _TEXT("please update .\\scripts\\CharacterAction.tx"), _TEXT("´íÎó"), 0);
					goto end;
				}

				if (m_SCharacterAction[iCurType].m_iMaxActionNum < _ttoi(tcsTemp)) {
					m_SCharacterAction[iCurType].m_iMaxActionNum = _ttoi(tcsTemp);
				}
			}
			else {
				break;
			}
		}

		if (m_SCharacterAction[iCurType].m_iActualActionNum < 1) {
			continue;
		}

		m_SCharacterAction[iCurType].m_SActionInfo = new (ActionInfo[m_SCharacterAction[iCurType].m_iMaxActionNum]);
		if (m_SCharacterAction[iCurType].m_SActionInfo == NULL) {
			m_SCharacterAction[iCurType].m_iMaxActionNum = 0;
			bRet = false;
			goto end;
		}
		memset((void *)(m_SCharacterAction[iCurType].m_SActionInfo), 0, sizeof(ActionInfo) * m_SCharacterAction[iCurType].m_iMaxActionNum);

		// Single action information
		fseek(fFile, lFilePos, SEEK_SET);
		while (!feof(fFile)) {
			lFilePos = ftell(fFile);
			lIndex = 0;
			_fgetts(tcsLine, 1023, fFile);
			StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" "), 1);

			// Carriage return, line feed
			if (tcsLine[lIndex] == 0x0a || tcsLine[lIndex] == 0x0d) {
				continue;
			}

			// Annotations
			if (tcsLine[lIndex] == _TEXT('/') && tcsLine[lIndex + 1] == _TEXT('/')) {
				continue;
			}

			// tab key
			if (tcsLine[lIndex] == _TEXT('\t')) {
				StringSkipCompartmentT(tcsLine, &lIndex, _TEXT("\x09"), 1);
				StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5);

				iCurActNO = _ttoi(tcsTemp);
				if (iCurActNO < 1) {
					continue;
				}

				m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sActionNO = iCurActNO;

				StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" ,\x09"), 3);
				StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5);
				m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sStartFrame = _ttoi(tcsTemp);

				StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" ,\x09"), 3);
				StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5);
				m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sEndFrame = _ttoi(tcsTemp);

				StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" ,\x09"), 3);
				lOldIndex = lIndex;
				
				// Number of keyframes
				iCurKeyFrame = 0;
				while (StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5)) {
					iCurKeyFrame++;
					StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" ,\x09"), 3);
				}

				if (iCurKeyFrame < 1) {
					continue;
				}

				m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sKeyFrameNum = iCurKeyFrame;
				m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sKeyFrame = new (short[m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sKeyFrameNum]);
				if (m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sKeyFrame == NULL) {
					m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sKeyFrameNum = 0;
					bRet = false;
					goto end;
				}

				lIndex = lOldIndex;

				// Keyframe
				iCurKeyFrame = 0;
				while (StringGetT(tcsTemp, 1023, tcsLine, &lIndex, _TEXT(" ,\x09\x0a\x0d"), 5)) {
					m_SCharacterAction[iCurType].m_SActionInfo[iCurActNO - 1].m_sKeyFrame[iCurKeyFrame] = _ttoi(tcsTemp);
					iCurKeyFrame++;
					StringSkipCompartmentT(tcsLine, &lIndex, _TEXT(" ,\x09"), 3);
				}
			}
			else {
				fseek(fFile, lFilePos, SEEK_SET);
				break;
			}
		}
	}
end:
	if (fFile) {
		fclose(fFile);
	}
	
	if (!bRet) {
		Free();
	}

	return bRet;
}

void CScriptFile::Free(void) {
	short i, j;
	if (m_SCharacterAction) {
		for (i = 0; i < m_iMaxCharacterType; i++) {
			if (m_SCharacterAction[i].m_SActionInfo) {
				for (j = 0; j < m_SCharacterAction[i].m_iMaxActionNum; j++) {
					if (m_SCharacterAction[i].m_SActionInfo[j].m_sKeyFrame) {
						delete[] m_SCharacterAction[i].m_SActionInfo[j].m_sKeyFrame;
						m_SCharacterAction[i].m_SActionInfo[j].m_sKeyFrame = NULL;
					}
				}
				delete[] m_SCharacterAction[i].m_SActionInfo;
				m_SCharacterAction[i].m_SActionInfo = NULL;
			}
		}
		delete[] m_SCharacterAction;
		m_SCharacterAction = NULL;
	}
}

long StringGetT(_TCHAR *out, long out_max, _TCHAR *in, long *in_from, _TCHAR *end_list, long end_len) {
	long offset = -1;		// set offset of get string to -1 for the first do process
	long i;					// temp variable

	--(*in_from);			// dec (*in_from) for the first do process
	do {
		out[++offset] = in[++(*in_from)];
		for (i = end_len - 1; i >= 0; --i) {
			if (out[offset] == end_list[i]) {
				out[offset] = 0x00;
				break;
			}
		}
	} while (out[offset] && offset < out_max);

	return offset;
}

void StringSkipCompartmentT(_TCHAR *in, long *in_from, _TCHAR *skip_list, long skip_len) {
	long i;		// Temp variable
	while (in[(*in_from)]) {
		for (i = skip_len - 1; i >= 0; --i) {
			if (in[(*in_from)] == skip_list[i]) {
				break;
			}
		}
		
		// Dismatch skip conditions, finished
		if (i < 0) {
			break; 
		}
		
		// match skip conditions, skip it
		else {
			++(*in_from); 
		}
	}
}

bool CScriptFile::GetCharAction(int iCharType, SChaAction *SCharAct) {
	if (m_SCharacterAction == NULL || SCharAct == NULL) {
		return false;
	}

	if (iCharType < 1 || iCharType > m_iMaxCharacterType) {
		return false;
	}

	if (m_SCharacterAction[iCharType - 1].m_iCharacterType < 1) {
		return false;
	}

	memcpy((void *)SCharAct, (void *)(m_SCharacterAction + iCharType - 1), sizeof(SChaAction));

	return true;
}

s_string& I_Effect::getEffectModelName() {
	return m_strModelName;
}
