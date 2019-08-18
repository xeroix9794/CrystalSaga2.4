#include "StdAfx.h"
#include "GlobalInc.h"
#include "LEModelEff.h"
#include "LERender.h"
#include ".\LEparticlectrl.h"

CLEPartCtrl::CLEPartCtrl(void) {
	m_strName = "nameless";
	m_iPartNum = 0;

	// If the length is 0, it will loop
	m_fLength = 0;
	m_fCurTime = 0;

	m_iStripNum = 0;
	m_pcStrip = NULL;

	m_iModelNum = 0;
}
CLEPartCtrl::~CLEPartCtrl(void) {
	SAFE_DELETE_ARRAY(m_pcStrip);
	for (int n = 0; n < m_iModelNum; n++) {
		SAFE_DELETE(m_vecModel[n]);
	}
	m_vecModel.clear();
}

void CLEPartCtrl::SetStripCharacter(LECharacter* pCha) {
	for (int n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].AttachCharacter(pCha);
	}
}

void CLEPartCtrl::SetStripItem(LESceneItem* pItem, bool bloop) {
	for (int n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].AttachItem(pItem);
		m_pcStrip[n].SetLoop(bloop);
	}
}

void CLEPartCtrl::SetItemDummy(LESceneItem* pItem, int dummy1, int dummy2) {
	for (int n = 0; n < m_iPartNum; ++n) {
		m_vecPartSys[n]->SetItemDummy(pItem, dummy1, dummy2);
	}
}

CChaModel* CLEPartCtrl::NewModel(const s_string& strID, CLEResManger* pResMagr) {
	m_iModelNum++;
	if (m_iModelNum > 1) {
		m_iModelNum = 1;
		return NULL;
	}

	m_vecModel.resize(m_iModelNum);

	m_vecModel[m_iModelNum - 1] = new CChaModel;
	m_vecModel[m_iModelNum - 1]->m_pDev = pResMagr->m_pDev;

	m_vecModel[m_iModelNum - 1]->LoadScript(strID);
	m_vecModel[m_iModelNum - 1]->SetVel(120);
	m_vecModel[m_iModelNum - 1]->SetPlayType(PLAY_LOOP);
	m_vecModel[m_iModelNum - 1]->SetCurPose(1);
	m_vecModel[m_iModelNum - 1]->PlayPose(0, PLAY_PAUSE);

	return m_vecModel[m_iModelNum - 1];
}

CChaModel* CLEPartCtrl::GetModel(int iIdx) {
	return m_vecModel[iIdx];
}

CLEStrip* CLEPartCtrl::NewStrip(const s_string& strTex, CLEResManger* pResMagr) {
	SAFE_DELETE_ARRAY(m_pcStrip);
	m_iStripNum = 1;
	m_pcStrip = new CLEStrip[m_iStripNum];
	m_pcStrip[0].CreateStrip(pResMagr->m_pDev, strTex, pResMagr);
	return &m_pcStrip[0];
}

CLEStrip* CLEPartCtrl::GetStrip(int iIdx) {
	return &m_pcStrip[iIdx];
}

void CLEPartCtrl::SetPlayType(int nType) {
	
	// Loop
	if (nType == 0) {
		int n = 0;
		for (n = 0; n < m_iPartNum; ++n) {
			m_vecPartSys[n]->SetPlayTime(0);
			m_vecPartSys[n]->SetLoop(true);
		}

		for (n = 0; n < m_iModelNum; n++) {
			m_vecModel[n]->SetPlayType(PLAY_LOOP);
			m_vecModel[n]->Play();
		}
	}
}

void CLEPartCtrl::Play(int iTime) {
	int n = 0;
	for (n = 0; n < m_iPartNum; ++n) {
		m_vecPartSys[n]->Play(iTime);
	}

	for (n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].Play();
	}

	for (n = 0; n < m_iModelNum; n++) {
		m_vecModel[n]->Play();
	}
}

bool CLEPartCtrl::IsPlaying() {
	if (m_iPartNum > 0) {
		for (int n = 0; n < m_iPartNum; ++n) {
			if (m_vecPartSys[n]->IsPlaying()) {
				return true;
			}
		}
	}
	else if (m_iStripNum > 0) {
		for (int n = 0; n < m_iStripNum; ++n) {
			if (m_pcStrip[n].IsPlaying()) {
				return true;
			}
		}
	}
	else {
		for (int n = 0; n < m_iModelNum; n++) {
			if (m_vecModel[n]->IsPlaying()) {
				return true;
			}
		}
	}
	return false;
}

void CLEPartCtrl::CopyPartCtrl(CLEPartCtrl* pPart) {
	m_strName = pPart->m_strName;
	m_vecPartSys.resize(pPart->m_iPartNum);

	int n = 0;
	for (n = 0; n < pPart->m_iPartNum; ++n) {
		AddPartSys(pPart->m_vecPartSys[n]);
	}

	SAFE_DELETE_ARRAY(m_pcStrip);
	m_iStripNum = pPart->m_iStripNum;
	if (m_iStripNum) {
		m_pcStrip = new CLEStrip[m_iStripNum];
		for (n = 0; n < m_iStripNum; ++n) {
			m_pcStrip[n].CopyStrip(&pPart->m_pcStrip[n]);
		}
	}

	for (n = 0; n < m_iModelNum; n++) {
		SAFE_DELETE(m_vecModel[n]);
	}

	m_iModelNum = pPart->m_iModelNum;
	CChaModel* pModel;
	char psID[32];
	m_vecModel.resize(m_iModelNum);
	for (n = 0; n < m_iModelNum; n++) {
		pModel = pPart->m_vecModel[n];
		m_vecModel[n] = new CChaModel;
		_snprintf_s(psID, 32, _TRUNCATE, "%d", pModel->GetID());
		if (!m_vecModel[n]->LoadScript(psID)) {
			LG("error", "msgLoadScript %s", psID);
		}

		m_vecModel[n]->SetVel(pModel->GetVel());
		m_vecModel[n]->SetPlayType(pModel->GetPlayType());
		m_vecModel[n]->SetCurPose(pModel->GetCurPose());
		m_vecModel[n]->SetCurColor(pModel->GetCurColor());
		m_vecModel[n]->SetSrcBlend(pModel->GetSrcBlend());
		m_vecModel[n]->SetDestBlend(pModel->GetDestBlend());
		m_vecModel[n]->PlayPose(0, PLAY_PAUSE);
	}
}

void CLEPartCtrl::BindingRes(CLEResManger* pResMagr) {
	m_pfDailTime = pResMagr->GetDailTime();

	int n = 0;
	for (n = 0; n < m_iPartNum; ++n) {
		m_vecPartSys[n]->BindingRes(pResMagr);
	}

	for (n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].BindingRes(pResMagr);
	}

	for (n = 0; n < m_iModelNum; n++) {
		m_vecModel[n]->m_pDev = pResMagr->m_pDev;
	}
}

CLEPartSys* CLEPartCtrl::AddPartSys(CLEPartSys* part) {
	m_iPartNum++;
	if (m_iPartNum > MAX_PART_SYS) {
		m_iPartNum = MAX_PART_SYS;
		return NULL;
	}
	m_vecPartSys.setsize(m_iPartNum);
	CopyPartSys(m_vecPartSys[m_iPartNum - 1], part);
	return m_vecPartSys[m_iPartNum - 1];
}

CLEPartSys*	CLEPartCtrl::NewPartSys() {
	m_iPartNum++;
	if (m_iPartNum > MAX_PART_SYS) {
		m_iPartNum = MAX_PART_SYS;
		return NULL;
	}
	m_vecPartSys.setsize(m_iPartNum);
	return m_vecPartSys[m_iPartNum - 1];
}

void CLEPartCtrl::DeletePartSys(int iID) {
	if (iID < 0) {
		return;
	}

	if (m_iPartNum == 0) {
		return;
	}

	m_iPartNum--;
	m_vecPartSys.remove(iID);
}

void CLEPartCtrl::Clear() {
	m_iPartNum = 0;
	m_vecPartSys.clear();
}

void CLEPartCtrl::UpdateStrip() {
	for (int n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].UpdateFrame();
	}
}

void CLEPartCtrl::FrameMove(DWORD	dwDailTime) {
	int n = 0;
	for (n = 0; n < m_iPartNum; ++n) {
		m_vecPartSys[n]->FrameMove(dwDailTime);
	}

	for (n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].FrameMove();
	}

	for (n = 0; n < m_iModelNum; n++) {
		if (m_vecModel[n]->IsPlaying()) {
			m_vecModel[n]->UpdateMatrix();
			m_vecModel[n]->FrameMove();
		}
	}
}

void CLEPartCtrl::Render() {
	int n = 0;
	for (n = 0; n < m_iPartNum; ++n) {
		m_vecPartSys[n]->Render();
	}

	for (n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].Render();
	}

	for (n = 0; n < m_iModelNum; n++) {
		if (m_vecModel[n]->IsPlaying()) {
			m_vecModel[n]->Begin();
			m_vecModel[n]->Render();
			m_vecModel[n]->End();
		}
	}
}

bool CLEPartCtrl::SaveToFile(char* pszName) {
	if (strcmp(pszName, "no") == 0 || strcmp(pszName, "yes") == 0) {
		int i;
		i = 1;
	}

	FILE* t_pFile;
	fopen_s(&t_pFile, pszName, "wb");
	if (!t_pFile) {
		LG("error", "msg %s,Read-only file, open failed", pszName);
		return false;
	}

	// Vers
	DWORD t_dwVersion = CLEPartCtrl::ParVersion;
	fwrite(&t_dwVersion, sizeof(t_dwVersion), 1, t_pFile);

	char pszPartName[32];
	lstrcpy(pszPartName, m_strName.c_str());
	fwrite(pszPartName, sizeof(char), 32, t_pFile);

	fwrite(&m_iPartNum, sizeof(int), 1, t_pFile);

	fwrite(&m_fLength, sizeof(float), 1, t_pFile);

	int n = 0;
	for (n = 0; n < m_iPartNum; ++n) {
		m_vecPartSys[n]->SaveToFile(t_pFile);
	}

	fwrite(&m_iStripNum, sizeof(int), 1, t_pFile);
	for (n = 0; n < m_iStripNum; ++n) {
		m_pcStrip[n].SaveToFile(t_pFile);
	}

	fwrite(&m_iModelNum, sizeof(int), 1, t_pFile);
	for (n = 0; n < m_iModelNum; ++n) {
		m_vecModel[n]->SaveToFile(t_pFile);
	}

	fclose(t_pFile);
	return true;
}

bool CLEPartCtrl::LoadFromFile(char* pszName) {
	FILE* t_pFile;
	fopen_s(&t_pFile, pszName, "rb");
	if (t_pFile == NULL) {
		LG("error", "msg[%s] was not opened.(CLEPartCtrl::LoadFromFile)\n", pszName);
		return false;
	}

	if (strcmp(pszName, "no") == 0 || strcmp(pszName, "yes") == 0) {
		int i;
		i = 1;
	}
	string sName = pszName;

	DWORD t_dwVersion;
	fread(&t_dwVersion, sizeof(t_dwVersion), 1, t_pFile);
	if (t_dwVersion > CLEPartCtrl::ParVersion) {
		LG("error", "msg[%s]Version [%d] is higher than the version of the system[%d] (CLEPartCtrl::LoadFromFile)\n", pszName, t_dwVersion, CLEPartCtrl::ParVersion);
		return false;
	}

	if (t_dwVersion < 2) {
		LG("error", "msg[%s]Version [%d] is lower than the version that the system can handle[%d] (CLEPartCtrl::LoadFromFile)\n", pszName, t_dwVersion, 2);
		return false;
	}

	char pszPartName[32];
	fread(pszPartName, sizeof(char), 32, t_pFile);
	m_strName = pszPartName;

	fread(&m_iPartNum, sizeof(int), 1, t_pFile);

#ifdef USE_GAME
	m_vecPartSys.resize(m_iPartNum);
#endif
	
	m_vecPartSys.setsize(m_iPartNum);

	if (t_dwVersion >= 3) {
		fread(&m_fLength, sizeof(float), 1, t_pFile);
	}
	else {
		m_fLength = 0;
	}

	int n = 0;
	for (n = 0; n < m_iPartNum; ++n) {
		if (!m_vecPartSys[n]->LoadFromFile(t_pFile, t_dwVersion)) {
			LG("error", "msg[%s]The first [%d] subsystem failed to load.(CLEPartCtrl::LoadFromFile)\n", pszName, n);
			return false;
		}

		if (t_dwVersion < 6) {
			m_vecPartSys[n]->SetPlayTime(m_fLength);
		}
	}

	if (t_dwVersion >= 7) {
		SAFE_DELETE_ARRAY(m_pcStrip);
		fread(&m_iStripNum, sizeof(int), 1, t_pFile);
		m_pcStrip = new CLEStrip[m_iStripNum];

		for (n = 0; n < m_iStripNum; ++n) {
			if (!m_pcStrip[n].LoadFromFile(t_pFile, t_dwVersion)) {
				LG("error","msg [%s] [%d] Strip loading failed(CLEPartCtrl::LoadFromFile)\n", pszName, n);
				delete m_pcStrip;
				return false;
			}
		}
	}
	
	if (t_dwVersion >= 8) {
		fread(&m_iModelNum, sizeof(int), 1, t_pFile);
		m_vecModel.resize(m_iModelNum);
		for (n = 0; n < m_iModelNum; ++n) {
			m_vecModel[n] = new CChaModel;
			m_vecModel[n]->LoadFromFile(t_pFile);
		}
	}
	fclose(t_pFile);
	return true;
}

bool CLEPartCtrl::LoadFromMemory(CMemoryBuf* pbuf) {
	DWORD t_dwVersion;
	pbuf->mread(&t_dwVersion, sizeof(t_dwVersion), 1);
	if (t_dwVersion > CLEPartCtrl::ParVersion) {
		LG("error", "msg version [%d] is higher than the version of the system [%d] (CLEPartCtrl::LoadFromFile)\n", t_dwVersion, CLEPartCtrl::ParVersion);
		return false;
	}

	if (t_dwVersion < 2) {
		LG("error", "msg version [%d] is lower than the version that the system can handle[%d] (CLEPartCtrl::LoadFromFile)\n", t_dwVersion, 2);
		return false;
	}

	char pszPartName[32];
	pbuf->mread(pszPartName, sizeof(char), 32);
	m_strName = pszPartName;

	pbuf->mread(&m_iPartNum, sizeof(int), 1);

#ifdef USE_GAME
	m_vecPartSys.resize(m_iPartNum);
#endif

	m_vecPartSys.setsize(m_iPartNum);

	if (t_dwVersion >= 3) {
		pbuf->mread(&m_fLength, sizeof(float), 1);
	}
	else {
		m_fLength = 0;
	}

	int n = 0;
	for (n = 0; n < m_iPartNum; ++n) {
		m_vecPartSys[n]->LoadFromMemory(pbuf, t_dwVersion);
		if (t_dwVersion < 6) {
			m_vecPartSys[n]->SetPlayTime(m_fLength);
		}
	}
	
	if (t_dwVersion >= 7) {
		SAFE_DELETE_ARRAY(m_pcStrip);

		pbuf->mread(&m_iStripNum, sizeof(int), 1);
		if (m_iStripNum > 0) {
			m_pcStrip = new CLEStrip[m_iStripNum];
			for (n = 0; n < m_iStripNum; ++n) {
				m_pcStrip[n].LoadFromMemory(pbuf, t_dwVersion);
			}
		}
	}

	if (t_dwVersion >= 8) {
		pbuf->mread(&m_iModelNum, sizeof(int), 1);
		m_vecModel.resize(m_iModelNum);
		for (n = 0; n < m_iModelNum; ++n) {
			m_vecModel[n] = new CChaModel;
			m_vecModel[n]->LoadFromMemory(pbuf);
		}
	}

	return true;
}

void CLEPartCtrl::GetRes(CLEResManger* pResMagr, std::vector<INT>& vecTex, std::vector<INT>& vecModel, std::vector<INT>& vecEff) {
	char pszPath[MAX_PATH];
	char pszNewPath[MAX_PATH];
	std::vector<INT>::iterator it;
	int id = -1;

	int n;
	int idtex, idmodel, ideff;
	s_string strName;
	if (m_iStripNum > 0) {
		for (n = 0; n < m_iStripNum; ++n) {
			strName = m_pcStrip[n].GetTexName();
			id = pResMagr->GetTextureID(strName);
			if (id >= 0) {
				it = std::find(vecTex.begin(), vecTex.end(), id);
				if (it == vecTex.end()) {
					vecTex.push_back(id);
					_snprintf_s(pszPath, MAX_PATH, _TRUNCATE, "texture/effect/%s", strName.c_str());
					_snprintf_s(pszNewPath, MAX_PATH, _TRUNCATE, "effect/new/texture/%s", strName.c_str());
					// if (!::CopyFile(pszPath, pszNewPath, FALSE)) {
					// }
				}
			}
		}
	}
	else if (m_iPartNum > 0) {
		for (n = 0; n < m_iPartNum; ++n) {
			m_vecPartSys[n]->GetRes(idtex, idmodel, ideff);
			if (idtex >= 0) {
				it = std::find(vecTex.begin(), vecTex.end(), idtex);
				if (it == vecTex.end()) {
					vecTex.push_back(idtex);
				}
			}

			if (idmodel >= 0) {
				it = std::find(vecModel.begin(), vecModel.end(), idmodel);
				if (it == vecModel.end()) {
					vecModel.push_back(idmodel);
				}
			}

			if (ideff >= 0) {
				it = std::find(vecEff.begin(), vecEff.end(), ideff);
				if (it == vecEff.end()) {
					vecEff.push_back(ideff);
				}
			}
		}
	}
}

void CLEPartCtrl::GetHitRes(CLEResManger* pResMagr, std::vector<s_string>& vecPar) {
	if (m_iStripNum > 0) {
		return;
	}
	else if (m_iPartNum > 0) {
		s_string strname;
		int id, n;
		for (n = 0; n < m_iPartNum; ++n) {
			strname = m_vecPartSys[n]->GetHitEff();
			id = pResMagr->GetPartCtrlID(strname);
			if (id >= 0) {
				vecPar.push_back(strname);
			}
		}
	}
}

static void keyframe_proc(DWORD type, DWORD pose_id, DWORD key_id, DWORD key_frame, void* param) {
	CChaModel* pCha = ((CChaModel*)param);
	switch (type) {
	case KEYFRAME_TYPE_BEGIN: {
		pCha->SetPlaying(true);
		break;
	}
	case KEYFRAME_TYPE_KEY: {
		break;
	}
	case KEYFRAME_TYPE_END:
		if (pCha->GetPlayType() != PLAY_LOOP) {
			pCha->SetPlaying(false);
		}
		break;
	}
}

bool CChaModel::LoadScript(const s_string& strModel) {
	LEChaLoadInfo	sModel;
	SChaAction	sAction;

	if (!CScriptFile::m_ctScript.ReadSection((char*)(LPCSTR)strModel.c_str())) {
		return false;
	}

	if (!CScriptFile::m_ctScript.ReadLine("Skeleton", sModel.bone, 64)) {
		return false;
	}

	_iID = atoi((char*)(LPCSTR)strModel.c_str());

	char str[64];
	for (short i = 0; i < 5; i++) {
		_snprintf_s(str, 64, _TRUNCATE, "Skin%d", i + 1);
		if (!CScriptFile::m_ctScript.ReadLine(str, sModel.part[i], 64)) {
			return false;
		}
	}
	
	if (!LoadChaModel(sModel)) {
		return false;
	}
	
	if (!CScriptFile::m_ctScript.GetCharAction(atoi((char*)(LPCSTR)strModel.c_str()), &sAction)) {
		return false;
	}

	if (!LoadPose(sAction)) {
		return false;
	}

	SetPoseKeyFrameProc(keyframe_proc, (void*)this);

	return true;
}

bool CChaModel::LoadChaModel(LEChaLoadInfo& info) {
	if (strlen(info.bone) <= 0) {
		return false;
	}

	if (FAILED(LECharacter::Load(&info))) {
		return false;
	}

	return true;
}

bool CChaModel::LoadPose(SChaAction& SCharAct) {
	drPoseInfo pi;
	memset(&pi, 0, sizeof(pi));
	for (int iActCount = 0; iActCount < SCharAct.m_iMaxActionNum; iActCount++) {
		if (SCharAct.m_SActionInfo[iActCount].m_sActionNO < 1) {
			continue;
		}

		pi.start = SCharAct.m_SActionInfo[iActCount].m_sStartFrame;
		pi.end = SCharAct.m_SActionInfo[iActCount].m_sEndFrame;
		pi.key_frame_num = SCharAct.m_SActionInfo[iActCount].m_sKeyFrameNum < MAX_KEY_FRAME_NUM ? SCharAct.m_SActionInfo[iActCount].m_sKeyFrameNum : MAX_KEY_FRAME_NUM;
		for (int iKeyFrameCount = 0; iKeyFrameCount < (short)pi.key_frame_num; iKeyFrameCount++) {
			pi.key_frame_seq[iKeyFrameCount] = SCharAct.m_SActionInfo[iActCount].m_sKeyFrame[iKeyFrameCount];
		}

		if (GetPoseCtrl()) {
			GetPoseCtrl()->InsertPose(SCharAct.m_SActionInfo[iActCount].m_sActionNO, &pi, 1);
		}
	}
	return true;
}

void CChaModel::PlayPose(DWORD id, DWORD type) {
	LECharacter::PlayPose(id, type, 0.0f, _fVel);
}

void CChaModel::Begin() {
	m_pDev->GetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pDev->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	m_pDev->GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pDev->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pDev->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDev->GetDevice()->SetRenderState(D3DRS_SRCBLEND, _eSrcBlend);
	m_pDev->GetDevice()->SetRenderState(D3DRS_DESTBLEND, _eDestBlend);

#ifdef USE_RENDER
	D3DMATERIALX mtl;
	mtl.Ambient = (D3DCOLORVALUE)_dwCurColor;
	mtl.Diffuse = (D3DCOLORVALUE)_dwCurColor;
	SetMaterial(&mtl);
	SetOpacity(_dwCurColor.a);
#else
	m_pDev->SetRenderState(D3DRS_DITHERENABLE, FALSE);
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDev->SetRenderState(D3DRS_TEXTUREFACTOR, _dwCurColor);

	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	m_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	m_pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	m_pDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	m_pDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
#endif

}

void CChaModel::End() {
	m_pDev->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pDev->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pDev->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pDev->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDev->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pDev->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	m_pDev->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pDev->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pDev->GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
}

void CChaModel::SaveToFile(FILE* file) {
	fwrite(&_iID, sizeof(int), 1, file);
	fwrite(&_fVel, sizeof(float), 1, file);
	fwrite(&_iPlayType, sizeof(int), 1, file);
	fwrite(&_iCurPose, sizeof(int), 1, file);

	int eblend = (int)_eSrcBlend;
	fwrite(&eblend, sizeof(int), 1, file);
	eblend = (int)_eDestBlend;
	fwrite(&eblend, sizeof(int), 1, file);
	fwrite(&_dwCurColor, sizeof(D3DXCOLOR), 1, file);
}

void CChaModel::LoadFromFile(FILE* file) {
	fread(&_iID, sizeof(int), 1, file);
	fread(&_fVel, sizeof(float), 1, file);
	fread(&_iPlayType, sizeof(int), 1, file);
	fread(&_iCurPose, sizeof(int), 1, file);

	int eblend;
	fread(&eblend, sizeof(int), 1, file);
	_eSrcBlend = (D3DBLEND)eblend;
	fread(&eblend, sizeof(int), 1, file);
	_eDestBlend = (D3DBLEND)eblend;
	fread(&_dwCurColor, sizeof(D3DXCOLOR), 1, file);

	char psID[32];
	_snprintf_s(psID, 32, _TRUNCATE, "%d", _iID);
	LoadScript(psID);
	SetVel((int)(_fVel * 1000));
	SetPlayType(_iPlayType);
	SetCurPose(_iCurPose);
	PlayPose(_iCurPose, PLAY_PAUSE);
}

void CChaModel::LoadFromMemory(CMemoryBuf* pbuf) {
	pbuf->mread(&_iID, sizeof(int), 1);
	pbuf->mread(&_fVel, sizeof(float), 1);
	pbuf->mread(&_iPlayType, sizeof(int), 1);
	pbuf->mread(&_iCurPose, sizeof(int), 1);

	int eblend;
	pbuf->mread(&eblend, sizeof(int), 1);
	_eSrcBlend = (D3DBLEND)eblend;
	pbuf->mread(&eblend, sizeof(int), 1);
	_eDestBlend = (D3DBLEND)eblend;
	pbuf->mread(&_dwCurColor, sizeof(D3DXCOLOR), 1);

	char psID[32];
	_snprintf_s(psID, 32, _TRUNCATE, "%d", _iID);
	LoadScript(psID);
	SetVel((int)(_fVel * 1000));
	SetPlayType(_iPlayType);
	SetCurPose(_iCurPose);
	PlayPose(_iCurPose, PLAY_PAUSE);
}

bool CLELink::Create(LECharacter *pChaMain, int iDummy1, LECharacter *pChaTag, int iDummy2, char* pszTex, int iTexNum, CLEResManger* pResMgr, D3DXVECTOR3* pEyePos, LERender* pDev) {
	m_pDev = pDev;
	_pCEffFile = pResMgr->GetEffectFile();
	_fDailTime = pResMgr->GetDailTime();
	_pEyePos = pEyePos;

	_vStart = D3DXVECTOR3(0, 0, 0);
	_vEnd = D3DXVECTOR3(10, 0, 0);

	_vdir = _vEnd - _vStart;
	_fdist = D3DXVec3Length(&_vdir);
	D3DXVec3Normalize(&_vdir, &_vdir);

	int n;

	_pFrame = new LEFrame[LINK_FACE];
	for (n = 0; n < LINK_FACE; n++) {
		_pFrame[n].vPos1.m_dwDiffuse = 0xffffffff;
		_pFrame[n].vPos2.m_dwDiffuse = 0xffffffff;
	}

	_pChaMain = pChaMain;
	_pChaTag = pChaTag;
	_iDummy1 = iDummy1;
	_iDummy2 = iDummy2;

	char pszName[32];
	_pTex = new drITex*[iTexNum];
	s_string strName;
	int id;
	for (n = 0; n < iTexNum; n++) {
		_pTex[n] = NULL;
		_snprintf_s(pszName, 32, _TRUNCATE, "%s%d", pszTex, n);
		strName = pszName;
		id = pResMgr->GetTextureID(strName);
		if (id < 0) {
			return false;
		}

		_pTex[n] = pResMgr->GetTextureByIDdr(id);
	}
	return true;
}

void CLELink::ColArc(float fradius) {
	D3DXVECTOR3 veyedir, vcross;
	D3DXVec3Normalize(&veyedir, &(*_pEyePos - _vStart));
	D3DXVec3Cross(&vcross, &_vdir, &veyedir);
	D3DXVec3Normalize(&vcross, &vcross);
	D3DXVECTOR3 vOrg(-fradius, 0, 0);
	D3DXVECTOR3 vTemp;


	D3DXMATRIX mat;
	int n;
	int num = 10;
	float fstep = (D3DX_PI / 2.0f) / float(num);
	float fhei;
	float ft;
	for (n = 0; n < num; ++n) {
		D3DXMatrixRotationY(&mat, n * fstep);
		D3DXVec3TransformCoord(&vTemp, &vOrg, &mat);
		fhei = vTemp.z;

		ft = fradius + vTemp.x;
		if (ft < 0.00001f) {
			ft = 0.0f;
		}

		vTemp = _vStart + _vdir * ft;
		vTemp.z = fhei;

		ft = ft / _fdist;

		_pFrame[n].vPos1.m_SPos = vTemp + (-vcross * 1.0f);
		_pFrame[n].vPos2.m_SPos = vTemp + (vcross * 1.0f);

		if (n == 0) {
			_pFrame[n].vPos1.m_SUV.x = 0;
			_pFrame[n].vPos1.m_SUV.y = 1;

			_pFrame[n].vPos2.m_SUV.x = 0;
			_pFrame[n].vPos2.m_SUV.y = 0;
		}
		else {
			_pFrame[n].vPos1.m_SUV.x = ft;
			_pFrame[n].vPos1.m_SUV.y = 1;

			_pFrame[n].vPos2.m_SUV.x = ft;
			_pFrame[n].vPos2.m_SUV.y = 0;
		}
	}
	_pFrame[num].vPos1.m_SPos = _vEnd + (-vcross * 1.0f);
	_pFrame[num].vPos2.m_SPos = _vEnd + (vcross * 1.0f);

	_pFrame[num].vPos1.m_SUV.x = 1;
	_pFrame[num].vPos1.m_SUV.y = 1;

	_pFrame[num].vPos2.m_SUV.x = 1;
	_pFrame[num].vPos2.m_SUV.y = 0;
}

void CLELink::GetPhysique() {
	_vStart = D3DXVECTOR3(0, 0, 0);
	_vEnd = D3DXVECTOR3(10, 0, 0);

	_vdir = _vEnd - _vStart;
	_fdist = D3DXVec3Length(&_vdir);
	D3DXVec3Normalize(&_vdir, &_vdir);

	// Take the maximum radius
	float frad = 3.0f;
	if (_fdist > frad * 2) {
		_fRadius = frad - (frad * ((_fdist / frad - 1.0f) / 10.0f));
	}
	else {
		_fRadius = frad + ((frad - _fdist) / 10.0f);
	}
	ColArc(_fRadius);
}

void CLELink::FrameMove() {
	GetPhysique();
	_fCurTime += *_fDailTime;
	if (_fCurTime > 0.15f) {
		_iCurTex++;
		if (_iCurTex >= 4) {
			_iCurTex = 0;
		}
		_fCurTime = 0;
	}
}

void CLELink::Render() {
	_pCEffFile->SetTechnique(0);
	_pCEffFile->Begin();
	_pCEffFile->Pass(0);

	m_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	m_pDev->SetTransformWorld(&mat);
	m_pDev->SetTexture(0, _pTex[_iCurTex]->GetTex());
	m_pDev->SetVertexShader(LINK_FVF);
	m_pDev->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 20, _pFrame, sizeof(LinkVer));

	_pCEffFile->End();
}

