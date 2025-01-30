/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <string>

#include "Kodgen/CodeGen/PropertyCodeGen.h"

class SetPropertyCodeGen : public kodgen::MacroPropertyCodeGen
{
	public:
		SetPropertyCodeGen() noexcept:
			kodgen::MacroPropertyCodeGen("Set", kodgen::EEntityType::Field)
		{}

		virtual bool preGenerateCodeForEntity(kodgen::EntityInfo const& /* entity */, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */, kodgen::MacroCodeGenEnv& env) noexcept override
		{
			std::string errorMessage;

			//Check that Set property arguments are valid
			if (property.arguments.size() > 1)
			{
				errorMessage = "Set property can't take more than one argument.";
			}
			else if (property.arguments.size() > 0 && property.arguments[0] != "explicit")
			{
				errorMessage = "Set property only valid argument is 'explicit'.";
			}

			if (!errorMessage.empty())
			{
				//Log error message and abort generation
				if (env.getLogger() != nullptr)
				{
					env.getLogger()->log(errorMessage, kodgen::ILogger::ELogSeverity::Error);
				}

				return false;
			}

			//If arguments are valid, dispatch the generation call normally
			return true;
		}

		virtual bool generateClassFooterCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& /* property */, kodgen::uint8 /* propertyIndex */,
													  kodgen::MacroCodeGenEnv& env, std::string& inout_result) noexcept override
		{
			kodgen::FieldInfo const& field = static_cast<kodgen::FieldInfo const&>(entity);

			//Can't generate any setter if the field is originally const qualified
			if (field.type.typeParts.back().descriptor == kodgen::ETypeDescriptor::Const)
			{
				if (env.getLogger() != nullptr)
				{
					env.getLogger()->log("Can't generate setter for the field " + entity.getFullName() + " because it is const qualified. Abort generation.", kodgen::ILogger::ELogSeverity::Error);
				}

				return false;
			}

			std::string paramName = "_kodgen" + field.name;

			std::string preTypeQualifiers;

			//Upper case the first field info char if applicable
			std::string methodName = field.name;
			methodName.replace(0, 1, 1, static_cast<char>(std::toupper(methodName.at(0)))); 
			methodName.insert(0, "set");
			methodName += "(";

			methodName += field.type.getCanonicalName();
			methodName += ((field.type.sizeInBytes == 0u || field.type.sizeInBytes > 4u) &&
						   !(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::Ptr)	&&
						   !(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::LRef)) ? " const& " : " ";
			methodName += paramName;

			methodName += ")";

			if (field.isStatic)
			{
				preTypeQualifiers = "static";
			}

			inout_result += preTypeQualifiers + "void " + methodName + ";" + env.getSeparator();

			return true;
		}

		virtual bool generateSourceFileHeaderCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */,
														   kodgen::MacroCodeGenEnv& env, std::string& inout_result) noexcept override
		{
			kodgen::FieldInfo const& field = static_cast<kodgen::FieldInfo const&>(entity);

			std::string paramName = "_kodgen" + field.name;

			//Don't generate setter definition if it is marked as explicit
			if (!property.arguments.empty())	//explicit is the only supported argument, so if it is not empty it must be explicit
			{
				return true;
			}

			std::string preTypeQualifiers;

			//Upper case the first field info char if applicable
			std::string methodName = field.name;
			methodName.replace(0, 1, 1, static_cast<char>(std::toupper(methodName.at(0)))); 
			methodName.insert(0, "set");
			methodName += "(";

			methodName += field.type.getCanonicalName();
			methodName += ((field.type.sizeInBytes == 0u || field.type.sizeInBytes > 4u) &&
						   !(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::Ptr)	&&
						   !(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::LRef)) ? " const& " : " ";
			methodName += paramName;

			methodName += ")";

			inout_result += preTypeQualifiers + "void " + entity.outerEntity->getFullName() + "::" + methodName + " { " + field.name + " = " + paramName + "; }" + env.getSeparator();

			return true;
		}
};