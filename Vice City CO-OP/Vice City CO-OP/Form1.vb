'=================================================
'        Vice City CO-OP 0.1.0 User Client
'=================================================
'   Author(s): Zeyad_Ahmed
'   Copyrights (c) VC:CO-OP Team
'   Licence GNU v3
Public Class Form1
    Dim Launcher As New Launcher

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim x As New FolderBrowserDialog
        If x.ShowDialog = Windows.Forms.DialogResult.OK Then
            Dim game As String
            game = x.SelectedPath
            If IO.File.Exists(game & "/gta-vc.exe") Then
                TextBox1.Text = game & "/gta-vc.exe"
            Else
                MsgBox("Cant find the game", MsgBoxStyle.Critical, "Error")
            End If
        End If
    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        Launcher.LaunchGame(TextBox1.Text)

    End Sub

    Private Sub TextBox2_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles TextBox2.TextChanged
        If TextBox2.Text.Contains("^") Then
            TextBox2.Text.Replace("^", "")
        End If
    End Sub
End Class
