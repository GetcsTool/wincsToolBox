Add-Type -AssemblyName System.Windows.Forms,System.Drawing
$bounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
$bmp = New-Object System.Drawing.Bitmap($bounds.Width, $bounds.Height)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen(0, 0, 0, 0, $bmp.Size)
$path = "E:\Uninstall\财神科技\test_screenshot.png"
$bmp.Save($path)
$g.Dispose()
$bmp.Dispose()
Write-Output "Screenshot saved: $path ($($bounds.Width) x $($bounds.Height))"
