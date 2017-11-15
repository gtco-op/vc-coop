// Server.h

#pragma once

using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Text;

namespace Server {

	public ref class Sender
	{
	public:

		UdpClient^ sender = gcnew UdpClient();
		void Connect(String^ ip, Int32 port)
		{
			try
			{
				sender->Connect(ip, port);
			}
			catch (Exception^ ex)
			{
				log("[ERROR]" + ex->Message);
			}
		}

		void log(String^ text)
		{
			Console::WriteLine(text);
		}

		void Send(String^ text)
		{
			try
			{
				array<unsigned char>^ bytes = Encoding::Default->GetBytes(text);
				sender->Send(bytes, bytes->Length);
			}
			catch (Exception^ ex)
			{
				log("[ERROR]" + ex->Message);
			}
		}

	};


	public ref class Reciever
	{
	public:
		UdpClient^ sock;
		Int32 port = 89;
		IPEndPoint^ lpendp;
		void Initialize(Int32 port)
		{
			sock = gcnew UdpClient(port);
			lpendp = gcnew IPEndPoint(0, port);
		}

		void Log(String^ text)
		{
			Console::WriteLine(text);
		}

		void Recieve()
		{
			String^ re_text;
			array<unsigned char>^ re_bytes;
			try
			{
				re_bytes = sock->Receive(lpendp);
				re_text = Encoding::Default->GetString(re_bytes);
				Console::WriteLine(re_text);
			}
			catch (Exception^ ex)
			{
				Log("[ERROR] " + ex->Message);
			}
		}

	};
}
