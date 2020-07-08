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
		"UtilsLibrary"
	}
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"vendor/glm/glm/fwd.hpp"
	}
	
	includedirs
	{
		"vendor/WS2/include",
		"vendor/glm",
		"UtilsLibrary/src"
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
		"vendor/glm",
		"vendor/WS2/include"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		
project "UtilsLibrary"
	location "UtilsLibrary"
	kind "StaticLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
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
		
	
		
		