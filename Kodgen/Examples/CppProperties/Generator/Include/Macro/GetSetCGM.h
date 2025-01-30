#pragma once

#include <Kodgen/CodeGen/Macro/MacroCodeGenModule.h>

#include "GetPropertyCodeGen.h"
#include "SetPropertyCodeGen.h"

class GetSetCGM : public kodgen::MacroCodeGenModule
{
	private:
		GetPropertyCodeGen	_getPropertyCodeGen;
		SetPropertyCodeGen	_setPropertyCodeGen;

	public:
		GetSetCGM() noexcept
		{
			addPropertyCodeGen(_getPropertyCodeGen);
			addPropertyCodeGen(_setPropertyCodeGen);
		}

		GetSetCGM(GetSetCGM const&):
			GetSetCGM() //Call the default constructor to add the copied instance its own property references
		{
		}

		virtual GetSetCGM* clone() const noexcept override
		{
			return new GetSetCGM(*this);
		}
};