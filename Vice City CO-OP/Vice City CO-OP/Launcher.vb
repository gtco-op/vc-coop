Public Class Launcher
    Public Sub LaunchGame(ByVal location As String)
        Process.Start(location)
    End Sub
    Public Sub LaunchClient(ByVal ip As String, ByVal port As Int32, ByVal nickname As String)
        Process.Start(".../client.exe", ip & port & nickname)
    End Sub
End Class
