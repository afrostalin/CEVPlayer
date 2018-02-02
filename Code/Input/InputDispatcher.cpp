// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "InputDispatcher.h"

#include <CryGame/IGameFramework.h>

CInputDispatcher::CInputDispatcher()
{
	IActionMapManager* pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
	if (pActionMapManager != nullptr)
	{
		gEnv->pGameFramework->GetIActionMapManager()->AddExtraActionListener(this);
	}
}

CInputDispatcher::~CInputDispatcher()
{
	if (gEnv->pGameFramework != nullptr)
	{
		IActionMapManager* pAMMgr = gEnv->pGameFramework->GetIActionMapManager();
		if (pAMMgr != nullptr)
		{
			gEnv->pGameFramework->GetIActionMapManager()->RemoveExtraActionListener(this);
		}
	}

	m_actions.clear();
}

void CInputDispatcher::OnAction(const ActionId & actionID, int activationMode, float value)
{
	for (const SAction& action : m_actions)
	{
		if (!strcmp(action.szName, actionID.c_str()))
		{
			action.callback(activationMode, value);
			break;
		}
	}
}

void CInputDispatcher::AddAction(const char * szName, TActionCallback callback)
{
	for (auto it = m_actions.begin(); it != m_actions.end(); it++)
	{
		if (strcmp(it->szName, szName) == 0)
		{
			m_actions.erase(it);
			break;
		}
	}

	m_actions.emplace_back(CInputDispatcher::SAction{ szName, callback });
}

void CInputDispatcher::RegisterAction(const char * szGroupName, const char * szName, TActionCallback callback)
{
	IActionMapManager *pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
	IActionMap* pActionMap = pActionMapManager->GetActionMap(szGroupName);

	if (pActionMap == nullptr)
	{
		pActionMap = pActionMapManager->CreateActionMap(szGroupName);
	}

	IActionMapAction* pAction = pActionMap->GetAction(ActionId(szName));

	if (pAction != nullptr)
	{
		pActionMap->RemoveAction(ActionId(szName));
	}

	pActionMap->CreateAction(ActionId(szName));

	AddAction(szName, callback);
}

void CInputDispatcher::BindAction(const char * szGroupName, const char * szName, const char* enumName, EActionInputDevice device, EKeyId keyId, bool bOnPress, bool bOnRelease, bool bOnHold)
{
	IActionMapManager *pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
	IActionMap* pActionMap = pActionMapManager->GetActionMap(szGroupName);
	if (pActionMap == nullptr)
	{
		LogError("Tried to bind input component action %s in group %s without registering it first!", szName, szGroupName);
		return;
	}

	IActionMapAction* pAction = pActionMap->GetAction(ActionId(szName));
	if (pAction == nullptr)
	{
		LogError("Tried to bind input component action %s in group %s without registering it first!", szName, szGroupName);
		return;
	}

	SActionInput input;
	input.input = input.defaultInput = enumName;
	input.inputDevice = device;

	if (bOnPress)
	{
		input.activationMode |= eAAM_OnPress;
	}
	if (bOnRelease)
	{
		input.activationMode |= eAAM_OnRelease;
	}
	if (bOnHold)
	{
		input.activationMode |= eAAM_OnHold;
	}

	pActionMap->AddAndBindActionInput(ActionId(szName), input);

	pActionMapManager->EnableActionMap(szGroupName, true);
}
