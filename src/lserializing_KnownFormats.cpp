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

#include <algorithm>
#include "lserializing/lserializing_KnownFormats.h"
#include "lserializing/lserializing_SerializingFormat.h"

namespace limes::serializing
{

FormatNotFoundError::FormatNotFoundError (std::string_view requestedFormat)
	: std::runtime_error (requestedFormat.data())
{
}


KnownFormats& KnownFormats::get() noexcept
{
	static KnownFormats manager;

	return manager;
}

const std::vector<const Format*>& KnownFormats::getAllFormats() const noexcept
{
	return formats;
}

void KnownFormats::registerFormat (const Format& newFormat)
{
	if (formats.empty())
	{
		formats.emplace_back (&newFormat);
		defaultFormat = &newFormat;
		return;
	}

	if (std::find_if (formats.begin(), formats.end(), [name = newFormat.getName()] (const Format* f)
					  { return f->getName() == name; })
		== formats.end())
	{
		formats.emplace_back (&newFormat);

		if (defaultFormat == nullptr)
			defaultFormat = &newFormat;
	}
}

void KnownFormats::deregisterFormat (const Format& format)
{
	if (auto ptr = std::find (formats.begin(), formats.end(), &format); ptr != formats.end())
		formats.erase (ptr);
}

const Format* KnownFormats::getFormatForString (std::string_view string) const noexcept
{
	for (auto* f : formats)
		if (f->probablyMatchesString (string))
			return f;

	return nullptr;
}

const Format* KnownFormats::getFormatForFileExtension (std::string_view extension) const noexcept
{
	for (auto* f : formats)
		for (const auto& xtn : f->getFileExtensions())
			if (xtn == extension)
				return f;

	return nullptr;
}

const Format* KnownFormats::getFormatWithName (std::string_view formatName) const noexcept
{
	for (auto* f : formats)
		if (f->getName() == formatName)
			return f;

	return nullptr;
}

Node KnownFormats::parse (std::string_view string) const
{
	if (auto* f = getFormatForString (string))
		return f->parse (string);

	throw FormatNotFoundError { "Unknown format" };
}

void KnownFormats::deserialize (SerializableData& data, std::string_view string) const noexcept
{
	data.deserialize (parse (string));
}

std::string KnownFormats::serialize (const SerializableData& data, bool prettyPrint, const Format* format) const
{
	if (format == nullptr)
	{
		format = defaultFormat;

		if (format == nullptr)
			throw FormatNotFoundError { "UnspecifiedFormat" };
	}

	return format->serialize (data.serialize(), prettyPrint);
}

std::string KnownFormats::convert (std::string_view string, std::string_view newFormatName, const Format* origFormat) const
{
	return convert (string, getFormatWithName (newFormatName), origFormat);
}

std::string KnownFormats::convert (std::string_view& string, const Format* newFormat, const Format* origFormat) const
{
	if (newFormat == nullptr)
		throw FormatNotFoundError { "KnownFormats::convert - target format not specified" };

	if (origFormat == nullptr)
	{
		origFormat = getFormatForString (string);

		if (origFormat == nullptr)
			throw FormatNotFoundError { "KnownFormats::convert - original format not specified, and could not be deduced" };
	}

	return origFormat->convertTo (string, *newFormat);
}

const Format* KnownFormats::getDefaultFormat() const noexcept
{
	return defaultFormat;
}

void KnownFormats::setDefaultFormat (const Format& format) noexcept
{
	registerFormat (format);
	defaultFormat = &format;
}

}  // namespace limes::serializing
