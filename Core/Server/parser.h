#pragma once
using namespace System::Text;
#include "stdafx.h"	
using namespace System;
public ref class Parser{


public:
	
	String^ Parse(String^ data)
	{
		if (data->StartsWith("move")){
			String^ Seprator = " ";
			array<String^>^ sp = data->Split(Convert::ToChar(" "));
			String^ id = sp[0];
			String^ x = sp[1];
			String^ y = sp[2];
			String^ z = sp[3];
			// i have to use SetPedPos(id, x, y, z);
		}
	}
};