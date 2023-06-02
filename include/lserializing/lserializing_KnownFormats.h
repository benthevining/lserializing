/*
 * ======================================================================================
 *  __    ____  __  __  ____  ___
 * (  )  (_  _)(  \/  )( ___)/ __)
 *  )(__  _)(_  )    (  )__) \__ \
 * (____)(____)(_/\/\_)(____)(___/
 *
 *  This file is part of the Limes open source library and is licensed under the terms of the GNU Public License.
 *
 *  Commercial licenses are available; contact the maintainers at ben.the.vining@gmail.com to inquire for details.
 *
 * ======================================================================================
 */

#pragma once

#include <stdexcept>
#include <memory>
#include <type_traits>
#include <string_view>
#include <vector>
#include "lserializing/lserializing_Export.h"

/** @file
	This file defines the serializing::KnownFormats class.

	@ingroup limes_serializing
 */

namespace limes::serializing
{

/** This namespace contains string literals that can be passed to the
	KnownFormats object in order to retrieve Format objects that are
	known to Limes.

	@see KnownFormats, Format

	@ingroup limes_serializing
 */
namespace formats
{

/** The JSON serialization format. */
LSERIAL_EXPORT static constexpr auto JSON = "JSON";

/** The XML serialization format. */
LSERIAL_EXPORT static constexpr auto XML = "XML";

/** The YAML serialization format. */
LSERIAL_EXPORT static constexpr auto YAML = "YAML";

/** The INI serialization format. */
LSERIAL_EXPORT static constexpr auto INI = "INI";

/** The TOML serialization format. */
LSERIAL_EXPORT static constexpr auto TOML = "TOML";

}  // namespace formats

/** @exception FormatNotFoundError
	@ingroup limes_serializing
 */
struct LSERIAL_EXPORT FormatNotFoundError final : public std::runtime_error
{
	explicit FormatNotFoundError (std::string_view requestedFormat);
};

class Format;
class SerializableData;
class Node;

/** Any superclass of Format satisfies this concept.
	@ingroup limes_serializing
 */
template <typename T>
concept IsFormat = std::is_base_of_v<Format, T>;

/** This class manages a registry of Format objects.

	The following serialization formats are provided by this library:
	- JSON
	- XML
	- YAML
	- TOML
	- INI

	Other third party libraries may add new serialization formats and register
	them with the KnownFormats object, allowing Limes code to find and use
	custom formats as if they were native to Limes.

	@see Format

	@ingroup limes_serializing

	@todo docs
	@todo set pretty print
	@todo set format for file extension. Overrides querying registered formats.
 */
class LSERIAL_EXPORT KnownFormats final
{
public:
	static KnownFormats& get() noexcept;

	/** @name Finding formats */
	///@{

	[[nodiscard]] const std::vector<const Format*>& getAllFormats() const noexcept;

	[[nodiscard]] const Format* getFormatForString (std::string_view string) const noexcept;

	[[nodiscard]] const Format* getFormatForFileExtension (std::string_view extension) const noexcept;

	[[nodiscard]] const Format* getFormatWithName (std::string_view formatName) const noexcept;

	///@}

	/** @name The default format */
	///@{

	[[nodiscard]] const Format* getDefaultFormat() const noexcept;

	void setDefaultFormat (const Format& format) noexcept;

	///@}

	/** @name Format conversion */
	///@{

	[[nodiscard]] std::string convert (std::string_view string, std::string_view newFormatName, const Format* origFormat = nullptr) const;

	[[nodiscard]] std::string convert (std::string_view string, const Format* newFormat, const Format* origFormat = nullptr) const;

	///@}

	/** @name Deserialization */
	///@{

	[[nodiscard]] Node parse (std::string_view string) const;

	void deserialize (SerializableData& data, std::string_view string) const noexcept;

	///@}

	[[nodiscard]] std::string serialize (const SerializableData& data, bool prettyPrint = false, const Format* format = nullptr) const;

	template <IsFormat FormatType, bool MakeDefault = false>
	class LSERIAL_EXPORT Register final
	{
		static_assert (std::is_default_constructible_v<FormatType>);

	public:
		Register()
		{
			struct InternalRegister final
			{
				InternalRegister()
				{
					if constexpr (MakeDefault)
						KnownFormats::get().setDefaultFormat (format);
					else
						KnownFormats::get().registerFormat (format);
				}

				~InternalRegister()
				{
					KnownFormats::get().deregisterFormat (format);
				}

			private:
				const FormatType format;
			};

			[[maybe_unused]] static const InternalRegister reg;
		}
	};

private:
	KnownFormats() = default;

	void registerFormat (const Format& newFormat);
	void deregisterFormat (const Format& format);

	std::vector<const Format*> formats;

	const Format* defaultFormat { nullptr };
};

}  // namespace limes::serializing
