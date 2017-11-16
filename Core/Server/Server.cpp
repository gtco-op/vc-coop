/*
		Vice City CO-OP Server v0.1.0.a
		Contributer(s): Zeyad_Ahmed
						Crspy
		Copyright (c) 2017 VC:CO-OP Team
*/

#include "stdafx.h"

#include "Server.h"

using namespace Server;

String^ gdt()
{
	return Dns::GetHostName();
}

void main()
{
	Reciever^ reciever = gcnew Reciever();
	Sender^ sender = gcnew Sender();
	Console::Title = "Vice City: CO-OP Server";
	Console::WriteLine("=======================================");
	Console::WriteLine("     Vice City: CO-OP Server");
	Console::WriteLine("=======================================");
	Console::WriteLine("Copyrights (c) 2017 VC:CO-OP Team" + "\n" + "If you have any problem , talk to us at www.vicecityco-op.ml");
	Console::WriteLine("---------------------------------------");
	try
	{
		String^ hostname = Dns::GetHostName();
		String^ ip = Dns::GetHostAddresses(hostname)->GetValue(1)->ToString();
		reciever->Initialize(5012);
		sender->Connect(ip, 5012);
		sender->Send("[SERVER] Server Started"); //It will be displayed if it got connected
		do
		{
			reciever->Recieve();
			String^ m; 
			
			sender->Send("[SERVER OWNER] " + Console::ReadLine()); // send chat message

		} while (true);

	}
	catch (Exception^ ex)
	{
		Console::WriteLine("[ERROR]4 " + ex->Message);
		Threading::Thread::Sleep(90000);
	}
}

