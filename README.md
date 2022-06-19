# vsf.linux
Porting linux applications to VSF

## Prepare
1. PC
    - Windows 10 or later
    - Visual Studio 2022
    - npcap

## x86/x64
### submodules
1. vsf
    - PLOOC
    - zlib
    - mbedlts
2. ncurses
3. less
4. curl

## upy @ project/vc_upy
### submodules
1. vsf
    - PLOOC
1. upy

## aic8800m @ project/cmake/aic8800
### submodules
1. vsf
    - PLOOC
    - CMSIS
    - AIC8800M/vendor
    - mbedtls
    - btstack
2. curl

## aic8800m.meui @ project/cmake/aic8800.meui
1. vsf
    - PLOOC
    - CMSIS
    - AIC8800M/vendor
    - mbedtls
    - btstack
2. quickjs
3. quickjs/MEUI

## WebAssembly
### submodules
1. vsf
   - PLOOC
   - mbedtls
2. curl
### patches
1. /patch/vsf/webassembly.patch to /vsf

## WAMR
### submodules
1. vsf
   - PLOOC
2. wamr
