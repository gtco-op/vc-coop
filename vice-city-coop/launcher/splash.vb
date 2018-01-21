Imports System.Net

Public Class splash
    Private Sub PictureBox1_Click(sender As Object, e As EventArgs) Handles PictureBox1.Click
        MsgBox("Stop clicking cant you wait LEEEEL", MsgBoxStyle.Information, "WAIT")
    End Sub

    Private Sub splash_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Timer1.Start()
    End Sub

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        main.Show()
        Timer1.Stop()
        Me.Close()
    End Sub
End Class
