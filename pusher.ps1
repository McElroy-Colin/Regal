# Powershell script

# Automatically push changes to github.
# "-f" argument performs a force push.
# Another argument command provides a custom description.

$curr_date = Get-Date -Format "MM-dd"
$curr_time = Get-Date -Format "HH:mm tt"
$force_bool = $false
$desc_bool = $false
$add_com = git add .
if ($args.Count -eq 0) {
    $commit_com = git commit -m "Automatic commit on $curr_date at $curr_time."
} elseif ($args.Count -lt 3) {
    foreach ($arg in $args) {
        if (($arg -eq "-f") -and (-not $force_bool)) {
            $force_bool = $true
        } elseif (($arg -is [string]) -and (-not $desc_bool)) {
            $desc_bool = $true
            $desc = $arg
        }
    }
    if ($desc_bool) {
        $commit_com = git commit -m "$desc`nCommit on $curr_date at $curr_time."
    } else {
        $commit_com = git commit -m "Automatic commit on $curr_date at $curr_time."
    }
} else {
    Write-Host "Too many arguments passed."
    exit 1
}
if ($force_bool) {
    $push_com = git push origin main -f
} else {
    $push_com = git push origin main
}


Write-Host "`nAdding ./B-plus-plus to github..."
$add_com
Write-Host "`nCommitting changes...`n"
$commit_com
Write-Host "`nPushing commit...`n"
$push_com
Write-Host "Done!`n"