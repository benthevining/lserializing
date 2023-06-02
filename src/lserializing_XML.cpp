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

#include <string_view>
#include <memory>
#include "lserializing/lserializing_SerializingFormat.h"
#include "lserializing/lserializing_KnownFormats.h"
#include "lserializing/lserializing_Node.h"
#include "lserializing/lserializing_Export.h"

namespace limes::serializing
{

class LSERIAL_NO_EXPORT XMLFormat final : public Format
{
public:
	[[nodiscard]] std::string_view getName() const noexcept final
	{
		return formats::XML;
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
				xtns.emplace_back (".xml");
			}
		};

		static const ExtensionsHolder holder;

		return holder.xtns;
	}

	[[nodiscard]] Node parse (std::string_view string) const final;

	[[nodiscard]] std::unique_ptr<Printer> createPrinter (bool shouldPrettyPrint) const noexcept final;

	[[nodiscard]] std::unique_ptr<Schema> createSchemaFrom (const Node& data) const noexcept final;

	[[nodiscard]] text::CharacterEncoding* getDefaultCharacterEncoding() const noexcept final
	{
		return nullptr;
	}
};

LSERIAL_NO_EXPORT static const KnownFormats::Register<XMLFormat> xml_init;

/*-----------------------------------------------------------------------------------------------------------------------*/

Node XMLFormat::parse (std::string_view /*string*/) const
{
	return {};

	//	struct Parser final
	//	{
	//	public:
	//		explicit Parser (const std::string_view& xmlText)
	//			: source (xmlText), current (xmlText)
	//		{
	//		}
	//
	//		inline Node parse()
	//		{
	//			skipCommentsAndWhitespace();
	//
	//			// skip the XML declaration
	//
	//			return Node::createNull();
	//		}
	//
	//	private:
	//		inline void skipCommentsAndWhitespace()
	//		{
	//			// whitespace
	//			for (auto p = current;
	//				 std::isspace (static_cast<unsigned char> (p.popFirstChar()));
	//				 current = p)
	//				;
	//
	//			// comments
	//			if (current.startsWith ("<!--"))
	//			{
	//				current = current.find ("-->");
	//				current += 3;
	//			}
	//		}
	//
	//		// finds the closing tag, advances the current pointer
	//		// to the end of the closing tag, and returns all text
	//		// between the opening and closing tags
	//		inline std::string_view getTextBetweenTags()
	//		{
	//			skipCommentsAndWhitespace();
	//
	//			if (! current.skipIfStartsWith ('<'))
	//			{
	//				// throw error
	//			}
	//
	//			const auto* tagNameStart = current.data();
	//
	//			current = current.find ('>');
	//			--current;
	//
	//			const std::string_view tagName { tagNameStart, current.data() };
	//
	//			current += 2;
	//
	//			const auto* contentStart = current.data();
	//
	//			std::stringstream stream;
	//
	//			stream << "</" << tagName << '>';
	//
	//			const auto endTag = stream.str();
	//
	//			current = current.find (endTag);
	//			--current;
	//
	//			std::string_view tagContent { contentStart, current.data() };
	//
	//			current += endTag.length();
	//
	//			return tagContent;
	//		}
	//
	//		inline Node parseArray()
	//		{
	//		}
	//
	//		inline Node parseObject()
	//		{
	//		}
	//
	//		inline Node parseValue()
	//		{
	//		}
	//
	//		text::utf8::Pointer source, current;
	//	};
	//
	//	Parser p { string };
	//
	//	return p.parse();
}

/*-----------------------------------------------------------------------------------------------------------------------*/

std::unique_ptr<Printer> XMLFormat::createPrinter ([[maybe_unused]] bool shouldPrettyPrint) const noexcept
{
	class XMLPrinter final : public Printer
	{
	public:
	private:
		std::string printNull() final
		{
			return "";
		}

		std::string printNumber (double number) final
		{
			return text::quoted (std::to_string (number));
		}

		std::string printString (std::string_view string) final
		{
			return text::quoted (string);
		}

		std::string printBoolean (bool boolean) final
		{
			if (boolean)
				return "true";

			return "false";
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

	return std::make_unique<XMLPrinter>();
}

/*-----------------------------------------------------------------------------------------------------------------------*/

std::unique_ptr<Schema> XMLFormat::createSchemaFrom (const Node& /*data*/) const noexcept
{
	class XMLSchema final : public Schema
	{
	public:
	private:
		bool validateNext (const Node&) final
		{
			return true;
		}
	};

	return std::make_unique<XMLSchema>();
}

}  // namespace limes::serializing
