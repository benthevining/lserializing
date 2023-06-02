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

class LSERIAL_NO_EXPORT TOMLFormat final : public Format
{
public:
	[[nodiscard]] std::string_view getName() const noexcept final
	{
		return formats::TOML;
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
				xtns.emplace_back (".toml");
			}
		};

		static const ExtensionsHolder holder;

		return holder.xtns;
	}

	[[nodiscard]] Node parse (std::string_view string) const final;

	[[nodiscard]] std::unique_ptr<Printer> createPrinter (bool shouldPrettyPrint) const noexcept final;

	[[nodiscard]] text::CharacterEncoding* getDefaultCharacterEncoding() const noexcept final
	{
		return nullptr;
	}
};

LSERIAL_NO_EXPORT static const KnownFormats::Register<TOMLFormat> toml_init;

/*-----------------------------------------------------------------------------------------------------------------------*/

Node TOMLFormat::parse (std::string_view /*string*/) const
{
	return {};
}

/*-----------------------------------------------------------------------------------------------------------------------*/

std::unique_ptr<Printer> TOMLFormat::createPrinter ([[maybe_unused]] bool shouldPrettyPrint) const noexcept
{
	class TOMLPrinter final : public Printer
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

	return std::make_unique<TOMLPrinter>();
}


}  // namespace limes::serializing
