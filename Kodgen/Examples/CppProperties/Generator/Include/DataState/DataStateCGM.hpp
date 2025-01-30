#pragma once

/*!
@file DataStateCGM.hpp
@brief DataStateCGM class declaration.
@details DataStateCGM is a code generation module that manages the analyses of
		 the structs or classes marked as Data (thanks to DataPCG) in user
		 code and sends back up information about them (through the inout_result
		 string) to the DataStateCGU for further processing.
*/

#include <Kodgen/CodeGen/CodeGenModule.h>

#include "DataPCG.hpp"

class DataStateCGM : public kodgen::CodeGenModule
{
private:
    DataPCG _dataPCG;

	//Make the addPropertyCodeGen private to replace it with a more restrictive method accepting DataStateCGM.
	using kodgen::CodeGenModule::addPropertyCodeGen;

public:
    DataStateCGM() noexcept
    {
        addPropertyCodeGen(_dataPCG);
    }

    DataStateCGM(DataStateCGM const&) noexcept :
        DataStateCGM() //Call default constructor to add the propertyCodeGen
    {
            
    }

	ICloneable* clone() const noexcept override
	{
		return new DataStateCGM(*this);
	}
		
	virtual bool initialGenerateCode(kodgen::CodeGenEnv& env, std::string& inout_result) noexcept override
	{
		env.getLogger()->log("DataStateCGM::initialGenerateCode() called", kodgen::ILogger::ELogSeverity::Info);

		return true;
	}

	virtual bool finalGenerateCode(kodgen::CodeGenEnv& env, std::string& inout_result) noexcept override
	{
		env.getLogger()->log("DataStateCGM::finalGenerateCode() called", kodgen::ILogger::ELogSeverity::Info);
			
		return true;
	}

	virtual kodgen::ETraversalBehaviour	generateCodeForEntity(kodgen::EntityInfo const& entity,
		kodgen::CodeGenEnv& env, std::string& inout_result)	noexcept override
	{
		if (entity.entityType && kodgen::EEntityType::Namespace)
		{
			//Our target are structs and classes, so we go inside Namespaces.
			return kodgen::ETraversalBehaviour::Recurse;
		}

		else if (entity.entityType && kodgen::EEntityType::Class || entity.entityType && kodgen::EEntityType::Struct)
		{
			//We go over all structs and classes in the namespace, but we don't care
			//about what is inside for the code generation of the DataState.
			env.getLogger()->log("DataStateCGM::generateCodeForEntity() called on entity " + entity.getFullName(), kodgen::ILogger::ELogSeverity::Info);
			return kodgen::ETraversalBehaviour::Continue;
		}

		else
		{
			//We skip everythin else to get to the next possible namespace.
			return kodgen::ETraversalBehaviour::Break;
		}

	}
};