#pragma once

enum CustomDataType {
	TYPE_CLIENT_SCRIPT_DATA = 1000,
	TYPE_SERVER_SCRIPT,
	TYPE_CLIENT_SCRIPT,
	TYPE_SCM,
	TYPE_MISC
};
class CCustomData
{
public:
	CCustomData(std::string szName, CustomDataType data_type, char* data_ptr, double data_size, std::string dir = std::string(""));
	~CCustomData();

	std::string		GetName() { return szName; }
	
	CustomDataType	GetType() { return cType; }

	char*			GetData() { return pData; }
	size_t			GetSize() { return pDataSize; }

	std::string		GetDir() { return szDir;  }

protected:
	std::string		szName;

	CustomDataType	cType;

	char*			pData;
	size_t			pDataSize;

	std::string		szDir;
};

