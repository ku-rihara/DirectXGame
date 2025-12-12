param(
    [Parameter(Mandatory=$true, Position=0)]
    [string]$NamespaceName,
    
    [Parameter(Mandatory=$false, Position=1)]
    [string]$TargetDirectory = "."
)

# 対象ディレクトリの検証
if (-not (Test-Path $TargetDirectory)) {
    Write-Error "指定されたディレクトリが見つかりません: $TargetDirectory"
    exit 1
}

# .h ファイルの処理
Get-ChildItem -Path $TargetDirectory -Filter "*.h" -Recurse | ForEach-Object {
    $filePath = $_.FullName
    Write-Host "処理中: $filePath"
    
    $lines = Get-Content $filePath
    
    if ($lines.Count -lt 1) {
        Write-Warning "スキップ: $filePath (ファイルが空です)"
        return
    }
    
    # 最初のクラスまたは構造体宣言を検索
    $classLineIndex = -1
    $templateLineIndex = -1
    $namespaceLineIndex = -1
    
    for ($i = 0; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        
        # コメント行をスキップ
        if ($line -match '^\s*//' -or $line -match '^\s*namespace\b') {
            continue
        }
        
        # namespace宣言を検知（より厳密なパターン）
        if ($namespaceLineIndex -eq -1 -and $line -match '^namespace\s*{') {
            $namespaceLineIndex = $i
            Write-Host "  namespace検出: $($line.Trim())"
        }
        
        # template宣言を検知
        if ($templateLineIndex -eq -1 -and $line -match '^\s*template\s*<') {
            $templateLineIndex = $i
            Write-Host "  テンプレート検出: $($line.Trim())"
        }
        
        # class または struct の宣言を検知
        # final キーワードにも対応
        if ($line -match '^\s*(class|struct)\s+[\w<>]+(\s+final)?\s*(:|\{|;|$)') {
            $classLineIndex = $i
            Write-Host "  クラス/構造体検出: $($line.Trim())"
            break
        }
    }
    
    if ($classLineIndex -eq -1) {
        Write-Warning "スキップ: $filePath (クラス/構造体宣言が見つかりません)"
        return
    }
    
    # namespaceを挿入する位置を決定
    # 優先順位: namespace > template > class
    $insertLineIndex = $classLineIndex
    
    if ($namespaceLineIndex -ne -1 -and $namespaceLineIndex -lt $classLineIndex) {
        $insertLineIndex = $namespaceLineIndex
        Write-Host "  namespace挿入位置: 既存namespace宣言の前 (行 $($insertLineIndex + 1))"
    } elseif ($templateLineIndex -ne -1 -and $templateLineIndex -lt $classLineIndex) {
        $insertLineIndex = $templateLineIndex
        Write-Host "  namespace挿入位置: テンプレート宣言の前 (行 $($insertLineIndex + 1))"
    } else {
        Write-Host "  namespace挿入位置: クラス宣言の前 (行 $($insertLineIndex + 1))"
    }
    
    # namespaceを追加
    $newLines = @()
    
    # 挿入位置より前の行を追加
    if ($insertLineIndex -gt 0) {
        $newLines += $lines[0..($insertLineIndex - 1)]
    }
    
    # namespaceを追加
    $newLines += "namespace $NamespaceName {"
    $newLines += ""
    
    # 挿入位置以降の行を追加
    $newLines += $lines[$insertLineIndex..($lines.Count - 1)]
    
    # 末尾に}; // namespace名を追加
    $newLines += ""
    $newLines += "}; // $NamespaceName"
    
    # ファイルに書き込み
    $newLines | Set-Content $filePath -Encoding UTF8
}

# .cpp ファイルの処理
Get-ChildItem -Path $TargetDirectory -Filter "*.cpp" -Recurse | ForEach-Object {
    $filePath = $_.FullName
    Write-Host "処理中: $filePath"
    
    $lines = Get-Content $filePath
    
    if ($lines.Count -lt 1) {
        Write-Warning "スキップ: $filePath (ファイルが空です)"
        return
    }
    
    # 対応する.hファイル名を取得（拡張子なし）
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($filePath)
    
    # .hファイルのinclude行を検索
    $includeLineIndex = -1
    for ($i = 0; $i -lt $lines.Count; $i++) {
        # #include "...BaseName.h" のパターンを検索
        if ($lines[$i] -match "^\s*#include\s+[<""].*$baseName\.h[>""]") {
            $includeLineIndex = $i
            Write-Host "  include検出: $($lines[$i].Trim())"
            break
        }
    }
    
    if ($includeLineIndex -eq -1) {
        Write-Warning "スキップ: $filePath (対応する.hファイルのincludeが見つかりません)"
        return
    }
    
    # include行の直後にusing namespaceを追加
    $newLines = @()
    
    # include行までを追加
    $newLines += $lines[0..$includeLineIndex]
    
    # using namespaceを追加
    $newLines += ""
    $newLines += "using namespace $NamespaceName;"
    
    # 残りの行を追加
    if ($includeLineIndex -lt $lines.Count - 1) {
        $newLines += $lines[($includeLineIndex + 1)..($lines.Count - 1)]
    }
    
    # ファイルに書き込み
    $newLines | Set-Content $filePath -Encoding UTF8
}

Write-Host "完了しました。"
