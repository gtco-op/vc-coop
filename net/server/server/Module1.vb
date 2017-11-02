'=================================================
'   Vice City: CO-OP Server v0.1.0
'   File Author(s): Zeyad Ahmed
'   Copyrights (c) 2017 VC:CO-OP Team
'=================================================
Imports System.Text, System.IO, System.Net, System.Net.Sockets


Module Module1
    Dim reciever As New reciever
    Dim sender As New sender

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
            reciever.Intialize(5012) 'Starting Reciever at the local Address
            sender.connect(ip, 5012) 'starting sender at local address and connecting to it
            sender.Send("[SERVER] Server Started") 'Just testing some sending bytes
            reciever.Recieve() 'recieve the last byte
            Threading.Thread.Sleep(10000)

        Catch ex As Exception
            Console.WriteLine("[ERROR] " & ex.Message)
            Threading.Thread.Sleep(90000)
        End Try
    End Sub
End Module
