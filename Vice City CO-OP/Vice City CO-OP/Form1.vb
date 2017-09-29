Public Class Form1

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim x As New FolderBrowserDialog
        If x.ShowDialog = Windows.Forms.DialogResult.OK Then
            Dim game As String
            game = x.SelectedPath
            If IO.File.Exists(game & "gta-vc.exe") Then
                TextBox1.Text = game & "gta-vc.exe"
            Else
                MsgBox("Cant find the game", MsgBoxStyle.Critical, "Error")
            End If
        End If
    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        Process.Start("...\vccoopclient", TextBox2.Text) 'that will launch the client with the nickname as an argument

    End Sub
End Class
