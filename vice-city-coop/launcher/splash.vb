'           Vice City CO-OP Launcher 0.1.0
'           Author(s) Zeyad Ahmed
'                     Athantos AKA NicusorN5
'           Copyrights (c) 2017-2018 VC:CO-OP Team
Imports System.Net
Imports System
Imports System.IO
Imports System.Diagnostics
Imports Microsoft.VisualBasic
Imports System.Windows.Forms


Public Class splash
    Private Sub PictureBox1_Click(sender As Object, e As EventArgs) Handles PictureBox1.Click
        MsgBox("Stop clicking cant you wait LEEEEL", MsgBoxStyle.Information, "WAIT")
    End Sub
    Sub ChooseVCPath()
        Dim fb_FolderBrowser As New FolderBrowserDialog
        fb_FolderBrowser.Description = "Choose the directory where Grand Theft Auto: Vice City exists"
        If fb_FolderBrowser.ShowDialog = DialogResult.OK Then
            Dim vc_dir As String
            vc_dir = fb_FolderBrowser.SelectedPath
            If File.Exists(vc_dir & "\gta-vc.exe") Then
                My.Settings.GameDir = vc_dir
            Else
                MsgBox("Cant find game executable, please try again.", MsgBoxStyle.Critical, "Cant find the game.")
                ChooseVCPath()
            End If

        End If
    End Sub
    Sub CheckPath()
        If File.Exists(My.Settings.GameDir & "\gta-vc.exe") Then

        Else
            MsgBox("Cant find game executable", MsgBoxStyle.Critical, "Cant find the game.")
            ChooseVCPath()
        End If

    End Sub
    Sub Initalize_Game()
        If File.Exists(My.Settings.GameDir & "\data\main.scm") Or File.Exists(My.Settings.GameDir & "\data\main.scm") Or File.Exists(My.Settings.GameDir & "\data\main.scm") Or File.Exists(My.Settings.GameDir & "\data\main.scm") Or File.Exists(My.Settings.GameDir & "\data\main.scm") Then
            File.Copy(Environment.CurrentDirectory + "\scm\main.scm", My.Settings.GameDir + "\data\main.scm", True)
            File.Copy(Environment.CurrentDirectory + "\asi\vccoop_main.asi", My.Settings.GameDir + "\vccoop_main.asi", True)
            File.Copy(Environment.CurrentDirectory + "\reso\LOADSC0.txd", My.Settings.GameDir + "\txd\LOADSC.txd", True)
            File.Copy(Environment.CurrentDirectory + "\dlls\d3d8.dll", My.Settings.GameDir + "\d3d8.dll", True)
            File.Copy(Environment.CurrentDirectory + "\dlls\rwd3d9.dll", My.Settings.GameDir + "\rwd3d9.dll", True)
        End If


    End Sub
    Sub Start_Game()
        Dim VC As New Process
        Dim ProcInfo As New ProcessStartInfo
        ProcInfo.WorkingDirectory = My.Settings.GameDir
        ProcInfo.FileName = ProcInfo.WorkingDirectory + "/gta-vc.exe"
        VC.StartInfo = ProcInfo
        VC.Start()
    End Sub
    Private Sub splash_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Timer1.Start()

    End Sub

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        Timer1.Stop()
        If My.Settings.GameDir = "null" Then
            ChooseVCPath()
            Start_Game()
            ProgressBar1.Value = 100
            Me.Close()
        Else

            CheckPath()
            ProgressBar1.Maximum = 100
            ProgressBar1.Minimum = 0
            ProgressBar1.Value = 50
            Initalize_Game()
            ProgressBar1.Value = 75
            Start_Game()
            ProgressBar1.Value = 100
            Me.Close()
        End If

    End Sub
End Class
