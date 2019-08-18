#pragma once

#include "drHeader.h"
#include "drStdInc.h"
#include "drClassDecl.h"
#include "drErrorCode.h"
#include "drDirectX.h"
#include "drITypes.h"
#include "drInterfaceExt.h"
#include "drShaderMgr.h"

DR_BEGIN

// remarks
// 1����̬shader���������
//    �����ʼ����ʱ���decl��shader��ֵ���м䲻�����գ��������л�
// 2����̬shader���������
//    �����ʼ����ʱ���decl��ֵ����Ⱦʱ����ݶ������ͣ�����������
//    drShaderDeclMgr������̬ƥ����ң������㷨��Ч����Ҫ��
// 3���Զ���Shader�����������user-defined shader��


class drRenderCtrlAgent : public drIRenderCtrlAgent
{
	DR_STD_DECLARATION()

private:
	drMatrix44 _mat_local;
	drMatrix44 _mat_parent;
	drMatrix44 _mat_global;

	drIResourceMgr* _res_mgr;
	drIMeshAgent* _mesh_agent;
	drIMtlTexAgent* _mtltex_agent;
	drIAnimCtrlAgent* _anim_agent;
	drIRenderCtrlVS* _render_ctrl;

	DWORD _decl_type;
	DWORD _vs_type;
	DWORD _ps_type;


public:
	void setPixelShader(const std::string& filename)
	{
		_render_ctrl->setPixelShader(filename);
	}

public:
	drRenderCtrlAgent(drIResourceMgr* res_mgr);
	~drRenderCtrlAgent();

	drIResourceMgr* GetResourceMgr()
	{
		return _res_mgr;
	}

	drIMeshAgent* GetMeshAgent()
	{
		return _mesh_agent;
	}

	drIMtlTexAgent* GetMtlTexAgent()
	{
		return _mtltex_agent;
	}

	drIAnimCtrlAgent* GetAnimCtrlAgent()
	{
		return _anim_agent;
	}

	drIRenderCtrlVS* GetRenderCtrlVS()
	{
		return _render_ctrl;
	}

	void SetMatrix(const drMatrix44* mat)
	{
		_mat_global = *mat;
	}

	void SetLocalMatrix(const drMatrix44* mat_local);
	void SetParentMatrix(const drMatrix44* mat_parent);
	drMatrix44* GetLocalMatrix()
	{
		return &_mat_local;
	}

	drMatrix44* GetParentMatrix()
	{
		return &_mat_parent;
	}

	drMatrix44* GetGlobalMatrix()
	{
		return &_mat_global;
	}

	void BindMeshAgent(drIMeshAgent* agent)
	{
		_mesh_agent = agent;
	}

	void BindMtlTexAgent(drIMtlTexAgent* agent)
	{
		_mtltex_agent = agent;
	}

	void BindAnimCtrlAgent(drIAnimCtrlAgent* agent)
	{
		_anim_agent = agent;
	}

	DR_RESULT SetRenderCtrl(DWORD ctrl_type);
	void SetRenderCtrl(drIRenderCtrlVS* ctrl)
	{
		_render_ctrl = ctrl;
	}

	void SetVertexShader(DWORD type)
	{
		_vs_type = type;
	}

	void SetVertexDeclaration(DWORD type)
	{
		_decl_type = type;
	}

	DWORD GetVertexShader() const
	{
		return _vs_type;
	}
	DWORD GetVertexDeclaration() const
	{
		return _decl_type;
	}


	virtual DR_RESULT Clone(drIRenderCtrlAgent** ret_obj);
	virtual DR_RESULT BeginSet();
	virtual DR_RESULT EndSet();
	virtual DR_RESULT BeginSetSubset(DWORD subset);
	virtual DR_RESULT EndSetSubset(DWORD subset);
	virtual DR_RESULT DrawSubset(DWORD subset);
};



DR_END
