'===================================================
'          Vice City: CO-OP Client v0.1.0
'===================================================
'---------------------------------------------------
'   File Author(s): Zeyad_Ahmed
'   Copyrights (c) 2017 VC:CO-OP Team
'               www.vicecityco-op.ml
'---------------------------------------------------
Imports System.Net
Imports System.Net.Sockets
Imports System.Text
Module Client
    Sub Main(ByVal args As String())
        Try
            'command-line client:>      vccoopclient.exe <ip> <port> <nickname>
            'e.g                        vccoopclient.exe 192.168.1.2 53621 Zeyad_Ahmed
            Dim ip As String = args(0)
            Dim port As Int16 = args(1)
            Dim nickname As String = args(2)
            Dim SpecialCommand As String = args(3)
            'Intializing the Server
            Dim reciever As UdpClient  'the reciever
            Dim sender As UdpClient 'the sender
            Dim lpendp As IPEndPoint
            reciever = New UdpClient(port)
            lpendp = New IPEndPoint(0, port)
            sender = New UdpClient()
            reciever.Connect(ip, port)
            Dim bytes() As Byte = Encoding.Default.GetBytes(nickname & " connected from ip " & ip & ":" & port)
            sender.Send(bytes, bytes.Length)
            Threading.Thread.Sleep(50000)
        Catch ex As Exception
            Console.WriteLine(ex.Message)
        End Try

    End Sub
End Module
