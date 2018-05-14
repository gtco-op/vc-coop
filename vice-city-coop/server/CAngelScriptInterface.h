#pragma once

class CAngelScriptInterface
{
public:
	CAngelScriptInterface();
	CAngelScriptInterface(const char* filename, const char* modulename);
	~CAngelScriptInterface();

	static void MessageCallback(const asSMessageInfo *msg, void *param);
	static void print(string &msg);

	void Initialize();
	void LoadScript(const char* filename, const char* modulename, bool execute);
	void Execute(const char* modulename);

	asIScriptEngine* GetEngine() { return engine; }
	void SetEngine(asIScriptEngine* e) { engine = e; }

private:
	asIScriptEngine * engine;
};
