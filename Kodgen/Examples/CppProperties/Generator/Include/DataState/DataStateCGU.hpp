#pragma once

#include <Kodgen/CodeGen/CodeGenUnit.h>
#include "Kodgen/CodeGen/GeneratedFile.h"

#include "DataStateCGM.hpp" //-->include DataPCG --> include DataStateCGE
#include "DataStateCGUS.hpp"

class DataStateCGU final : public kodgen::CodeGenUnit
{
private:
	//Make the addModule method taking a CodeGenModule private to replace it with a more restrictive method accepting MacroCodeGenModule only.
	using kodgen::CodeGenUnit::addModule;

	std::array<std::string, DataStateCGE::DSCodeLoc_COUNT>	generatedCodePerSection;

private:
	void ResetGeneratedCode(DataStateCGE& _env) noexcept
	{
		_env.isFirst = true;

		for (std::string& generatedCode : generatedCodePerSection)
		{
			generatedCode.clear();
		}
	}

protected:
	virtual void finalGenerateCode(kodgen::CodeGenEnv& env,
		std::function<void(kodgen::CodeGenEnv&,
							std::string&)>	generate)			noexcept override
	{
		//do nothing
		//no final calls for the CodeGenModule part of this CodeGenUnit
	}
		
	virtual void generateCodeForEntity(kodgen::EntityInfo const& entity, kodgen::CodeGenEnv& env,
		std::function<void(kodgen::EntityInfo const&,
			kodgen::CodeGenEnv&,
			std::string&)>		generate)	noexcept override
	{
		DataStateCGE& dsCGE = static_cast<DataStateCGE&>(env);

		for (kodgen::uint8 i = 0; i < DataStateCGE::DSCodeLoc_COUNT; ++i)
		{
			dsCGE.setCodeGenTarget(static_cast<DataStateCGE::DSCodeLoc>(i));

			generate(entity, env, generatedCodePerSection[i]);

			env.getLogger()->log("DataStateCGU::generateCodeForEntity() | code target : " + std::to_string(i) + " | generated code : " + generatedCodePerSection[i], kodgen::ILogger::ELogSeverity::Info);
		}
	}

	virtual void initialGenerateCode(kodgen::CodeGenEnv& env,
		std::function<void(kodgen::CodeGenEnv&,
							std::string&)>	generate)		noexcept override
	{
		//do nothing
		//no initial calls for the CodeGenModule part of this CodeGenUnit
	}

public:
	DataStateCGE* createCodeGenEnv() const noexcept override
	{
		return new DataStateCGE();
	}

	const DataStateCGUS* getSettings() const noexcept
	{
		return reinterpret_cast<DataStateCGUS const*>(settings);
	}

	void setSettings(DataStateCGUS const& cguSettings) noexcept
	{
		settings = reinterpret_cast<kodgen::CodeGenUnitSettings const*>(&cguSettings);
	}

	bool isUpToDate(std::filesystem::path const& sourceFile) const noexcept override
	{
		return false;//Always regenerate the DataState
	}

	void addModule(DataStateCGM& generationModule) noexcept
	{
		kodgen::CodeGenUnit::addModule(reinterpret_cast<kodgen::CodeGenModule&>(generationModule));
	}

	virtual bool preGenerateCode(kodgen::CodeGenEnv& env) noexcept override
	{
		if (kodgen::CodeGenUnit::preGenerateCode(env))
		{
			ResetGeneratedCode(static_cast<DataStateCGE&>(env));

			//Prepare the generated code for the DataState

			//Includes
			generatedCodePerSection[DataStateCGE::DSCodeLoc_Includes] = "#include <array>\n#include <variant>\n#include <vector>\n";

			//Data Vectors
			// --> nothing

			//Data Types Enum
			generatedCodePerSection[DataStateCGE::DSCodeLoc_EnumDataType] = "enum DataType : unsigned char \n{\n";

			//Typedefs
			generatedCodePerSection[DataStateCGE::DSCodeLoc_TypeDefs] = "typedef std::variant<";

			//GetDataVariantFuncDEF
			generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncDEF] = "template<DataType _DataType>\ninline DatasPtrVariant GetDatasPtrVar()\n{\n";

			//GetDataVariantFuncArr
			generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncArr] = "constexpr static std::array<GetDatasPtrVarFuncPtr, DataType_COUNT> getDatasPtrVarFuncPtrArr = {\n";

			//EmplaceBackDataFuncDEF
			generatedCodePerSection[DataStateCGE::DSCodeLoc_EmplaceBackDataFuncDEF] = "template<DataType _DataType>\ninline void EmplaceBackData()\n{\n";

			//EmplaceBackDataTemplateFuncs
			// --> nothing

			return true;
		}

		return false;
	}

	virtual bool postGenerateCode(kodgen::CodeGenEnv& env) noexcept override
	{
		const DataStateCGUS* castSettings = getSettings();
		kodgen::GeneratedFile generatedHeader(castSettings->getOutputDirectory() / (castSettings->getFileName() + castSettings->getHeaderFileExtension()));

		env.getLogger()->log("DataStateCGU::postGenerateCode() used to generate header at : " + generatedHeader.getPath().string(), kodgen::ILogger::ELogSeverity::Info);

		for (kodgen::uint8 i = 0; i < DataStateCGE::DSCodeLoc_COUNT; ++i)
		{
			env.getLogger()->log("DataStateCGU::postGenerateCode() | code target : " + std::to_string(i) + " | generated code : " + generatedCodePerSection[i], kodgen::ILogger::ELogSeverity::Info);
		}

		generatedHeader.writeLine("#pragma once\n");

		//Write includes
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_Includes]) + "\n");

		//Write namespace
		if (!castSettings->getNamespaceName().empty())
		{
			generatedHeader.writeLine("namespace " + castSettings->getNamespaceName() + "\n{");
		}

		//Write struct DataState
		generatedHeader.writeLine("struct " + castSettings->getClassName() + "\n{");

		//Write DataVectors
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_DataVectors]));

		//Write DataType enum
		generatedCodePerSection[DataStateCGE::DSCodeLoc_EnumDataType] += "DataType_COUNT\n};\n";
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_EnumDataType]));

		//Write custom type defs
		generatedCodePerSection[DataStateCGE::DSCodeLoc_TypeDefs].pop_back(); //Remove the last comma
		generatedCodePerSection[DataStateCGE::DSCodeLoc_TypeDefs] += "> DatasPtrVariant;"; //Finish the declaration
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_TypeDefs])); //Add to the header
		generatedHeader.writeLine("typedef DatasPtrVariant(DataState::* GetDatasPtrVarFuncPtr)();\n");

		//Write GetDatasPtrVariantFuncDEF
		generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncDEF] += "else { static_assert(false, \"Invalid DataType\"); }\n}\n";
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncDEF]));

		//Write EmplaceBackDataFuncDEF
		generatedCodePerSection[DataStateCGE::DSCodeLoc_EmplaceBackDataFuncDEF] += "else { static_assert(false, \"Invalid DataType\"); }\n}\n";
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_EmplaceBackDataFuncDEF]));

		//Write getDatasPtrVarFuncPtrArr
		generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncArr].pop_back(); //Remove the last \n
		generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncArr].pop_back(); //Remove the last comma
		generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncArr] += "};";
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncArr]));

		//Close struct DataState
		generatedHeader.writeLine("};");

		//Write EmplaceBackDataTemplateFuncs
		generatedHeader.writeLine(std::move(generatedCodePerSection[DataStateCGE::DSCodeLoc_EmplaceBackDataTemplateFuncs]));

		//Close namespace
		if (!castSettings->getNamespaceName().empty())
		{
			generatedHeader.writeLine("}");
		}

		return true;
	}
};