project "fmt"
	location "../build/%{prj.name}"
	kind "StaticLib"
	language "C++"
	toolset "gcc"
	cppdialect "C++17"
	staticruntime "on"
	targetdir ("../bin/%{prj.name}/%{cfg.buildcfg}")
	objdir ("../bin-init/%{prj.name}/%{cfg.buildcfg}")
	files{
		"include/**.h",
		"include/**.hpp",
		"src/**.cc",
		"src/**.cpp"
	}
	includedirs{
		"include"
	}
	filter "configurations:debug"
		defines {"DEBUG"} 
		runtime "Debug"
		symbols "on"
	filter "configurations:release"
		defines {"NDEBUG"} 
		optimize "on"
		runtime "Release"

