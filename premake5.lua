workspace "NetworkLibrary"
	architecture "x64"
	startproject "Chat"
	
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
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"vendor/WS2/include"
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
		"vendor/WS2/include",
		"NetworkLibrary/src"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
project "UnitTesting"
	location "UnitTesting"
	kind "ConsoleApp"
	language "C++"
	links
	{
		"Chat",
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
		"Chat/src"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
	
		
		