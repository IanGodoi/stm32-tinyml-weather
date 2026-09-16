$porta = New-Object System.IO.Ports.SerialPort "COM7",115200,"None",8,"One"
$porta.Open()

Write-Host "Conectado à COM7 em 115200 baud"
Write-Host "Ctrl+C para sair"

try {
    while ($true) {
        if ($porta.BytesToRead -gt 0) {
            Write-Host -NoNewline $porta.ReadExisting()
        }
        Start-Sleep -Milliseconds 50
    }
}
finally {
    $porta.Close()
}