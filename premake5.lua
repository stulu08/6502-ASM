include "./vendor/premake/premake_customization/solution_items.lua"
include "./vendor/premake/premake_customization/generate_doc.lua"
workspace "Emulator"
	startproject "6502"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	flags
	{
		"MultiProcessorCompile"
	}
	solution_items 
	{
		"**.lua",
		"README.md"
	}
	architecture "x86_64"

	filter "configurations:Debug"
		defines     "_DEBUG"

	filter "configurations:Release or Dist"
		defines     "NDEBUG"

	filter { "system:windows", "configurations:Dist", "toolset:not mingw" }
		flags		{ "LinkTimeOptimization" }
	
	filter "action:vs*"
		linkoptions {
			"/ignore:4006",
			"/ignore:4099"
		}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

buildToolsOutDir = "%{wks.location}/BuildTools/Out/"

Include = {}
Include["SBin"] = "%{wks.location}/S-Bin/Include"
Include["Emulator"] = "%{wks.location}/Emulator/src"
Include["Common"] = "%{wks.location}/BuildTools/Common/src"
Include["Compiler"] = "%{wks.location}/BuildTools/Compiler/src"
Include["Linker"] = "%{wks.location}/BuildTools/Linker/src"

include "Emulator"
include "S-Bin/Project.lua"
include "BuildTools"