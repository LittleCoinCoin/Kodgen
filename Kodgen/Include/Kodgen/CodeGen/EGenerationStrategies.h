#pragma once

#include <type_traits>

namespace kodgen
{
	/**
	*	@brief Flags indicating how the file generation should be done.
	*/
	enum class EGenerationStrategies : uint8
	{
		/** Unset type. */
		Undefined = 0u,

		/**
		* @brief Run the code generation process once for parsed each file.
		* @details Typically used when generating a file for each parsed file.
		*/
		OneGenerateForEachFile = 1 << 0,

		/**
		* @brief Run the code generation process once for all parsed files.
		* @details Typically used when generating a single file for all parsed files.
		*/
		OneGenerateForAllFiles = 1 << 1,

		/**
		* @brief Forces to parse all files, even if they are marked as up-to-date.
		*/
		ForceReparseAll = 1 << 2,

		/**
		* @brief Forces to regenerate all files, even if they are marked as up-to-date.
		*/
		ForceRegenerateAll = 1 << 3
	};

	/**
	*	@brief Binary "or" operation between 2 EGenerationStrategies masks.
	*
	*	@param mask1 First mask.
	*	@param mask2 Second mask.
	*
	*	@return The binary "or" value between the 2 provided masks.
	*/
	constexpr EGenerationStrategies operator|(EGenerationStrategies mask1, EGenerationStrategies mask2) noexcept
	{
		using UnderlyingType = std::underlying_type_t<EGenerationStrategies>;

		return static_cast<EGenerationStrategies>(static_cast<UnderlyingType>(mask1) | static_cast<UnderlyingType>(mask2));
	}

	/**
	*	@brief Binary "and" operation between 2 EGenerationStrategies masks.
	*
	*	@param mask1 First mask.
	*	@param mask2 Second mask.
	*
	*	@return The binary "and" value between the 2 provided masks.
	*/
	constexpr EGenerationStrategies operator&(EGenerationStrategies mask1, EGenerationStrategies mask2) noexcept
	{
		using UnderlyingType = std::underlying_type_t<EGenerationStrategies>;

		return static_cast<EGenerationStrategies>(static_cast<UnderlyingType>(mask1) & static_cast<UnderlyingType>(mask2));
	}

	/**
	*	@brief Check if 2 EGenerationStrategies masks overlap.
	*
	*	@param mask1 First mask to compare.
	*	@param mask2 Second mask to compare.
	*
	*	@return true if the 2 masks overlap, else false.
	*/
	constexpr bool operator&&(EGenerationStrategies mask1, EGenerationStrategies mask2) noexcept
	{
		return (mask1 & mask2) != EGenerationStrategies::Undefined;
	}
}