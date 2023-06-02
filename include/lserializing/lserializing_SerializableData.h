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

#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <utility>
#include <type_traits>
#include <functional>  // for std::hash
#include <stdexcept>
#include "lserializing/lserializing_Export.h"

/** @file
	This file defines the SerializableData class.

	@ingroup limes_serializing
 */

namespace limes::serializing
{

class Node;

/** This class provides an interface for any C++ object that can be serialized to and from a Node.

	In order to serialize a custom object, it either needs to inherit \c SerializableData or you
	need to implement a specialization of \c NodeConverter for your type.

	@see NodeConverter, Node

	@ingroup limes_serializing
 */
class LSERIAL_EXPORT SerializableData
{
public:
	/** Destructor. */
	virtual ~SerializableData() = default;

	/** This function must reproducibly save the state of this object. */
	[[nodiscard]] virtual Node serialize() const = 0;

	/** This function must restore a previous state saved by \c serialize() . */
	virtual void deserialize (const Node&) = 0;
};

/** @concept ImplementsSerializableData

	Any object that inherits SerializableData or implements \c serialize() and \c deserialize()
	functions with the same interface satisfies this concept.

	@see SerializableData
	@ingroup serializing
 */
template <typename T>
concept ImplementsSerializableData = std::is_base_of_v<T, SerializableData> || requires (T a, const Node& n)
{
	{ a.serialize() }; // TODO: verify return type is a Node
	{ a.deserialize (n) };
};

}  // namespace limes::serializing
