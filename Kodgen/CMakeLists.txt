cmake_minimum_required(VERSION 3.13.5)

project(KodgenLibrary)

# Setup output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)	# .dll / .so
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)	# .lib / .a
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)	# .exe

set(KodgenTargetLibrary Kodgen)

add_library(${KodgenTargetLibrary}
				STATIC
					"Source/Properties/PropertyParsingSettings.cpp"
					
					"Source/InfoStructures/EntityInfo.cpp"
					"Source/InfoStructures/NamespaceInfo.cpp"
					"Source/InfoStructures/VariableInfo.cpp"
					"Source/InfoStructures/FieldInfo.cpp"
					"Source/InfoStructures/FunctionInfo.cpp"
					"Source/InfoStructures/MethodInfo.cpp"
					"Source/InfoStructures/StructClassInfo.cpp"
					"Source/InfoStructures/NestedStructClassInfo.cpp"
					"Source/InfoStructures/EnumInfo.cpp"
					"Source/InfoStructures/NestedEnumInfo.cpp"
					"Source/InfoStructures/EnumValueInfo.cpp"
					"Source/InfoStructures/TypeInfo.cpp"
					"Source/InfoStructures/StructClassTree.cpp"
					"Source/InfoStructures/TemplateParamInfo.cpp"
	
					"Source/Parsing/ParsingError.cpp"
					"Source/Parsing/PropertyParser.cpp"
					"Source/Parsing/EntityParser.cpp"
					"Source/Parsing/NamespaceParser.cpp"
					"Source/Parsing/ClassParser.cpp"
					"Source/Parsing/VariableParser.cpp"
					"Source/Parsing/FieldParser.cpp"
					"Source/Parsing/FunctionParser.cpp"
					"Source/Parsing/MethodParser.cpp"
					"Source/Parsing/EnumParser.cpp"
					"Source/Parsing/EnumValueParser.cpp"
					"Source/Parsing/FileParser.cpp"
					"Source/Parsing/ParsingSettings.cpp"

					"Source/Parsing/ParsingResults/ParsingResultBase.cpp"
					
					"Source/Misc/EAccessSpecifier.cpp"
					"Source/Misc/Helpers.cpp"
					"Source/Misc/DefaultLogger.cpp"
					"Source/Misc/CompilerHelpers.cpp"
					"Source/Misc/System.cpp"
					"Source/Misc/Filesystem.cpp"
					"Source/Misc/TomlUtility.cpp"
					"Source/Misc/Settings.cpp"
	
					"Source/CodeGen/CodeGenUnit.cpp"
					"Source/CodeGen/CodeGenResult.cpp"
					"Source/CodeGen/CodeGenManager.cpp"
					"Source/CodeGen/GeneratedFile.cpp"
					"Source/CodeGen/CodeGenModule.cpp"
					"Source/CodeGen/CodeGenUnitSettings.cpp"
					"Source/CodeGen/CodeGenManagerSettings.cpp"
					"Source/CodeGen/CodeGenHelpers.cpp"
					"Source/CodeGen/PropertyCodeGen.cpp"
					"Source/CodeGen/ICodeGenerator.cpp"

					"Source/CodeGen/Macro/MacroCodeGenUnit.cpp"
					"Source/CodeGen/Macro/MacroCodeGenUnitSettings.cpp"
					"Source/CodeGen/Macro/MacroCodeGenerator.cpp"
					"Source/CodeGen/Macro/MacroCodeGenModule.cpp"
					"Source/CodeGen/Macro/MacroPropertyCodeGen.cpp"

					"Source/Threading/ThreadPool.cpp"
					"Source/Threading/TaskBase.cpp"
				)

if (MSVC)

	# Setup warning level
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")	# Remove the default flag /W3
	target_compile_options(${KodgenTargetLibrary} PRIVATE /W4 /MP)

else ()	# Clang or GCC

	target_compile_options(${KodgenTargetLibrary} PRIVATE -Wall -Wextra -Wpedantic)

endif()

# Setup compilation definitions
if (KODGEN_DEV)

	target_compile_definitions(${KodgenTargetLibrary} PRIVATE KODGEN_DEV=1)

	if (NOT MSVC) # Clang or GCC
		target_compile_options(${KodgenTargetLibrary} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
		target_link_options(${KodgenTargetLibrary} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
	endif()

endif()

# Setup language requirements
if(MSVC)
	if (MSVC_VERSION GREATER_EQUAL 1914)	# Visual Studio 2017 15.7 or later:
		message("Visual Studio 2017 15.7 or later detected: enabling /Zc:__cplusplus to force it to match value of /std")
		target_compile_options(${KodgenTargetLibrary} PUBLIC /Zc:__cplusplus)
	endif()
endif()
target_compile_features(${KodgenTargetLibrary} PUBLIC cxx_std_17)

# Setup include directories
target_include_directories(${KodgenTargetLibrary}
							PUBLIC
								Include
								ThirdParty/Include
							)

# Setup link directories
target_link_directories(${KodgenTargetLibrary}
							PUBLIC
								ThirdParty/x64/Static
								ThirdParty/x64/Shared
							)

# Link to required libraries
find_package(Threads REQUIRED)
target_link_libraries(${KodgenTargetLibrary}
						INTERFACE
							$<$<AND:$<CXX_COMPILER_ID:Clang,AppleClang>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>	#filesystem	pre Clang-9
							$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>					#filesystem	pre GCC-9
							clang
							${CMAKE_THREAD_LIBS_INIT}
						)

# Copy libclang shared library & vswhere to the bin folder
if (WIN32)
	add_custom_command(	TARGET ${KodgenTargetLibrary} POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy
						${PROJECT_SOURCE_DIR}/ThirdParty/x64/Shared/libclang.dll
						$<IF:$<BOOL:${MSVC}>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/libclang.dll,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libclang.dll>
					)

	add_custom_command(	TARGET ${KodgenTargetLibrary} POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy
						${PROJECT_SOURCE_DIR}/ThirdParty/x64/Bin/vswhere.exe
						$<IF:$<BOOL:${MSVC}>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/vswhere.exe,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vswhere.exe>
					)
endif()

add_subdirectory(Examples)
add_subdirectory(Tests)