BeforeAll {
    function Convert-PathToMsys {
        param(
            [string]$Path
        )

        if ($Path -match '^([A-Za-z]):') {
            $drive = $matches[1].ToLower()
            return "/$drive" + ($Path.Substring(2) -replace '\\', '/')
        }

        if ($Path -match '^\\(?!\\)') {
            $drive = (Get-Location).Drive.Name.TrimEnd(':').ToLower()
            return "/$drive" + ($Path -replace '\\', '/')
        }

        return ($Path -replace '\\', '/')
    }
}

Describe "Convert-PathToMsys" {
    It "converts drive-letter paths" {
        Convert-PathToMsys 'C:\tmp\Qt' | Should -Be '/c/tmp/Qt'
    }

    It "converts root-relative paths using current drive" {
        $drive = (Get-Location).Drive.Name.TrimEnd(':').ToLower()
        Convert-PathToMsys '\tmp\Qt' | Should -Be "/$drive/tmp/Qt"
    }

    It "returns POSIX paths unchanged" {
        Convert-PathToMsys '/c/tmp/Qt' | Should -Be '/c/tmp/Qt'
    }
}
