#include <Kodgen/Parsing/FileParser.h>
#include <Kodgen/CodeGen/CodeGenManager.h>
#include <Kodgen/CodeGen/EGenerationStrategies.h>
#include <Kodgen/CodeGen/Macro/MacroCodeGenUnit.h>
#include <Kodgen/CodeGen/Macro/MacroCodeGenUnitSettings.h>
#include <Kodgen/Misc/Filesystem.h>
#include <Kodgen/Misc/DefaultLogger.h>

#include "Macro/GetSetCGM.h"
#include "DataState/DataStateCGU.hpp"
#include "DataState/DataStateCGM.hpp"


void initCodeGenUnitSettingsOneGenForEach(fs::path const& workingDirectory, kodgen::MacroCodeGenUnitSettings& out_cguSettings)
{
	//All generated files will be located in WorkingDir/Include/Generated
	out_cguSettings.setOutputDirectory(workingDirectory / "Include" / "Generated");
	
	//Setup generated files name pattern
	out_cguSettings.setGeneratedHeaderFileNamePattern("##FILENAME##.h.h");
	out_cguSettings.setGeneratedSourceFileNamePattern("##FILENAME##.src.h");
	out_cguSettings.setClassFooterMacroPattern("##CLASSFULLNAME##_GENERATED");
	out_cguSettings.setHeaderFileFooterMacroPattern("File_##FILENAME##_GENERATED");
}

void initCodeGenUnitSettingsOneGenForAll(fs::path const& workingDirectory, DataStateCGUS& out_cguSettings)
{
	//All generated files will be located in WorkingDir/Include/Generated
	out_cguSettings.setOutputDirectory(workingDirectory / "Include" / "Generated");

	//Setup generated file options
	out_cguSettings.setNamespaceName("");//same as default, just to show how to set it
	out_cguSettings.setClassName("DataState");//same as default, just to show how to set it
	out_cguSettings.setFileName("DataState");//same as default, just to show how to set it
	out_cguSettings.setHeaderFileExtension(".h.hpp");//same as default, just to show how to set it
}

void initCodeGenManagerSettings(fs::path const& workingDirectory, kodgen::CodeGenManagerSettings& out_generatorSettings)
{
	fs::path includeDirectory	= workingDirectory / "Include";
	fs::path generatedDirectory	= includeDirectory / "Generated";

	//Parse WorkingDir/...
	out_generatorSettings.addToProcessDirectory(includeDirectory);

	//Ignore generated files...
	out_generatorSettings.addIgnoredDirectory(generatedDirectory);

	//Only parse .h files
	out_generatorSettings.addSupportedFileExtension(".h");
}

bool initParsingSettings(kodgen::ParsingSettings& parsingSettings)
{
	//We abort parsing if we encounter a single error while parsing
	parsingSettings.shouldAbortParsingOnFirstError = true;

	//Each property will be separed by a ,
	parsingSettings.propertyParsingSettings.propertySeparator = ',';

	//Subproperties are surrounded by []
	parsingSettings.propertyParsingSettings.argumentEnclosers[0] = '[';
	parsingSettings.propertyParsingSettings.argumentEnclosers[1] = ']';

	//Each subproperty will be separed by a ,
	parsingSettings.propertyParsingSettings.argumentSeparator = ',';

	//Define the macros to use for each entity type
	parsingSettings.propertyParsingSettings.namespaceMacroName	= "KGNamespace";
	parsingSettings.propertyParsingSettings.classMacroName		= "KGClass";
	parsingSettings.propertyParsingSettings.structMacroName		= "KGStruct";
	parsingSettings.propertyParsingSettings.fieldMacroName		= "KGVariable";
	parsingSettings.propertyParsingSettings.fieldMacroName		= "KGField";
	parsingSettings.propertyParsingSettings.functionMacroName	= "KGFunction";
	parsingSettings.propertyParsingSettings.methodMacroName		= "KGMethod";
	parsingSettings.propertyParsingSettings.enumMacroName		= "KGEnum";
	parsingSettings.propertyParsingSettings.enumValueMacroName	= "KGEnumVal";

	//This is setuped that way for CI tools only
	//In reality, the compiler used by the user machine running the generator should be set.
	//It has nothing to see with the compiler used to compile the generator.
#if defined(__GNUC__)
	return parsingSettings.setCompilerExeName("g++");
#elif defined(__clang__)
	return parsingSettings.setCompilerExeName("clang++");
#elif defined(_MSC_VER)
	return parsingSettings.setCompilerExeName("msvc");
#else
	return false;	//Unsupported compiler
#endif
}

int main(int argc, char** argv)
{
	kodgen::DefaultLogger logger;

	if (argc <= 1)
	{
		logger.log("No working directory provided as first program argument", kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	fs::path workingDirectory = argv[1];

	if (!fs::is_directory(workingDirectory))
	{
		logger.log("Provided working directory is not a directory or doesn't exist", kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	logger.log("Working Directory: " + workingDirectory.string());

	//Setup FileParser
	kodgen::FileParser fileParser;
	fileParser.logger = &logger;

	if (!initParsingSettings(fileParser.getSettings()))
	{
		logger.log("Compiler could not be set because it is not supported on the current machine or vswhere could not be found (Windows|MSVC only).", kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	//Setup CodeGenManager
	kodgen::CodeGenManager codeGenMgr;
	codeGenMgr.logger = &logger;

	initCodeGenManagerSettings(workingDirectory, codeGenMgr.settings);
	
	//Setup code generation unit for one generate for each file
	kodgen::MacroCodeGenUnit macroCodeGenUnit;
	macroCodeGenUnit.logger = &logger;

	kodgen::MacroCodeGenUnitSettings cguSettings;//Add code generation unit settings
	initCodeGenUnitSettingsOneGenForEach(workingDirectory, cguSettings);
	macroCodeGenUnit.setSettings(cguSettings);
	
	GetSetCGM getSetCodeGenModule;//Add code generation modules
	macroCodeGenUnit.addModule(getSetCodeGenModule);

	//Setup code generation unit for one generate for all files
	DataStateCGU dataStateCodeGenUnit;
	dataStateCodeGenUnit.logger = &logger;

	DataStateCGUS dataStateCodeGenUnitSettings;//Add code generation unit settings
	initCodeGenUnitSettingsOneGenForAll(workingDirectory, dataStateCodeGenUnitSettings);
	dataStateCodeGenUnit.setSettings(dataStateCodeGenUnitSettings);

	DataStateCGM dataStateCodeGenModule;//Add code generation modules
	dataStateCodeGenUnit.addModule(dataStateCodeGenModule);

	//Kick-off code generation
	kodgen::EGenerationStrategies reparseAndRegenAll = kodgen::EGenerationStrategies::ForceReparseAll | kodgen::EGenerationStrategies::ForceRegenerateAll;
	kodgen::CodeGenResult genResultOneGenForEach = codeGenMgr.run(fileParser, macroCodeGenUnit, reparseAndRegenAll |
		kodgen::EGenerationStrategies::OneGenerateForEachFile);
	kodgen::CodeGenResult genResultOneGenForAll = codeGenMgr.run(fileParser, dataStateCodeGenUnit, reparseAndRegenAll |
		kodgen::EGenerationStrategies::OneGenerateForAllFiles);

	if (genResultOneGenForEach.completed && genResultOneGenForAll.completed)
	{
		logger.log("Generation completed successfully in " +
			std::to_string(genResultOneGenForEach.duration) +
			std::to_string(genResultOneGenForAll.duration) +
			" seconds.");
	}
	else
	{
		logger.log("An error happened during code generation.", kodgen::ILogger::ELogSeverity::Error);
	}

	return EXIT_SUCCESS;
}