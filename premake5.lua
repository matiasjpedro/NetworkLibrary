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
	location "NetworkLibrary"
	kind "StaticLib"
	language "C++"
	links
	{
		"vendor/WS2/lib/**.lib"
	}
	
	targetdir ("%{prj.name}/bin/" .. outputdir)
	objdir ("%{prj.name}/intermediate/" .. outputdir)
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
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
	location "NetworkLibrary"
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
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"NetworkLibrary/src/"
	}	
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
	
		
		