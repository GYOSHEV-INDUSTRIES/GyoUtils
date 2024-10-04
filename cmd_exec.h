/*
In this file:
- functions to execute console commands from code, useful to automate build compilations scripts for c++ projects (and other)
*/

// Cogno: vcvarsall setups a TON of environment variables, if we want to automatically call vcvarsall we can do these things:
// - check if we have already everything setupped using these environment variables: Platform, VSCMD_ARG_TGT_ARCH, VSCMD_ARG_HOST_ARCH, VSCMD_ARG_app_plat, CommandPromptType
// - try to call vcvarsall x64
// - try to call vcvarsall x64 with the default path (C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat and maybe even 2022 or something)
// - call shell.bat 
// - ask the user to setup itself

// I don't really know what we should do... maybe change compiler? clang? gcc? I don't know...