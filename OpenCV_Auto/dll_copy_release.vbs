Set FSO = CreateObject("Scripting.FileSystemObject")

Path = ".\"
If Wscript.Arguments.Count > 0 Then
	Path = Wscript.Arguments(0)
End If

DestFolder = "Release"
Set SrcFolder = FSO.GetFolder(Path & "..\OpenCV_install\x86\vc16\bin\")
Set CurrentFolder = FSO.GetFolder(Path)

' コピー先があるか？
Find = False

For Each Subfolder in CurrentFolder.SubFolders
    If Subfolder.name =DestFolder Then
        Find = true
        Exit For
    End IF
Next

' フォルダがないなら作成
If Find = False Then
    FSO.CreateFolder DestFolder
End If   


SearchCopyFile SrcFolder , CurrentFolder & "\" & DestFolder   

Sub SearchCopyFile(Folder, Destination)
    For Each File in Folder.Files 'Foler内のファイルを列挙する
        Fname = File.name
        
		' Ret = MsgBox (Fname, 1, "Find File")
		' If Ret > 1 Then Exit For

		If Len(FName) > 4 Then
			Ext = Mid(Fname,Len(FName)-3)
			' Ret = MsgBox ("Ext: " & Ext, 1)
			
			If StrComp(Ext,".dll") = 0 Then
				' dll 発見
				' d.dll を除外する
				Ext = Mid(Fname,Len(FName)-4)
				If StrComp(Ext,"d.dll") <> 0 Then
			        If FSO.FileExists(Destination & "\" & Fname) = False Then
						' Txt = Folder & "\" & Fname & " => " & Destination & "\" & Fname
						' Ret = MsgBox (Txt, 1, "Copy")

			            ' コピー
			            bResult = FSO.CopyFile( Folder & "\" & Fname, Destination & "\" & Fname, False )
			        End IF
		        End IF
	        End IF
		ENd If
    Next
    For Each Subfolder in Folder.SubFolders 'Foler内のフォルダを列挙する
        SearchCopyFile Subfolder , Destination'再帰呼び出し
    Next
End Sub

