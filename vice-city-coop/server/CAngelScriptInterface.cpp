#include "server.h"

CAngelScriptInterface::CAngelScriptInterface()
{
	this->Initialize();
}
CAngelScriptInterface::CAngelScriptInterface(const char* filename, const char* modulename)
{
	this->Initialize();

	this->LoadScript(filename, modulename, true);
}
CAngelScriptInterface::~CAngelScriptInterface()
{

}
void CAngelScriptInterface::MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	gLog->Log("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}
void CAngelScriptInterface::Initialize()
{
	SetEngine(asCreateScriptEngine());

	int r = GetEngine()->SetMessageCallback(asFUNCTION(CAngelScriptInterface::MessageCallback), 0, asCALL_CDECL); assert(r >= 0);

	RegisterStdString(GetEngine());

	r = GetEngine()->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(CAngelScriptInterface::print), asCALL_CDECL); assert(r >= 0);
}
void CAngelScriptInterface::LoadScript(const char* filename, const char* modulename, bool execute = true)
{
	CScriptBuilder builder;
	int r = builder.StartNewModule(GetEngine(), modulename);
	if (r < 0)
	{
		gLog->Log("Unrecoverable error while starting a new module.\n");
		return;
	}
	r = builder.AddSectionFromFile(filename);
	if (r < 0)
	{
		gLog->Log("Please correct the errors in the script and try again.\n");
		return;
	}
	r = builder.BuildModule();
	if (r < 0)
	{
		gLog->Log("Please correct the errors in the script and try again.\n");
		return;
	}

	if (execute)
		Execute(modulename);
}
void CAngelScriptInterface::Execute(const char* modulename)
{
	asIScriptModule *mod = GetEngine()->GetModule(modulename);
	asIScriptFunction *func = mod->GetFunctionByDecl("void main()");

	if (func == 0)
	{
		gLog->Log("The script must have the function 'void main()'. Please add it and try again.\n");
		return;
	}
	asIScriptContext *ctx = GetEngine()->CreateContext();
	ctx->Prepare(func);
	int r = ctx->Execute();
	if (r != asEXECUTION_FINISHED)
	{
		if (r == asEXECUTION_EXCEPTION)
		{
			gLog->Log("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
		}
	}

	ctx->Release();
	engine->ShutDownAndRelease();
}
void CAngelScriptInterface::print(string &msg)
{
	gLog->Log("%s", msg.c_str());
}