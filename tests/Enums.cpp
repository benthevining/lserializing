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

#include "lserializing/lserializing.h"
#include <catch2/catch_test_macros.hpp>

#define TAGS "[serializing][enums]"

namespace serial = limes::serializing;
namespace enums	 = serial::enums;

TEST_CASE ("Serializing enum values", TAGS)
{
	using ObjectType = serial::ObjectType;

	const auto orig = ObjectType::Null;

	REQUIRE (enums::toEnum<ObjectType> (enums::fromEnum<double> (orig)) == orig);

	const auto node = serial::NodeConverter<ObjectType>::serialize (orig);

	REQUIRE (node.isNumber());

	auto after = ObjectType::String;

	REQUIRE (orig != after);

	serial::NodeConverter<ObjectType>::deserialize (node, after);

	REQUIRE (orig == after);
}
