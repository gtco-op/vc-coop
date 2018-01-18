Imports System.Net

Public Class splash
    Private Sub PictureBox1_Click(sender As Object, e As EventArgs) Handles PictureBox1.Click
        MsgBox("Stop clicking cant you wait LEEEEL")
    End Sub
    Sub CheckUpdate()
        Dim version As String = New System.Net.WebClient().DownloadString("http://master.vicecityco-op.ml/updates/currentupdate")
        If version <> My.Settings.Version Then
            'download new version
        End If
    End Sub
    Private Sub splash_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        If My.Settings.FirstStart = False Then
            setting.ShowDialog()
        Else
            CheckUpdate()
        End If
    End Sub
End Class
