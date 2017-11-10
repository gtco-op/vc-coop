'---------------------------------------------------
'   File Author(s): Zeyad_Ahmed
'   Copyrights (c) 2017 VC:CO-OP Team
'               www.vicecityco-op.ml
'---------------------------------------------------
Imports System.Text, System.IO, System.Net, System.Net.Sockets

Public Class reciever
    Dim sock As UdpClient
    Dim port As Int32 = 89
    Dim lpendp As IPEndPoint
    Sub Intialize(ByVal port As Int32)
        sock = New UdpClient(port)
        lpendp = New IPEndPoint(0, port)
    End Sub
    Sub Log(ByVal text As String)
        Console.WriteLine(text)
    End Sub
    Sub Recieve()
        Dim re_text As String
        Dim re_bytes() As Byte
        Try
            re_bytes = sock.Receive(lpendp)
            re_text = Encoding.Default.GetString(re_bytes)
            Console.WriteLine(re_text)
        Catch ex As Exception
            Log("[ERROR] " & ex.Message)
        End Try
    End Sub
End Class
