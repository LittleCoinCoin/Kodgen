#include <Kodgen/Parsing/FileParser.h>
#include <Kodgen/CodeGen/CodeGenManager.h>
#include <Kodgen/CodeGen/EGenerationStrategies.h>
#include <Kodgen/CodeGen/Macro/MacroCodeGenUnit.h>
#include <Kodgen/CodeGen/Macro/MacroCodeGenUnitSettings.h>
#include <Kodgen/Misc/Filesystem.h>
#include <Kodgen/Misc/DefaultLogger.h>

#include "Macro/GetSetCGM.h"

void initCodeGenUnitSettings(fs::path const& workingDirectory, kodgen::MacroCodeGenUnitSettings& out_cguSettings)
{
	//All generated files will be located in WorkingDir/Include/Generated
	out_cguSettings.setOutputDirectory(workingDirectory / "Include" / "Generated");
	
	//Setup generated files name pattern
	out_cguSettings.setGeneratedHeaderFileNamePattern("##FILENAME##.h.h");
	out_cguSettings.setGeneratedSourceFileNamePattern("##FILENAME##.src.h");
	out_cguSettings.setClassFooterMacroPattern("##CLASSFULLNAME##_GENERATED");
	out_cguSettings.setHeaderFileFooterMacroPattern("File_##FILENAME##_GENERATED");
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

	//Setup code generation unit
	kodgen::MacroCodeGenUnit codeGenUnit;
	codeGenUnit.logger = &logger;

	kodgen::MacroCodeGenUnitSettings cguSettings;
	initCodeGenUnitSettings(workingDirectory, cguSettings);
	codeGenUnit.setSettings(cguSettings);

	//Add code generation modules
	GetSetCGM getSetCodeGenModule;
	codeGenUnit.addModule(getSetCodeGenModule);

	//Setup CodeGenManager
	kodgen::CodeGenManager codeGenMgr;
	codeGenMgr.logger = &logger;

	initCodeGenManagerSettings(workingDirectory, codeGenMgr.settings);

	//Kick-off code generation
	kodgen::CodeGenResult genResult = codeGenMgr.run(fileParser, codeGenUnit,
		kodgen::EGenerationStrategies::ForceReparseAll | kodgen::EGenerationStrategies::ForceRegenerateAll |
		kodgen::EGenerationStrategies::OneGenerateForEachFile);

	if (genResult.completed)
	{
		logger.log("Generation completed successfully in " + std::to_string(genResult.duration) + " seconds.");
	}
	else
	{
		logger.log("An error happened during code generation.", kodgen::ILogger::ELogSeverity::Error);
	}

	return EXIT_SUCCESS;
}