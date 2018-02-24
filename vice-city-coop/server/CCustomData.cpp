#include "server.h"

CCustomData::CCustomData(std::string szName, CustomDataType data_type, char* data_ptr, double data_size, std::string dir)
{
	if (szName.empty())
	{
		gLog->Log("[CCustomData] Failed to initialize CCustomData object. Reason: No name set for data object.\n");
		return;
	}
	if (data_size <= 0)	
	{
		gLog->Log("[CCustomData] Failed to initialize CCustomData object. Reason: data_size <= 0 (%d)\n", data_size);
		return;
	}
	if (!dir.empty())
	{
		szDir = dir;
	}

	this->szName	 = szName;
	this->cType		 = data_type;
	this->pDataSize	 = data_size;
	this->pData		 = new char[this->pDataSize];
	memset(this->pData, 0x00, this->pDataSize);

	memcpy(this->pData, data_ptr, this->pDataSize);

	gLog->Log("[CCustomData] %s\tType: %s\tData Size: %d\n", this->szName.c_str(), gDataMgr->GetCustomDataTypeString(cType).c_str(), this->pDataSize);
}
CCustomData::~CCustomData() 
{
	delete[] this->pData;
}