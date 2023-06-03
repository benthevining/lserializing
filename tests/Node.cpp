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
#include <type_traits>
#include <string>
#include <string_view>
#include <vector>
#include <map>

#define TAGS "[serializing][Node]"

namespace serial = limes::serializing;
using Node = serial::Node;
using ObjectType = serial::ObjectType;

static_assert (std::is_same_v<serial::DataType<ObjectType::Number>, double>);
static_assert (std::is_same_v<serial::DataType<ObjectType::String>, std::string>);
static_assert (std::is_same_v<serial::DataType<ObjectType::Boolean>, bool>);
static_assert (std::is_same_v<serial::DataType<ObjectType::Array>, std::vector<Node>>);
static_assert (std::is_same_v<serial::DataType<ObjectType::Object>, std::map<std::string, Node>>);
static_assert (std::is_same_v<serial::DataType<ObjectType::Null>, serial::NullType>);

TEST_CASE ("Node - constructor & type checking", TAGS)
{
	SECTION ("Number")
	{
		Node n { ObjectType::Number };

		REQUIRE (n.isNumber());
		REQUIRE (n.getType() == ObjectType::Number);

		n.getNumber() = 1.;
		REQUIRE (n.get<double>() == 1.);
		REQUIRE (n.get<ObjectType::Number>() == 1.);

		n = 4.;

		REQUIRE (n.getNumber() == 4.);

		const auto n2 = Node::createNumber (6.);
		REQUIRE (n2.isNumber());
		REQUIRE (n2.getType() == ObjectType::Number);

		REQUIRE_THROWS (n.getString());
		REQUIRE_THROWS (n.getBoolean());
		REQUIRE_THROWS (n.getArray());
		REQUIRE_THROWS (n.getObject());

		REQUIRE_THROWS (n = false);
	}

	SECTION ("String")
	{
		Node n { ObjectType::String };

		REQUIRE (n.isString());
		REQUIRE (n.getType() == ObjectType::String);

		n.getString() = "Hello world";
		REQUIRE (n.get<std::string>() == "Hello world");
		REQUIRE (n.get<ObjectType::String>() == "Hello world");

		n = "Goodbye";

		REQUIRE (n.getString() == "Goodbye");

		const auto n2 = Node::createString ("");
		REQUIRE (n2.isString());
		REQUIRE (n2.getType() == ObjectType::String);

		REQUIRE_THROWS (n.getNumber());
		REQUIRE_THROWS (n.getBoolean());
		REQUIRE_THROWS (n.getArray());
		REQUIRE_THROWS (n.getObject());

		REQUIRE_THROWS (n = 12.);
	}

	SECTION ("Boolean")
	{
		Node n { ObjectType::Boolean };

		REQUIRE (n.isBoolean());
		REQUIRE (n.getType() == ObjectType::Boolean);

		n.getBoolean() = true;
		REQUIRE (n.get<bool>());
		REQUIRE (n.get<ObjectType::Boolean>());

		n = false;

		REQUIRE (! n.getBoolean());

		const auto n2 = Node::createBoolean (true);
		REQUIRE (n2.isBoolean());
		REQUIRE (n2.getType() == ObjectType::Boolean);

		REQUIRE_THROWS (n.getNumber());
		REQUIRE_THROWS (n.getString());
		REQUIRE_THROWS (n.getArray());
		REQUIRE_THROWS (n.getObject());

		REQUIRE_THROWS (n = "hello");
	}

	SECTION ("Array")
	{
		Node n { ObjectType::Array };

		REQUIRE (n.isArray());
		REQUIRE (n.getType() == ObjectType::Array);

		REQUIRE (n.getArray().empty());
		REQUIRE (n.get<serial::Array>().empty());
		REQUIRE (n.get<ObjectType::Array>().empty());

		REQUIRE_THROWS (n.getNumber());
		REQUIRE_THROWS (n.getBoolean());
		REQUIRE_THROWS (n.getString());
		REQUIRE_THROWS (n.getObject());

		REQUIRE_THROWS (n = true);
	}

	SECTION ("Object")
	{
		Node n { ObjectType::Object };

		REQUIRE (n.isObject());
		REQUIRE (n.getType() == ObjectType::Object);

		REQUIRE (n.getObject().empty());
		REQUIRE (n.get<serial::Object>().empty());
		REQUIRE (n.get<ObjectType::Object>().empty());

		REQUIRE_THROWS (n.getNumber());
		REQUIRE_THROWS (n.getBoolean());
		REQUIRE_THROWS (n.getString());
		REQUIRE_THROWS (n.getArray());

		REQUIRE_THROWS (n = 23.);
	}

	SECTION ("Null")
	{
		Node n { ObjectType::Null };

		REQUIRE (n.isNull());
		REQUIRE (n.getType() == ObjectType::Null);

		const auto n2 = Node::createNull();
		REQUIRE (n2.isNull());
		REQUIRE (n2.getType() == ObjectType::Null);

		Node n3;

		REQUIRE (n3.isNull());
		REQUIRE (n3.getType() == ObjectType::Null);

		REQUIRE_THROWS (n.getNumber());
		REQUIRE_THROWS (n.getBoolean());
		REQUIRE_THROWS (n.getString());
		REQUIRE_THROWS (n.getArray());
		REQUIRE_THROWS (n.getObject());

		REQUIRE_THROWS (n = 1.);
	}
}

TEST_CASE ("Node - subscript operators", TAGS)
{
	SECTION ("Array")
	{
		Node n { ObjectType::Array };

		REQUIRE (n.getNumChildren() == 0UL);

		n.addChildNull();
		n.addChildNumber (4.);

		REQUIRE (n.getNumChildren() == 2UL);

		REQUIRE (n[0UL].isNull());
		REQUIRE (n[1UL].getNumber() == 4.);

		REQUIRE_THROWS (n["foo"]);
	}

	SECTION ("Object")
	{
		Node n { ObjectType::Object };

		REQUIRE (n.getNumChildren() == 0UL);

		n.addChildNumber (42., "foo");
		n.addChildString ("hello", "bar");

		REQUIRE (n.getNumChildren() == 2UL);

		REQUIRE (n["foo"].getNumber() == 42.);
		REQUIRE (n["bar"].getString() == "hello");

		REQUIRE_THROWS (n[0UL]);
	}

	SECTION ("Other types")
	{
		const auto boolean = Node::createBoolean (true);

		REQUIRE_THROWS (boolean[0UL]);
		REQUIRE_THROWS (boolean["foo"]);

		const auto string = Node::createString ("fizzbuzz");

		REQUIRE_THROWS (string[0UL]);
		REQUIRE_THROWS (string["foo"]);

		const auto number = Node::createNumber (3.14);

		REQUIRE_THROWS (number[0UL]);
		REQUIRE_THROWS (number["foo"]);

		const auto null = Node::createNull();

		REQUIRE_THROWS (null[0UL]);
		REQUIRE_THROWS (null["foo"]);
	}
}

TEST_CASE ("Node - querying children & parents", TAGS)
{
	Node n { ObjectType::Object };

	REQUIRE (n.isRoot());
	REQUIRE (! n.hasParent());
	REQUIRE (n.getParent() == nullptr);
	REQUIRE (&n.getRoot() == &n);

	REQUIRE (n.getNumChildren() == 0UL);
	REQUIRE (! n.hasChildWithName("foo"));

	n.addChildNumber (36., "foo");

	REQUIRE (n.getNumChildren() == 1UL);
	REQUIRE (n.hasChildWithName("foo"));

	{
		auto& child = n["foo"];

		REQUIRE (! child.isRoot());
		REQUIRE (child.hasParent());
		REQUIRE (child.getParent() == &n);
		REQUIRE (&child.getRoot() == &n);
	}

	n.addChildArray ("bar");

	REQUIRE (n.getNumChildren() == 2UL);
	REQUIRE (n.hasChildWithName("bar"));

	auto& array = n["bar"];

	REQUIRE (! array.isRoot());
	REQUIRE (array.hasParent());
	REQUIRE (array.getParent() == &n);
	REQUIRE (&array.getRoot() == &n);

	REQUIRE (array.getNumChildren() == 0UL);

	array.addChildNull();
	array.addChildBoolean (true);
	array.addChildString ("fizzbuzz", "");

	REQUIRE (array.getNumChildren() == 3UL);

	REQUIRE (n.getNumChildren() == 2UL);

	auto& child = array[1UL];

	REQUIRE (! child.isRoot());
	REQUIRE (child.hasParent());
	REQUIRE (child.getParent() == &array);
	REQUIRE (&child.getRoot() == &n);
}

TEST_CASE ("Node - querying children of non-Object nodes", TAGS)
{
	const auto n = Node::createString ("hello world");

	REQUIRE (n.getNumChildren() == 0UL);
	REQUIRE (! n.hasChildWithName ("foo"));
}

TEST_CASE ("Node - getName()", TAGS)
{
	Node n { ObjectType::Object };

	REQUIRE (! n.hasName());

	auto& child = n.addChildNumber (42., "foo");

	REQUIRE (child.hasName());
	REQUIRE ((child.getName() == "foo"));
}
