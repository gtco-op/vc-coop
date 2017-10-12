'=================================================
'   Vice City: CO-OP Server v0.1.0
'   File Author(s): Zeyad Ahmed
'   Copyrights (c) 2017 VC:CO-OP Team
'=================================================
Imports System.Text, System.IO, System.Net, System.Net.Sockets


Module Module1
    Dim sock As UdpClient
    Dim port As Int32 = 25648
    Dim reciever As UdpClient
    Dim ipend As IPEndPoint = New IPEndPoint(0, port)

    Function gdt() As String
        Return Dns.GetHostName

    End Function

    Sub main()
        Console.Title = "Vice City: CO-OP Server v0.1.0"
        Console.WriteLine("=======================================")
        Console.WriteLine("     Vice City: CO-OP Server v0.1.0")
        Console.WriteLine("=======================================")
        Console.WriteLine("Copyrights (c) 2017 VC:CO-OP Team" & vbCrLf & "If you have any problem , talk to us at www.vicecityco-op.com")
        Console.WriteLine("---------------------------------------")
        Try
            Dim hostname As String = Dns.GetHostName()
            Dim ip As String = Dns.GetHostAddresses(hostname).GetValue(3).ToString
            Console.WriteLine("Starting Server at " & ip & ":" & port)

            sock = New UdpClient()
            sock.Connect(ip, port)
            Console.WriteLine("Started.")
            'Threading.Thread.Sleep(100000)
            Do
                Dim bytes() As Byte = reciever.Receive(ipend)
                Console.WriteLine(Encoding.Default.GetString(bytes))
            Loop
        Catch ex As Exception
            Console.WriteLine("Error " & ex.Message)
            Threading.Thread.Sleep(90000)
        End Try
    End Sub
End Module
