#pragma once

class CLEEffectCtrl;

#pragma warning(disable: 4275)

#define SCRIPT_TABLE

#ifndef SCRIPT_TABLE
struct LIGHTENGINE_API EFF_Param {
	int  m_iModelNum;
	std::vector<s_string> m_vecModelName;
	int	m_iPartNum;
	float m_fvel;
	std::vector<s_string> m_vecParName;
	std::vector<int> m_vecDummyIdx;
	int m_iRenderIdx;
	int m_iLightID;
	s_string m_strResult;
};

struct LIGHTENGINE_API Group_Param {
	int nClass;
	std::vector<int> vecParam;
	std::vector<int> vecNum;
	int nRenderIdx;
};

class LIGHTENGINE_API CEffectScript {
public:
	CEffectScript();
	~CEffectScript();

	bool InitScript();
	void ReleaseScript();

	bool LoadParamFromName(EFF_Param* pParam, const s_string& strName);
	bool LoadParamFromName(Group_Param* pParam, const s_string& strName);

	CScriptFile m_cScript;
	static CEffectScript m_cEffScript;
};
#else
#include "TableData.h"
class LIGHTENGINE_API EFF_Param : public CRawDataInfo {
public:
	EFF_Param() {
		strncpy_s(szName, "Nameless effects", _TRUNCATE);
		nModelNum = 0;
		memset(strModel, 0, sizeof(char) * 8 * 24);
		nVel = 0;
		nParNum = 0;
		memset(strPart, 0, sizeof(char) * 8 * 24);
		memset(&nDummy, -1, sizeof(int) * 8);
		nRenderIdx = -1;
		nLightID = -1;
		strResult[0] = 0;
	}

	char szName[32];
	int nModelNum;
	char strModel[8][24];
	int nVel;

	int nParNum;
	char strPart[8][24];

	int nDummy[8];

	int nRenderIdx;

	int nLightID;

	char strResult[24];
};

// 2016 modified
class LIGHTENGINE_API CEff_ParamSet : public CRawDataSet {
public:
	static CEff_ParamSet* I() { return _Instance; }

	CEff_ParamSet(int nIDStart, int nIDCnt) : CRawDataSet(nIDStart, nIDCnt) {
		_Instance = this;
		_Init();
	}

protected:

	// Equivalent to a single key, remember yourself
	static CEff_ParamSet* _Instance;

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt) {
		return new EFF_Param[nCnt];
	}

	virtual void _DeleteRawDataArray() {
		delete[](EFF_Param*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize() {
		return sizeof(EFF_Param);
	}

	virtual void* _CreateNewRawData(CRawDataInfo *pInfo) {
		return NULL;
	}

	virtual void _DeleteRawData(CRawDataInfo *pInfo) {
		SAFE_DELETE(pInfo->pData);
	}

	virtual BOOL _ReadRawDataInfo(CRawDataInfo *pRawDataInfo, vector<string> &ParamList) {
		int idx = 0;
		EFF_Param *pInfo = (EFF_Param*)pRawDataInfo;

		strncpy_s(pInfo->szName, pRawDataInfo->szDataName, _TRUNCATE);

		pInfo->nModelNum = Str2Int(ParamList[idx++].c_str());
		string lstr1[8];
		int n;
		int dummynum = 0;
		pInfo->nModelNum = Util_ResolveTextLine(ParamList[idx++].c_str(), lstr1, 8, ',');
		for (n = 0; n < pInfo->nModelNum; n++) {
			strncpy_s(pInfo->strModel[n], lstr1[n].c_str(), _TRUNCATE);
		}

		pInfo->nVel = Str2Int(ParamList[idx++].c_str());
		pInfo->nParNum = Str2Int(ParamList[idx++].c_str());
		if (pInfo->nParNum > 0) {
			pInfo->nParNum = Util_ResolveTextLine(ParamList[idx++].c_str(), lstr1, 8, ',');
			if (pInfo->nParNum > 0) {
				for (n = 0; n < pInfo->nParNum; n++) {
					strncpy_s(pInfo->strPart[n], lstr1[n].c_str(), _TRUNCATE);
				}
			}
		}
		else {
			idx++;
		}

		if (pInfo->nParNum > 0) {
			dummynum = Util_ResolveTextLine(ParamList[idx++].c_str(), lstr1, 8, ',');
			if (dummynum > 0) {
				for (n = 0; n < dummynum; n++) {
					pInfo->nDummy[n] = Str2Int(lstr1[n].c_str());
				}
			}
		}
		else {
			idx++;
		}

		pInfo->nRenderIdx = Str2Int(ParamList[idx++].c_str());
		pInfo->nLightID = Str2Int(ParamList[idx++].c_str());
		strncpy_s(pInfo->strResult, ParamList[idx++].c_str(), _TRUNCATE);

		return TRUE;
	}

	virtual void _ProcessRawDataInfo(CRawDataInfo *pRawDataInfo) {
	}
};

class LIGHTENGINE_API Group_Param : public CRawDataInfo {
public:
	Group_Param() {
		strncpy_s(szName, "Nameless effects", _TRUNCATE);
		nTypeNum = 0;
		memset(&nTypeID, -1, sizeof(int) * 8);
		memset(&nNum, 0, sizeof(int) * 8);
		nTotalNum = 0;
		nRenderIdx = -1;
	}

	char szName[32];
	int nTypeNum;

	int nTypeID[8];
	int nNum[8];
	int nTotalNum;

	int nRenderIdx;
};

class LIGHTENGINE_API CGroup_ParamSet : public CRawDataSet {
public:
	static CGroup_ParamSet* I() { return _Instance; }
	CGroup_ParamSet(int nIDStart, int nIDCnt) : CRawDataSet(nIDStart, nIDCnt) {
		_Instance = this;
		_Init();
	}

protected:

	// Equivalent to a single key, remember yourself
	static CGroup_ParamSet* _Instance;

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt) {
		return new Group_Param[nCnt];
	}

	virtual void _DeleteRawDataArray() {
		delete[](Group_Param*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize() {
		return sizeof(Group_Param);
	}

	virtual void* _CreateNewRawData(CRawDataInfo *pInfo) {
		return NULL;
	}

	virtual void _DeleteRawData(CRawDataInfo *pInfo) {
		SAFE_DELETE(pInfo->pData);
	}

	virtual BOOL _ReadRawDataInfo(CRawDataInfo *pRawDataInfo, vector<string> &ParamList) {
		int idx = 0;
		Group_Param *pInfo = (Group_Param*)pRawDataInfo;

		strncpy_s(pInfo->szName, pRawDataInfo->szDataName, _TRUNCATE);

		pInfo->nTypeNum = Str2Int(ParamList[idx++].c_str());
		string lstr1[8];
		int n;
		int m;
		if (pInfo->nTypeNum > 0) {
			pInfo->nTypeNum = Util_ResolveTextLine(ParamList[idx++].c_str(), lstr1, 8, ',');
			for (n = 0; n < pInfo->nTypeNum; n++) {
				pInfo->nTypeID[n] = Str2Int(lstr1[n].c_str());
			}
		}
		else {
			idx++;
		}

		if (pInfo->nTypeNum > 0) {
			m = Util_ResolveTextLine(ParamList[idx++].c_str(), lstr1, 8, ',');
			for (n = 0; n < m; n++) {
				pInfo->nNum[n] = Str2Int(lstr1[n].c_str());
			}
		}
		else {
			idx++;
		}

		pInfo->nRenderIdx = Str2Int(ParamList[idx++].c_str());
		return TRUE;
	}

	virtual void _ProcessRawDataInfo(CRawDataInfo *pRawDataInfo) {
		Group_Param *pInfo = (Group_Param*)pRawDataInfo;
		int n;
		pInfo->nTotalNum = 0;
		for (n = 0; n < 8; ++n) {
			pInfo->nTotalNum += pInfo->nNum[n];
		}
	}
};

inline EFF_Param* GetEFFParam(int nTypeID) {
	return (EFF_Param*)CEff_ParamSet::I()->GetRawDataInfo(nTypeID);
}

inline Group_Param* GetGroupParam(int nTypeID) {
	return (Group_Param*)CGroup_ParamSet::I()->GetRawDataInfo(nTypeID);
}
#endif

class LIGHTENGINE_API CLEEffectCtrl {
public:
	CLEEffectCtrl(void);
	~CLEEffectCtrl(void);

	void Emission(WORD wID, D3DXVECTOR3* vBegin, D3DXVECTOR3* vEnd);
	void SetTarget(D3DXVECTOR3* vTarget);
	void FrameMove(DWORD dwDailTime);
	void Render();
	void MoveTo(D3DXVECTOR3* vPos, LEMap* pmap = NULL);
	void BindingBone(D3DXMATRIX* pMatBone);
	void SetFontEffect(char* pszText, CLEFont* pFont);
	
	void Reset() { _CPartCtrl.Reset(); }
	void Clear() { _CPartCtrl.Clear(); }
	void CopyPartCtrl(CLEPartCtrl* pPart) { _CPartCtrl.CopyPartCtrl(pPart); }
	void BindingRes(CLEResManger* pResMagr) { _CPartCtrl.BindingRes(pResMagr); }

	void SetItemDummy(LESceneItem* pItem, int dummy1, int dummy2) {
		_CPartCtrl.SetItemDummy(pItem, dummy1, dummy2);
	}

	void SetSkillCtrl(SkillCtrl* pCtrl) {
		_CPartCtrl.SetSkillCtrl(pCtrl);
	}

	void setUseZBuff(bool bUseZ) {
		_CPartCtrl.setUseZBuff(bUseZ);
	}

	void setDir(D3DXVECTOR3* pDir) {
		_CPartCtrl.setDir(pDir);
	}

	CLEPartCtrl* GetPartCtrl() { return &_CPartCtrl; }
	CLEPartCtrl _CPartCtrl;
};

class CMagicCtrl;
inline void Part_trace(CMagicCtrl* pEffCtrl, void* pParam);
inline void Part_drop(CMagicCtrl* pEffCtrl, void* pParam);
inline void Part_fly(CMagicCtrl* pEffCtrl, void* pParam);
inline void Part_fshade(CMagicCtrl* pEffCtrl, void* pParam);
inline void Part_arc(CMagicCtrl* pEffCtrl, void* pParam);
inline void Part_dirlight(CMagicCtrl* pEffCtrl, void* pParam);
inline void Part_dist(CMagicCtrl* pEffCtrl, void* pParam);
inline void Part_dist2(CMagicCtrl* pEffCtrl, void* pParam);

class LIGHTENGINE_API CMagicCtrl {
public:
	CMagicCtrl(void);
	~CMagicCtrl(void);
	bool Create(int iID, CLEResManger	*pCResMagr);
	void MoveTo(D3DXVECTOR3* vPos);
	void FrameMove(DWORD	dwDailTime);
	void Render();

	void SetSkillCtrl(SkillCtrl* pCtrl) {
		for (int n = 0; n < _iModelNum; ++n) {
			_CpModel[n]->Scaling(pCtrl->fSize, pCtrl->fSize, pCtrl->fSize);
		}

		for (int n = 0; n < _iPartNum; ++n) {
			_vecPartCtrl[n].SetSkillCtrl(pCtrl);
		}
	}

	void SetTargetID(int iID) { _iTargetID = iID; }
	int GetTargetID() { return _iTargetID; }

	void Emission(D3DXVECTOR3* vStart, D3DXVECTOR3* vTarget);
	void CalculateEmission(D3DXVECTOR3* vStart, D3DXVECTOR3* vTarget);
	void Stop() {
		if (_pPartResult) {
			_bStop = true;
			_pPartResult->MoveTo(&_vPos, NULL);
			_pPartResult->setDir(&_vDir);
			_pPartResult->Play(1);
		}
		else {
			_bPlay = false;
		}
	}

	bool IsPlaying() { return _bPlay; }
	bool IsStop() { return _bStop; }

	void SetInValid() { _bPlay = false; }

	D3DXVECTOR3* GetPos() { return &_vPos; }

	void Reset();

	// Hit the object after the inheritance
	virtual void HitObj() {}

	int GetLightID();

	void ResetDir(D3DXVECTOR3* vTarget);

	void SetVel(float fvel) { _fVel = fvel; }

	void SetTargetDist(float fDist) {
		if (_iRnederIdx == 6 || _iRnederIdx == 7) {
			_fStartDist = fDist;
		}
	}

	void(*MagicUpdate)(CMagicCtrl* pMagicCtrl, void* pParam);
	friend void Part_trace(CMagicCtrl* pEffCtrl, void* pParam);
	friend void Part_drop(CMagicCtrl* pEffCtrl, void* pParam);
	friend void Part_fly(CMagicCtrl* pEffCtrl, void* pParam);
	friend void Part_fshade(CMagicCtrl* pEffCtrl, void* pParam);
	friend void Part_arc(CMagicCtrl* pEffCtrl, void* pParam);
	friend void Part_dirlight(CMagicCtrl* pEffCtrl, void* pParam);
	friend void Part_dist(CMagicCtrl* pEffCtrl, void* pParam);
	friend void Part_dist2(CMagicCtrl* pEffCtrl, void* pParam);

	int GetRenderIdx() { return _iRnederIdx; }
	int GetModelNum() { return _iModelNum; }

	CLEModelEff* GetModelEff(int id) { return _CpModel[id]; }

	int GetPartNum() { return _iPartNum; }

	CLEPartCtrl* GetPartCtrl(int id) { return &_vecPartCtrl[id]; }

	bool IsDail() { return (_fDailTime > 0 && _fCurEmiTime < _fDailTime); }

	void SetDailTime(float fTime) { _fDailTime = fTime; }

	void SetAlpha(float falpha) {
		int n;
		for (n = 0; n < _iModelNum; n++) {
			_CpModel[n]->SetAlpha(falpha);
		}

		for (n = 0; n < _iPartNum; n++) {
			_vecPartCtrl[n].SetAlpha(falpha);
		}
	}

	int m_iIdx;
	CLEResManger* m_pCResMagr;

protected:
	int _iModelNum;
	std::vector<CLEModelEff*> _CpModel;
	int _iPartNum;
	std::vector<CLEPartCtrl> _vecPartCtrl;
	std::vector<int> _vecDummy;

	CLEPartCtrl* _pPartResult;

	bool _bPlay;
	bool _bStop;

	float _fDailTime;
	float _fCurEmiTime;

	int _iRnederIdx;
	int _iLightID;

	s_string _strResult;

	int _iTargetID;

	D3DXVECTOR3 _vPos;
	D3DXVECTOR3 _vDir;
	float _fVel;
	D3DXVECTOR3 _vTarget;
	float _fDirXZ[2];

	float _fStartDist;
	float _fCurDist;

	float _fTargDist;
	D3DXVECTOR3 _vTargDir;
	D3DXVECTOR3 _vOldPos;
	D3DXVECTOR3 _vOldTarget;

	float _fDist;

	float _fHalfHei;
	D3DXVECTOR3 _vArcOrg;
	D3DXVECTOR3 _vArcAxis;
	float _fCurArc;

	int _iCurSNum;
	float _fCurTime;
};

#pragma warning(default: 4275)
