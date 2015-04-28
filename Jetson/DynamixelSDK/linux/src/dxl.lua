-- A solution contains projects, and defines the available configurations
solution "dxl"
	configurations { "Debug", "Release" }

	project "dxl"
		language "C++"
		kind     "SharedLib"
		files  { "../src/*.h",
				 "../src/*.cpp"}
		buildoptions { "-std=c++11" }
		links { "dl" }
		
		configuration { "Debug", "linux" }
			defines { "_DEBUG" }
			flags   { "Symbols", "SEH" }
			targetdir ("Debug")
			targetname ("dxl_debug")
	 
		configuration { "Release", "linux" }
			defines { "NDEBUG" }
			flags   { "Optimize", "SEH" }
			targetdir ("Release")
			targetname ("dxl")
