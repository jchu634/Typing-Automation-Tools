### Packaging Script
cmake --preset x64-release
cmake --build --preset x64-release

cd out/build/x64-release
7z a -tzip ../../../type-clipboard.zip type-clipboard.exe key_sim_lib.dll
7z a -tzip ../../../type-at-wpm.zip type-at-wpm.exe key_sim_lib.dll
cd ../../../