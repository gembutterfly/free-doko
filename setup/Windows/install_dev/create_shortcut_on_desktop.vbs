Dim ArgObj,arg_name,arg_command,arg_working_dir,arg_title,arg_icon
Set ArgObj = WScript.Arguments

arg_name = ArgObj(0)
arg_command = ArgObj(1)
arg_working_dir = ArgObj(2)
arg_title = ArgObj(3)
arg_icon = ArgObj(4)

Dim objShortcut
Dim objShell
Set objShell= WScript.CreateObject("Wscript.Shell")
strLPfad=objShell.SpecialFolders("Desktop")
Set objShortcut=objShell.CreateShortcut(strLPfad & "\" & arg_name & ".lnk")
objShortcut.TargetPath= arg_command
objShortcut.WorkingDirectory= arg_working_dir
objShortcut.Description= arg_title
objShortcut.IconLocation= arg_icon
objShortcut.Save
