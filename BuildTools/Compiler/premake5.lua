project "Compiler"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	

	files
	{
		"src/**.cpp",
		"src/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	includedirs
	{
		"src",
		"%{Include.Common}",
		"%{Include.Emulator}",
		"%{Include.SBin}"
	}
	
	links {
		"Common",
		"Emulator",
		"S-Bin",
	}
	
	prebuildcommands {
		"{MKDIR} \"%{buildToolsOutDir}\"",
		"{MKDIR} \"%{buildToolsOutDir}/bin\"",
	}
	postbuildcommands {
		"{COPYDIR} \"%{cfg.targetdir}\" \"%{buildToolsOutDir}/bin\"",
	}
	
	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		defines "ST_DEBUG"
		runtime "Debug"
		optimize "off"
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Dist"
		defines "ST_DIST"
		runtime "Release"
		optimize "on"
		symbols "off"
