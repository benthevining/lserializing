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

#include "lserializing/lserializing_SerializingFormat.h"

namespace limes::serializing
{

ParseError::ParseError (std::string_view& message, const text::utf8::LineAndColumn& lc)
	: std::runtime_error (message.data()), position (lc) { }


std::string Format::serialize (const Node& node, bool shouldPrettyPrint) const noexcept
{
	return createPrinter (shouldPrettyPrint)->print (node);
}

std::string Format::convertTo (std::string_view string, const Format& otherFormat, bool shouldPrettyPrint) const noexcept
{
	return otherFormat.serialize (parse (string), shouldPrettyPrint);
}

bool Format::probablyMatchesString (std::string_view string) const noexcept
{
	try
	{
		[[maybe_unused]] const auto n = parse (string);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

std::unique_ptr<Schema> Format::createSchemaFrom (const Node& /*data*/) const noexcept
{
	return nullptr;
}

const text::EscapeCharacterSequence& Format::getEscapeSequence() const noexcept
{
	static const text::EscapeCharacterSequence emptySequence;

	return emptySequence;
}

text::CharacterEncoding* Format::getDefaultCharacterEncoding() const noexcept
{
	return nullptr;
}

}  // namespace limes::serializing
