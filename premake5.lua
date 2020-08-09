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
		"vendor/WS2/lib/**.lib",
		("vendor/UtilsLibrary/bin/" .. outputdir .. "/**.lib" )
	}
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs
	{
		"vendor/WS2/include",
		"vendor/UtilsLibrary/UtilsLibrary/src/"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
project "Chat"
	location "Chat"
	kind "ConsoleApp"
	language "C++"
	links
	{
		"vendor/WS2/lib/**.lib",
		"NetworkLibrary"
	}
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"NetworkLibrary/src",
		"vendor/UtilsLibrary/src/**.h",
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
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"NetworkLibrary/src",
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
	
		
		