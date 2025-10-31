-- premake5.lua
workspace "PathTracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "PathTracer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "PathTracer"