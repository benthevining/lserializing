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

#include <type_traits>
#include <stdexcept>
#include "lserializing/lserializing_Export.h"
#include "lserializing/lserializing_Node.h"

/** @file
	Utilities for serializing enum values.

	@ingroup limes_serializing
 */

namespace limes::serializing
{

/** This namespace contains utilities for serializing enum values.

	@ingroup limes_serializing
 */
namespace enums
{

/** @ingroup limes_serializing
	@{
 */

/** True for any enum type. */
template <typename T>
concept Enum = std::is_enum_v<T>;

/** True for any arithmetic type. */
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

/** Evaluates to the underlying storage type of the given enum type. */
template <Enum Type>
using UnderlyingType = std::underlying_type_t<Type>;

/** Converts an enum to a desired integral type, first converting it to
	its underlying type.

	@see toEnum()
 */
template <Arithmetic To, Enum From>
[[nodiscard]] LSERIAL_EXPORT constexpr To fromEnum (From value) noexcept
{
	return static_cast<To> (static_cast<UnderlyingType<From>> (value));
}

/** Converts from an integral type to an enum value, by first converting
	to the enum's underlying type.

	@see fromEnum()
 */
template <Enum To, Arithmetic From>
[[nodiscard]] LSERIAL_EXPORT constexpr To toEnum (From value) noexcept
{
	return static_cast<To> (static_cast<UnderlyingType<To>> (value));
}

/** @} */

}  // namespace enums

/** A specialization of \c NodeConverter for any enum type.

	Internally, Node objects store numbers as doubles.

	@ingroup limes_serializing
 */
template <enums::Enum Type>
struct LSERIAL_EXPORT NodeConverter<Type> final
{
public:
	/** Converts the enum value to a \c Number \c Node .

		@see enums::fromEnum()
	 */
	[[nodiscard]] static Node serialize (const Type& t)
	{
		return Node::createNumber (enums::fromEnum<double> (t));
	}

	/** Converts the \c Node to the desired enum type.
		The node must be a \c Number type.

		@see enums::toEnum()
	 */
	static void deserialize (const Node& node, Type& t)
	{
		if (! node.isNumber())
			throw std::runtime_error { "Enum NodeConverter: Node is not a Number!" };

		t = enums::toEnum<Type> (node.getNumber());
	}
};

}  // namespace limes::serializing
