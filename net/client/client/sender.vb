Imports System.Net
Imports System.Net.Sockets
Imports System.Text

Public Class sender
    Dim sender As UdpClient = New UdpClient()
    Public Sub Intialize(ByVal ip As String, ByVal port As Int32)
        Dim IPEnd As IPEndPoint = New IPEndPoint(ip, port)
    End Sub
    Sub connect(ByVal ip As String, ByVal port As Int32)
        Try
            sender.Connect(ip, port)
        Catch ex As Exception
            log("[ERROR]" & ex.Message)
        End Try
    End Sub


    Sub log(ByVal text As String)
        Console.WriteLine(text)
    End Sub
    Sub Intialize()

    End Sub
    Sub Send(ByVal text As String)
        Try
            Dim bytes() As Byte = Encoding.Default.GetBytes(text)
            sender.Send(bytes, bytes.Length)
        Catch ex As Exception
            log("[ERROR]" & ex.Message)
        End Try
    End Sub

End Class
