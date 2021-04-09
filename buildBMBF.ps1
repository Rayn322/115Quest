# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/lib115Mod.so", "./libs/arm64-v8a/libbeatsaber-hook_1_2_6.so", "./bmbfmod.json" -DestinationPath "./115Mod_v1.0.0.zip" -Update
}
