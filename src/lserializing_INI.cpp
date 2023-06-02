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

#include <memory>
#include "lserializing/lserializing_SerializingFormat.h"
#include "lserializing/lserializing_KnownFormats.h"
#include "lserializing/lserializing_Node.h"
#include "lserializing/lserializing_Export.h"

namespace limes::serializing
{

class LSERIAL_NO_EXPORT INIFormat final : public Format
{
public:
	[[nodiscard]] std::string_view getName() const noexcept final
	{
		return formats::INI;
	}

	bool supportsComments() const noexcept final
	{
		return true;
	}

	const std::vector<std::string_view>& getFileExtensions() const noexcept final
	{
		struct ExtensionsHolder final
		{
			std::vector<std::string_view> xtns;

			ExtensionsHolder()
			{
				xtns.emplace_back (".ini");
			}
		};

		static const ExtensionsHolder holder;

		return holder.xtns;
	}

	[[nodiscard]] Node parse (std::string_view string) const final;

	[[nodiscard]] std::unique_ptr<Printer> createPrinter (bool shouldPrettyPrint) const noexcept final;
};

LSERIAL_NO_EXPORT static const KnownFormats::Register<INIFormat> ini_init;

/*-----------------------------------------------------------------------------------------------------------------------*/

Node INIFormat::parse (std::string_view string) const
{
	class Parser final
	{
	public:
		explicit Parser (std::string_view inputText)
			: source (inputText), current (inputText)
		{
		}

		inline Node parse()
		{
			return Node {};

			//			skipWhitespace();
			//
			//			if (popIf ('['))
			//				return parseObject();

			// throw error

			// when done, insert all vars from default section into every other
			// section, without overriding existing section vars

			// REPLACING VARIABLES
			// 1. Locally within each section, every occurrence "${variable}" is replaced by "${section:variable}".
			// 2. Every occurrence of "${section:variable}" is replaced by its value.
			// 3. The previous step is repeated until no more replacements are possible, or until the recursion depth is reached.
		}

	private:
		inline void skipWhitespace()
		{
			for (auto p = current;
				 std::isspace (static_cast<unsigned char> (p.popFirstChar()));
				 current = p)
				;

			// INI comments start with ;
			if (popIf (';'))
			{
				current.jumpToAfter (text::getNewline());
				skipWhitespace();
			}
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

		inline Node parseObject()
		{
			// find the ending ]
			// throw if line doesn't end w ]

			skipWhitespace();

			// end this object & start a new one if line begins with [

			// if line contains = then all before are variable and all after are value
			// both are trimmed
			// if variable was already assigned, report error

			// otherwise report error

			return {};
		}

		inline Node parseValue()
		{
			skipWhitespace();
			return Node {};
		}

		inline Node parseArray()
		{
			return Node {};
		}

		inline Node parseNumber (bool /*negate*/)
		{
			return Node {};
		}

		inline std::string parseString()
		{
			return {};
		}

		[[noreturn]] inline void throwError (const std::string_view& message)
		{
			throwError (message, current);
		}

		[[noreturn]] inline void throwError (const std::string_view& message, text::utf8::Pointer errorPos)
		{
			throw ParseError { message, text::utf8::LineAndColumn::find (source, errorPos) };
		}

		text::utf8::Pointer source, current;
	};

	Parser p { string };

	return p.parse();
}

/*-----------------------------------------------------------------------------------------------------------------------*/

std::unique_ptr<Printer> INIFormat::createPrinter ([[maybe_unused]] bool shouldPrettyPrint) const noexcept
{
	class INIPrinter final : public Printer
	{
	public:
	private:
		std::string printNull() final
		{
			return {};
		}

		std::string printNumber (double /*number*/) final
		{
			return {};
		}

		std::string printString (std::string_view /*string*/) final
		{
			return {};
		}

		std::string printBoolean (bool /*boolean*/) final
		{
			return {};
		}

		std::string printArray (const Array& /*array*/) final
		{
			return {};
		}

		std::string printObject (const Object& /*object*/) final
		{
			return {};
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

	return std::make_unique<INIPrinter>();
}


}  // namespace serializing
