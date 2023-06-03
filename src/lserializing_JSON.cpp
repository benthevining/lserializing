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

#include <cctype>
#include <cstdlib>
#include <string_view>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <memory>
#include <vector>
#include "lserializing/lserializing_SerializingFormat.h"
#include "lserializing/lserializing_KnownFormats.h"
#include "lserializing/lserializing_Node.h"
#include "lserializing/lserializing_Export.h"

namespace limes::serializing
{

class LSERIAL_NO_EXPORT JSONFormat final : public Format
{
public:
	[[nodiscard]] std::string_view getName() const noexcept final
	{
		return formats::JSON;
	}

	bool supportsComments() const noexcept final
	{
		return false;
	}

	const std::vector<std::string_view>& getFileExtensions() const noexcept final
	{
		struct ExtensionsHolder final
		{
			std::vector<std::string_view> xtns;

			ExtensionsHolder()
			{
				xtns.emplace_back (".json");
			}
		};

		static const ExtensionsHolder holder;

		return holder.xtns;
	}

	[[nodiscard]] Node parse (std::string_view string) const final;

	[[nodiscard]] std::unique_ptr<Printer> createPrinter (bool shouldPrettyPrint) const noexcept final;

	[[nodiscard]] std::unique_ptr<Schema> createSchemaFrom (const Node& data) const noexcept final;
};

LSERIAL_NO_EXPORT static const KnownFormats::Register<JSONFormat, true> json_init;

/*-----------------------------------------------------------------------------------------------------------------------*/

Node JSONFormat::parse (std::string_view string) const
{
	struct Parser final
	{
	public:
		explicit Parser (std::string_view inputText)
			: source (inputText), current (inputText)
		{
		}

		inline Node parse()
		{
			skipWhitespace();

			if (popIf ('['))
				return parseArray();

			if (popIf ('{'))
				return parseObject();

			if (! isEOF())
				throwError ("Expected an object or array");

			return Node::createNull();
		}

	private:
		inline void skipWhitespace()
		{
			for (auto p = current;
				 std::isspace (static_cast<unsigned char> (p.popFirstChar()));
				 current = p)
				;
		}

		inline bool popIf (char c)
		{
			return current.skipIfStartsWith (c);
		}

		inline bool popIf (const char* c)
		{
			return current.skipIfStartsWith (c);
		}

		inline std::uint32_t pop()
		{
			return current.popFirstChar();
		}

		inline bool isEOF()
		{
			return current.empty();
		}

		inline Node parseArray()
		{
			const auto arrayStart = current;

			auto result = Node { ObjectType::Array };

			skipWhitespace();

			if (popIf (']'))
				return result;

			for (;;)
			{
				skipWhitespace();

				if (isEOF())
					throwError ("Unexpected EOF in array declaration", arrayStart);

				result.getArray().push_back (parseValue());

				skipWhitespace();

				if (popIf (','))
					continue;

				if (popIf (']'))
					break;

				throwError ("Expected ',' or ']'");
			}

			return result;
		}

		inline Node parseObject()
		{
			const auto objectStart = current;

			auto result = Node { ObjectType::Object };

			skipWhitespace();

			if (popIf ('}'))
				return result;

			for (;;)
			{
				skipWhitespace();

				if (isEOF())
					throwError ("Unexpected EOF in object declaration", objectStart);

				if (! popIf ('"'))
					throwError ("Expected a name");

				const auto errorPos = current;
				const auto name		= parseString();

				if (name.empty())
					throwError ("Property names cannot be empty", errorPos);

				skipWhitespace();

				if (! popIf (':'))
					throwError ("Expected ':'");

				auto& obj = result.getObject();

				if (obj.contains (name))
					throwError ("Duplicate keys in same object", errorPos);

				obj.emplace (std::make_pair (name, parseValue()));

				skipWhitespace();

				if (popIf (','))
					continue;

				if (popIf ('}'))
					break;

				throwError ("Expected ',' or '}'");
			}

			return result;
		}

		inline Node parseValue()
		{
			skipWhitespace();

			auto startPos = current;

			switch (pop())
			{
				case '[' : return parseArray();
				case '{' : return parseObject();
				case '"' : return Node::createString (parseString());
				case '-' :
				{
					skipWhitespace();
					return parseNumber (true);
				}
				case '0' : [[fallthrough]];
				case '1' : [[fallthrough]];
				case '2' : [[fallthrough]];
				case '3' : [[fallthrough]];
				case '4' : [[fallthrough]];
				case '5' : [[fallthrough]];
				case '6' : [[fallthrough]];
				case '7' : [[fallthrough]];
				case '8' : [[fallthrough]];
				case '9' :
				{
					current = startPos;
					return parseNumber (false);
				}
				default : break;
			}

			current = startPos;

			if (popIf ("null"))
				return Node::createNull();

			if (popIf ("true"))
				return Node::createBoolean (true);

			if (popIf ("false"))
				return Node::createBoolean (false);

			throwError ("Syntax error");
		}

		inline Node parseNumber (bool negate)
		{
			auto startPos = current;
			bool hadDot = false, hadExponent = false;

			for (;;)
			{
				const auto lastPos = current;
				const auto c	   = pop();

				if (c >= '0' && c <= '9')
					continue;

				if (c == '.' && ! hadDot)
				{
					hadDot = true;
					continue;
				}

				if (! hadExponent && (c == 'e' || c == 'E'))
				{
					hadDot		= true;
					hadExponent = true;
					continue;
				}

				if (std::isspace (static_cast<int> (c)) != 0
					|| c == ',' || c == '}' || c == ']' || c == 0)
				{
					current					= lastPos;
					char* endOfParsedNumber = nullptr;

					if (! (hadDot || hadExponent))
					{
						const auto v = std::strtoll (startPos.data(), &endOfParsedNumber, 10);

						if (endOfParsedNumber == lastPos.data()
							&& v != std::numeric_limits<decltype (v)>::max()
							&& v != std::numeric_limits<decltype (v)>::min())
							return Node::createNumber (static_cast<double> (negate ? -v : v));
					}

					if (endOfParsedNumber == lastPos.data())
					{
						const auto v = std::strtod (startPos.data(), &endOfParsedNumber);

						return Node::createNumber (negate ? -v : v);
					}
				}

				throwError ("Syntax error in number", lastPos);
			}
		}

		inline std::string parseString()
		{
			std::stringstream s;

			for (;;)
			{
				auto c = pop();

				if (c == '"')
					break;

				if (c == '\\')
				{
					const auto errorPos = current;

					c = pop();

					switch (c)
					{
						case 'a' : c = '\a'; break;
						case 'b' : c = '\b'; break;
						case 'f' : c = '\f'; break;
						case 'n' : c = '\n'; break;
						case 'r' : c = '\r'; break;
						case 't' : c = '\t'; break;
						case 'u' : c = parseUnicodeCharacterNumber (false); break;
						case 0 : throwError ("Unexpected EOF in string constant", errorPos);
						default : break;
					}
				}

				char utf8Bytes[8];

				const auto numBytes = text::utf8::fromUnicode (utf8Bytes, c);

				for (std::uint32_t i = 0; i < numBytes; ++i)
					s << utf8Bytes[i];
			}

			return s.str();
		}

		inline std::uint32_t parseUnicodeCharacterNumber (bool isLowSurrogate)
		{
			std::uint32_t result = 0;

			for (auto i = 4; --i >= 0;)
			{
				const auto errorPos = current;

				auto digit = pop();

				if (digit >= '0' && digit <= '9')
					digit -= '0';
				else if (digit >= 'a' && digit <= 'f')
					digit = 10 + (digit - 'a');
				else if (digit >= 'A' && digit <= 'F')
					digit = 10 + (digit - 'A');
				else
					throwError ("Syntax error in unicode character", errorPos);

				result = (result << 4) + digit;
			}

			if (isLowSurrogate && ! text::utf8::isLowSurrogate (result))
				throwError ("Expected a unicode low surrogate codepoint");

			if (text::utf8::isHighSurrogate (result))
			{
				if (! isLowSurrogate && popIf ("\\u"))
					return text::utf8::SurrogatePair::combineParts (result, parseUnicodeCharacterNumber (true));

				throwError ("Expected a unicode low surrogate codepoint");
			}

			return result;
		}

		[[noreturn]] inline void throwError (std::string_view message)
		{
			throwError (message, current);
		}

		[[noreturn]] inline void throwError (std::string_view message, text::utf8::Pointer errorPos)
		{
			throw ParseError { message, text::utf8::LineAndColumn::find (source, errorPos) };
		}

		text::utf8::Pointer source, current;
	};

	Parser p { string };

	return p.parse();
}

/*-----------------------------------------------------------------------------------------------------------------------*/

static constexpr auto QUOTE_CHAR = '\'';

static inline std::string quoteString (std::string_view input)
{
	std::string copy { input };

	if (! copy.starts_with (QUOTE_CHAR))
		copy.insert (0, std::string { QUOTE_CHAR });

	if (! copy.ends_with (QUOTE_CHAR))
		copy += QUOTE_CHAR;

	return copy;
}

static inline std::string unquoteString (std::string_view input)
{
	std::string copy { input };

	auto dropFirstChars = [&copy](size_t numChars)
	{
		copy = copy.substr (numChars, copy.length());
	};

	if (copy.starts_with ("\\\""))
		dropFirstChars (2);
	else if (copy.starts_with ('"'))
		dropFirstChars (1);
	else if (copy.starts_with ("\\\'"))
		dropFirstChars (2);
	else if (copy.starts_with ('\''))
		dropFirstChars (1);

	auto dropLastChars = [&copy](size_t numChars)
	{
		for (auto i = 0UL; i < numChars; ++i)
		{
			if (copy.empty())
				return;

			copy.pop_back();
		}
	};

	if (copy.ends_with ("\\\""))
		dropLastChars (2);
	else if (copy.ends_with ('"'))
		dropLastChars (1);
	else if (copy.ends_with ("\\\'"))
		dropLastChars (2);
	else if (copy.ends_with ('\''))
		dropLastChars (1);

	return copy;
}

static inline std::string joinStrings (const std::vector<std::string>& strings, std::string_view glue)
{
	if (strings.size() == 1)
		return strings[0];

	std::stringstream stream;

	for (auto i = 0UL; i < strings.size(); ++i)
	{
		const auto& string = strings[i];

		stream << string;

		if ((i + 1 < strings.size()) && (! string.ends_with (glue)))
			stream << glue;
	}

	return stream.str();
}

std::unique_ptr<Printer> JSONFormat::createPrinter ([[maybe_unused]] bool shouldPrettyPrint) const noexcept
{
	class JSONPrinter final : public Printer
	{
	public:
	private:
		std::string printNull() final
		{
			return "null";
		}

		std::string printNumber (double number) final
		{
			if (std::isfinite (number))
				return unquoteString (std::to_string (number));

			if (std::isnan (number))
				return "\"NaN\"";

			if (number >= 0)
				return "\"Infinity\"";

			return "\"-Infinity\"";
		}

		std::string printString (std::string_view string) final
		{
			std::stringstream stream;

			stream << '"';

			text::utf8::Pointer text { string };

			auto writeUnicode = [&stream] (auto digit)
			{
				auto hexDigit = [] (auto value) -> char
				{ return "0123456789abcdef"[value & 15]; };

				stream << "\\u" << hexDigit (digit >> 12) << hexDigit (digit >> 8) << hexDigit (digit >> 4) << hexDigit (digit);
			};

			while (! text.empty())
			{
				const auto c = *text;

				switch (c)
				{
					case 0 : break;

					case '\"' : stream << "\\\""; break;
					case '\\' : stream << "\\\\"; break;
					case '\n' : stream << "\\n"; break;
					case '\r' : stream << "\\r"; break;
					case '\t' : stream << "\\t"; break;
					case '\a' : stream << "\\a"; break;
					case '\b' : stream << "\\b"; break;
					case '\f' : stream << "\\f"; break;

					default :
					{
						if (c > 31 && c < 127)
						{
							stream << static_cast<char> (c);
							break;
						}

						if (c >= 0x10000)
						{
							const auto pair = text::utf8::SurrogatePair::fromFullCodepoint (c);

							writeUnicode (pair.high);
							writeUnicode (pair.low);

							break;
						}

						writeUnicode (c);
						break;
					}
				}

				++text;
			}

			stream << '"';

			return stream.str();
		}

		std::string printBoolean (bool boolean) final
		{
			if (boolean)
				return "true";

			return "false";
		}

		std::string printArray (const Array& array) final
		{
			std::vector<std::string> strings;

			for (const auto& element : array)
				strings.emplace_back (print (element));	 // cppcheck-suppress useStlAlgorithm

			return "[ " + joinStrings (strings, ", ") + " ]";
		}

		std::string printObject (const Object& object) final
		{
			std::vector<std::string> strings;

			for (const auto& element : object)
			{
				auto str = quoteString (element.first);
				str += ':';
				str += print (element.second);

				strings.emplace_back (str);
			}

			return "{ " + joinStrings (strings, ", ") + " }";
		}

		void arrayBegin() final
		{
		}

		void arrayEnd() final
		{
		}

		void objectBegin() final
		{
		}

		void objectEnd() final
		{
		}
	};

	return std::make_unique<JSONPrinter>();
}

/*-----------------------------------------------------------------------------------------------------------------------*/

std::unique_ptr<Schema> JSONFormat::createSchemaFrom (const Node& /*data*/) const noexcept
{
	class JSONSchema final : public Schema
	{
	public:
	private:
		bool validateNext (const Node&) final
		{
			return true;
		}
	};

	return std::make_unique<JSONSchema>();
}

}  // namespace limes::serializing
