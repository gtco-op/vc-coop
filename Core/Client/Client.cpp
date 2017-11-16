// This is the main DLL file.

#include "stdafx.h"

#include "Client.h"

using namespace Client;

void log(String^ text)
{
	Console::WriteLine(text);
}

int main(array<String^>^ args)
{
	Console::Title = "Vice City: CO-OP Client";
	String^ ip = args[0];
	Int32 port = Convert::ToUInt32(args[1]);
	String^ nickname = args[2];
	try
	{

		Sender^ sender;

		log("Connecting to " + ip + ":" + port + " By nickname" + nickname);

		sender->Connect(ip, port);
		sender->Send("Connected to " + ip + ":" + port + " By nickname" + nickname);

	}
	catch (Exception^ ex)
	{
		log("[Error] " + ex->Message);
	}

	/*
	// Intializing the Server

	UdpClient^ reciever;
	UdpClient^ sender;
	IPEndPoint^ lpendp;
	reciever = gcnew UdpClient(port);
	lpendp = gcnew IPEndPoint(0, port);
	sender = gcnew UdpClient();
	reciever->Connect(ip, port);
	array<unsigned char>^ bytes = Encoding::Default->GetBytes(nickname + " connected from ip " + ip + ":" + port);
	sender->Send(bytes, bytes->Length);
	Threading::Thread::Sleep(50000);

	catch (Exception^ ex)
	{
	Console::WriteLine(ex->Message);
	}
	*/
}

