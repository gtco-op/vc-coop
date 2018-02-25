#pragma once
class CLuaScript
{
public:
	CLuaScript(CCustomData* ptr);
	~CLuaScript();

	virtual CCustomData* GetData() { return m_Data; }
protected:
	CCustomData* m_Data;
};

