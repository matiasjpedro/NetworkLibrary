workspace "NetworkLibrary"
	architecture "x64"
	startproject "Sandbox"
	
	configurations
	{
		"Debug",
		"Release"
	}
	
outputdir= "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
project "NetworkLibrary"
	kind "StaticLib"
	language "C++"
	links
	{
		"vendor/WS2/lib/**.lib"
	}
	
	targetdir ("bin/" .. outputdir)
	objdir ("intermediate/" .. outputdir)
	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}
	
	includedirs
	{
		"vendor/WS2/include"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	links
	{
		"NetworkLibrary"
	}
	
	targetdir ("%{prj.name}/bin/" .. outputdir)
	objdir ("%{prj.name}/intermediate/" .. outputdir)
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"src/**.h",
		"src/**.cpp"
	}
	
	includedirs
	{
		"src/"
	}	
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
	
		
		