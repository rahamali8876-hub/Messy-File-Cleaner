### inatallation packeges ( on MINGW TERMINAL )
        pacman -S mingw-w64-x86_64-libjpeg-turbo
        pacman -S mingw-w64-x86_64-make
        pacman -S mingw-w64-x86_64-cppcheck
        pacman -S libjpeg-turbo
        pacman -S make

### REMOVE PACKEGES
        pacman -R mingw-w64-x86_64-make

### filter installed packeges
        pacman -Q
        pacman -Q | grep make
        pacman -Q | grep jpeg
        mingw-w64-x86_64-make 4.4.1-4
        mingw-w64-x86_64-libjpeg-turbo 3.1.3-1

### Find DLL Inside FILES
        nm -g build/img2pdf.dll | grep img2pdf_

### 🔥 How To Build (Windows MinGW)

        rmdir /s /q build
        cmake -S . -B build -G "MinGW Makefiles"
        cmake --build build

### Testing VIOLETION ( FORMATING / STATIC ANALYSIS / LAYER CHECKS / ETC. )
        cmake --build build --target layer_checks
        cmake --build build --target format
        cmake --build build --target format_check
        cmake --build build --target static_analysis ( After building only )
        cmake --build build --target cppcheck
        cmake --build build --target docs
        cmake --build build --target coverage
        cmake --build build --target graph
        cmake --build build --target all_quality
        cmake --build build --target distclean
        

### Remove build artifacts:
        rmdir /S /Q build

### 🔥 Debug Build
        cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
        cmake --build .

### 🔥 Release Build
        cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
        cmake --build .
