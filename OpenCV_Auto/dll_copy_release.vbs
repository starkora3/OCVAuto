Set FSO = CreateObject("Scripting.FileSystemObject")

Path = ".\"
If Wscript.Arguments.Count > 0 Then
	Path = Wscript.Arguments(0)
End If

DestFolder = "Release"
Set SrcFolder = FSO.GetFolder(Path & "..\OpenCV_install\x86\vc16\bin\")
Set CurrentFolder = FSO.GetFolder(Path)

' �R�s�[�悪���邩�H
Find = False

For Each Subfolder in CurrentFolder.SubFolders
    If Subfolder.name =DestFolder Then
        Find = true
        Exit For
    End IF
Next

' �t�H���_���Ȃ��Ȃ�쐬
If Find = False Then
    FSO.CreateFolder DestFolder
End If   


SearchCopyFile SrcFolder , CurrentFolder & "\" & DestFolder   

Sub SearchCopyFile(Folder, Destination)
    For Each File in Folder.Files 'Foler���̃t�@�C����񋓂���
        Fname = File.name
        
		' Ret = MsgBox (Fname, 1, "Find File")
		' If Ret > 1 Then Exit For

		If Len(FName) > 4 Then
			Ext = Mid(Fname,Len(FName)-3)
			' Ret = MsgBox ("Ext: " & Ext, 1)
			
			If StrComp(Ext,".dll") = 0 Then
				' dll ����
				' d.dll �����O����
				Ext = Mid(Fname,Len(FName)-4)
				If StrComp(Ext,"d.dll") <> 0 Then
			        If FSO.FileExists(Destination & "\" & Fname) = False Then
						' Txt = Folder & "\" & Fname & " => " & Destination & "\" & Fname
						' Ret = MsgBox (Txt, 1, "Copy")

			            ' �R�s�[
			            bResult = FSO.CopyFile( Folder & "\" & Fname, Destination & "\" & Fname, False )
			        End IF
		        End IF
	        End IF
		ENd If
    Next
    For Each Subfolder in Folder.SubFolders 'Foler���̃t�H���_��񋓂���
        SearchCopyFile Subfolder , Destination'�ċA�Ăяo��
    Next
End Sub

