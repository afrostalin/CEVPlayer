// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CryCore/Platform/IPlatformOS.h>
#include <IActionMapManager.h>


typedef std::function<void(int activationMode, float value)> TActionCallback;

class CInputDispatcher : public IActionListener
{
public:
	CInputDispatcher();
	~CInputDispatcher();
public:
	// IActionListener
	void OnAction(const ActionId& action, int activationMode, float value) override;
	// ~IActionListener
public:
	void RegisterAction(const char* szGroupName, const char* szName, TActionCallback callback);
	void BindAction(const char* szGroupName, const char* szName, const char* enumName, EActionInputDevice device, EKeyId keyId, bool bOnPress = true, bool bOnRelease = true, bool bOnHold = true);
private:
	void AddAction(const char* szName, TActionCallback callback);
protected:
	struct SAction
	{
		const char* szName;
		TActionCallback callback;
	};
protected:
	DynArray<SAction> m_actions;
};