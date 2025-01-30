#pragma once

/*!
@file DataPCG.hpp
@brief DataPCG class declaration.
@details DataPCG is a property code generation class that reacts to the Data
		 property in user code structs or classes and sends back up information.
*/

#include <Kodgen/CodeGen/PropertyCodeGen.h>

#include "DataStateCGE.hpp"

class DataPCG : public kodgen::PropertyCodeGen
{
private:

	bool generateCodeForIncludes(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForIncludes() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		std::string sourceFileExt = env.getFileParsingResult()->parsedFile.extension().string();

		inout_result += "#include \"" + entity.name + sourceFileExt + "\"\n";

		return true;
	}

	bool generateCodeForDataVectors(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForDataVectors() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		inout_result += "std::vector<" + entity.getFullName() + "> data_" + entity.name + ";\n";

		return true;
	}

	bool generateCodeForEnumDataType(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForEnumDataType() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		inout_result += "DataType_" + entity.name + ",\n";

		return true;
	}

	bool generateCodeForTypeDefs(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForTypeDefs() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		inout_result += "std::vector<" + entity.getFullName() + ">*,";

		return true;
	}

	bool generateCodeForGetDatasPtrVariantFuncDEF(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForGetDatasPtrVariantFuncDEF() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		if (env.getIsFirst())
		{
			inout_result += "if constexpr (_DataType == DataType_" + entity.name + ") { return &data_" + entity.name + "; }\n";

			//env.setIsFirst(false); //We don't set it to false here because we still need the value to be true for generateCodeForEmplaceBackDataFuncDEF
		}
		else
		{
			inout_result += "else if constexpr (_DataType == DataType_" + entity.name + ") { return &data_" + entity.name + "; }\n";
		}

		return true;
	}

	bool generateCodeForGetDatasPtrVariantFuncArr(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForGetDataVariantFuncArr() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		inout_result += "&DataState::template GetDatasPtrVar<DataType_" + entity.name + ">,\n";

		return true;

	}

	bool generateCodeForEmplaceBackDataFuncDEF(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForEmplaceBackDataFuncDEF() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		if (env.getIsFirst())
		{
			inout_result += "if constexpr (_DataType == DataType_" + entity.name + ") { data_" + entity.name + ".emplace_back(); }\n";
			env.setIsFirst(false);
		}
		else
		{
			inout_result += "else if constexpr (_DataType == DataType_" + entity.name + ") { data_" + entity.name + ".emplace_back(); }\n";
		}

		return true;

	}

	bool generateCodeForEmplaceBackDataTemplateFuncs(kodgen::EntityInfo const& entity,
		kodgen::Property const& property, kodgen::uint8 propertyIndex,
		DataStateCGE& env, std::string& inout_result) noexcept
	{
		env.getLogger()->log("DataPCG::generateCodeForEmplaceBackDataTemplateFuncs() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);

		inout_result += "template void DataState::EmplaceBackData<DataState::DataType_" + entity.name + ">();\n";

		return true;

	}

public:
	DataPCG() noexcept :
		kodgen::PropertyCodeGen("Data", kodgen::EEntityType::Class | kodgen::EEntityType::Struct)
	{

	}

	virtual bool initialGenerateCode(kodgen::CodeGenEnv& env,
		std::string& inout_result) noexcept override final
	{
		env.getLogger()->log("DataPCG::initialGenerateCode() called", kodgen::ILogger::ELogSeverity::Info);
		return true;
	}

	virtual bool finalGenerateCode(kodgen::CodeGenEnv& env,
		std::string& inout_result) noexcept	override final
	{
		env.getLogger()->log("DataPCG::finalGenerateCode() called", kodgen::ILogger::ELogSeverity::Info);
		return true;
	}

	virtual bool generateCodeForEntity(kodgen::EntityInfo const& entity,
		kodgen::Property const& property,
		kodgen::uint8				propertyIndex,
		kodgen::CodeGenEnv& env,
		std::string& inout_result) noexcept override final
	{

		DataStateCGE& dsCGE = static_cast<DataStateCGE&>(env);

		switch (dsCGE.getCodeGenTarget())
		{
		case DataStateCGE::DSCodeLoc_Includes:
			return generateCodeForIncludes(entity, property, propertyIndex, dsCGE, inout_result);
		case DataStateCGE::DSCodeLoc_DataVectors:
			return generateCodeForDataVectors(entity, property, propertyIndex, dsCGE, inout_result);
		case DataStateCGE::DSCodeLoc_EnumDataType:
			return generateCodeForEnumDataType(entity, property, propertyIndex, dsCGE, inout_result);
		case DataStateCGE::DSCodeLoc_TypeDefs:
			return generateCodeForTypeDefs(entity, property, propertyIndex, dsCGE, inout_result);
		case DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncDEF:
			return generateCodeForGetDatasPtrVariantFuncDEF(entity, property, propertyIndex, dsCGE, inout_result);
		case DataStateCGE::DSCodeLoc_GetDatasPtrVariantFuncArr:
			return generateCodeForGetDatasPtrVariantFuncArr(entity, property, propertyIndex, dsCGE, inout_result);
		case DataStateCGE::DSCodeLoc_EmplaceBackDataFuncDEF:
			return generateCodeForEmplaceBackDataFuncDEF(entity, property, propertyIndex, dsCGE, inout_result);
		case DataStateCGE::DSCodeLoc_EmplaceBackDataTemplateFuncs:
			return generateCodeForEmplaceBackDataTemplateFuncs(entity, property, propertyIndex, dsCGE, inout_result);
		default:
			env.getLogger()->log("DataPCG::generateCodeForEntity() called with unhandled DSCodeLoc. Abort generation.", kodgen::ILogger::ELogSeverity::Error);
			return false;
		}

		return true;
	}
		
};