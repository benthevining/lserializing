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

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include "lserializing/lserializing_Export.h"
#include "lserializing/lserializing_Node.h"
#include "lserializing/lserializing_Printer.h"
#include "lserializing/lserializing_Schema.h"

/** @file
	This file defines the serializing::Format class.

	@ingroup limes_serializing
 */

namespace limes::serializing
{

/** An exception that is thrown by the parsing functions if errors are encountered.

	@ingroup limes_serializing

	@todo add a copy of the original input string
	@todo add reference to format object
 */
struct LSERIAL_EXPORT ParseError final : public std::runtime_error
{
	ParseError (std::string_view message, const text::utf8::LineAndColumn& lc);

	text::utf8::LineAndColumn position;
};

/** This class represents a serialization format.

	You will usually get a Format object from the \c KnownFormats manager class. You should
	generally consider Format objects to be global; you'll only have local instances if you
	create them yourself and don't use the \c KnownFormats API.

	@ingroup limes_serializing

	@todo getCommentDelimiter
 */
class LSERIAL_EXPORT Format
{
public:
	/** Destructor. */
	virtual ~Format() = default;

	/** @name Information queries */
	///@{

	/** Returns the format's name.

		This string is used as the format's identifier in the \c KnownFormats API. Every instance
		of a custom Format class should return the same string from this method, and you should
		aim to choose an identifier that will be globally unique for your program.
	 */
	[[nodiscard]] virtual std::string_view getName() const noexcept = 0;

	/** Returns a list of file extensions this format can handle.

		For example, for the JSON format, this would return '.json'; XML would return '.xml', and
		YAML would return both '.yaml' and '.yml'.

		The file extensions do not need to be unique; multiple formats may handle the same file
		extensions.
	 */
	[[nodiscard]] virtual const std::vector<std::string_view>& getFileExtensions() const noexcept = 0;

	/** Returns true if this format supports comments. */
	virtual bool supportsComments() const noexcept = 0;

	/** This function should return true if the passed string looks like valid data for this format.

		The default implementation just calls \c parse() , catches any exceptions, and returns \c false
		if one is thrown, and \c true otherwise. Obviously this is far from ideal for performance,
		so if you are implementing a custom format, you are highly encouraged to override this
		method with something clever for your format.

		@todo A better base class implementation for this?
	 */
	[[nodiscard]] virtual bool probablyMatchesString (std::string_view string) const noexcept;

	///@}

	/** @name Parsing and validating */
	///@{

	/** This function parses the given string and returns a \c Node object populated with the data. */
	[[nodiscard]] virtual Node parse (std::string_view string) const = 0;

	/** Creates a \c Schema object from some data.

		Not all formats support schema, so this may return \c nullptr .
	 */
	[[nodiscard]] virtual std::unique_ptr<Schema> createSchemaFrom (const Node& data) const noexcept;

	///@}

	/** @name Serializing and printing */
	///@{

	/** Creates a \c Printer object for this format.

		Every format is expected to provide a valid printer; this should not return a \c nullptr .
	 */
	[[nodiscard]] virtual std::unique_ptr<Printer> createPrinter (bool shouldPrettyPrint = false) const noexcept = 0;

	/** Serializes the data in the \c Node to a string in this format.

		The default implementation calls \c createPrinter() and then calls the printer object's \c print()
		method, but custom formats may override this method to bypass this logic.
	 */
	[[nodiscard]] virtual std::string serialize (const Node& node, bool shouldPrettyPrint = false) const noexcept;

	///@}

	/** Converts a string in this format to another serialization format.

		The default implementation calls \c parse() to create a \c Node from the input string, then passes
		this \c Node to the other format's \c serialize() method. However, custom formats may wish to use
		some custom logic.

		@param string The input string, in this object's serialization format
		@param otherFormat The format to be converted to
		@param shouldPrettyPrint Whether the output string should be pretty-printed

		@returns The string in the destination format
	 */
	virtual std::string convertTo (std::string_view string, const Format& otherFormat, bool shouldPrettyPrint = false) const noexcept;
};

}  // namespace limes::serializing
