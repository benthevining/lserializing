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
#include <string>
#include <cmath>
#include <sstream>
#include <limes_core.h>
#include <stdexcept>
#include "lserializing/lserializing_Node.h"
#include "lserializing/lserializing_SerializableData.h"
#include "lserializing/lserializing_SerializingFormat.h"
#include "lserializing/lserializing_KnownFormats.h"

namespace limes::serializing
{

Node::Node (ObjectType typeToUse) noexcept
	: type (typeToUse)
{
	switch (type)
	{
		case (ObjectType::Number) : data = 0.; return;
		case (ObjectType::String) : data = std::string {}; return;
		case (ObjectType::Boolean) : data = false; return;
		case (ObjectType::Array) : data = Array {}; return;
		case (ObjectType::Object) : data = Object {}; return;
	}
}

Node::Node (const Node& other)
	: type (other.type), data (other.data), parent (other.parent)
{
}

Node& Node::operator= (const Node& other)
{
	type   = other.type;
	parent = other.parent;
	data   = other.data;

	return *this;
}

Node::Node (Node&& other) noexcept
	: type (other.type), data (std::move (other.data)), parent (other.parent)
{
}

Node& Node::operator= (Node&& other) noexcept
{
	type   = other.type;
	parent = other.parent;
	data   = std::move (other.data);

	return *this;
}

ObjectType Node::getType() const noexcept
{
	return type;
}

std::string_view Node::getTypeAsString() const noexcept
{
	switch (type)
	{
		case (ObjectType::Null) : return "Null";
		case (ObjectType::Object) : return "Object";
		case (ObjectType::Array) : return "Array";
		case (ObjectType::Boolean) : return "Boolean";
		case (ObjectType::String) : return "String";
		case (ObjectType::Number) : return "Number";
		default: return ""; // unreachable
	}
}

Node& Node::operator[] (std::string_view childName)
{
	if (! isObject())
		throw std::runtime_error { "Cannot call operator[string] on Node that is not an Object" };

	for (auto& pair : std::get<Object> (data))
		if (pair.first == childName)  // cppcheck-suppress useStlAlgorithm
			return pair.second;

	throw std::runtime_error { "Child node could not be found!" };
}

Node& Node::operator[] (size_t idx)
{
	if (! isArray())
		throw std::runtime_error { "Cannot call operator[size_t] on Node that is not an Array" };

	auto& array = std::get<Array> (data);

	if (idx >= static_cast<size_t> (array.size()))
		throw std::out_of_range { "Array index out of range!" };

	return array[idx];
}

size_t Node::getNumChildren() const noexcept
{
	if (isArray())
		return std::get<Array> (data).size();

	if (isObject())
		return std::get<Object> (data).size();

	return 0UL;
}

bool Node::hasChildWithName (std::string_view childName) const noexcept
{
	if (! isObject())
		return false;

	for (const auto& pair : std::get<Object> (data))
		if (pair.first == childName)  // cppcheck-suppress useStlAlgorithm
			return true;

	return false;
}

Node& Node::addChild (ObjectType childType, std::string_view childName)
{
	switch (childType)
	{
		case (ObjectType::Null) : return addChildNull (childName);
		case (ObjectType::Object) : return addChildObject (childName);
		case (ObjectType::Array) : return addChildArray (childName);
		case (ObjectType::Boolean) : return addChildBoolean (childName);
		case (ObjectType::String) : return addChildString (childName);
		case (ObjectType::Number) : return addChildNumber (childName);
	}
}

Node& Node::addChild (const Node& childNode, std::string_view childName)
{
	if (isArray())
	{
		auto& child = std::get<Array> (data).emplace_back (childNode);

		child.parent = this;

		return child;
	}

	if (isObject())
	{
		if (childName.empty())
			throw std::runtime_error { "addChild() on Object: childName cannot be empty!" };

		auto& parentObj = std::get<Object> (data);

		const auto name = std::string { childName };

		// cannot have duplicate keys in an object!
		if (parentObj.contains (name))
			throw std::runtime_error { "addChild() on Object: cannot have duplicate keys in an object!" };

		auto pair = parentObj.emplace (std::make_pair (name, childNode));

		auto& child = (*pair.first).second;

		child.parent = this;

		return child;
	}

	throw std::runtime_error { "Cannot addChild() to Node that is not an Array or an Object" };
}

Node& Node::addChildInternal (std::string_view& childName, ObjectType childType)
{
	if (isArray())
	{
		auto& child = std::get<Array> (data).emplace_back (childType);

		child.parent = this;

		return child;
	}

	if (isObject())
	{
		if (childName.empty())
			throw std::runtime_error { "addChild() on Object: childName cannot be empty!" };

		auto& parentObj = std::get<Object> (data);

		const auto name = std::string { childName };

		// cannot have duplicate keys in an object!
		if (parentObj.contains (name))
			throw std::runtime_error { "addChild() on Object: cannot have duplicate keys in an object!" };

		auto pair = parentObj.emplace (std::make_pair (name, Node { childType }));

		auto& child = (*pair.first).second;

		child.parent = this;

		return child;
	}

	throw std::runtime_error { "Cannot addChild() to Node that is not an Array or an Object" };
}

bool Node::isNumber() const noexcept
{
	return type == ObjectType::Number;
}

double& Node::getNumber()
{
	if (! isNumber())
		throw std::runtime_error { "getNumber(): Node is not a Number!" };

	return std::get<double> (data);
}

double Node::getNumber() const
{
	if (! isNumber())
		throw std::runtime_error { "getNumber(): Node is not a Number!" };

	return std::get<double> (data);
}

Node& Node::operator= (double value)
{
	getNumber() = value;
	return *this;
}

Node& Node::addChildNumber (std::string_view childName)
{
	return addChildInternal (childName, ObjectType::Number);
}

Node& Node::addChildNumber (double value, std::string_view childName)
{
	auto& child = addChildNumber (childName);

	child.getNumber() = value;

	return child;
}

bool Node::isString() const noexcept
{
	return type == ObjectType::String;
}

Node& Node::operator= (std::string_view value)
{
	getString() = value;
	return *this;
}

Node& Node::addChildString (std::string_view childName)
{
	return addChildInternal (childName, ObjectType::String);
}

std::string& Node::getString()
{
	if (! isString())
		throw std::runtime_error { "getString(): Node is not a String!" };

	return std::get<std::string> (data);
}

std::string_view Node::getString() const
{
	if (! isString())
		throw std::runtime_error { "getString(): Node is not a String!" };

	return std::get<std::string> (data);
}

bool Node::isBoolean() const noexcept
{
	return type == ObjectType::Boolean;
}

bool& Node::getBoolean()
{
	if (! isBoolean())
		throw std::runtime_error { "getBoolean(): Node is not a Boolean!" };

	return std::get<bool> (data);
}

bool Node::getBoolean() const
{
	if (! isBoolean())
		throw std::runtime_error { "getBoolean(): Node is not a Boolean!" };

	return std::get<bool> (data);
}

Node& Node::operator= (bool value)
{
	getBoolean() = value;
	return *this;
}

Node& Node::addChildBoolean (std::string_view childName)
{
	return addChildInternal (childName, ObjectType::Boolean);
}

Node& Node::addChildBoolean (bool value, std::string_view childName)
{
	auto& child = addChildBoolean (childName);

	child.getBoolean() = value;

	return child;
}

bool Node::isArray() const noexcept
{
	return type == ObjectType::Array;
}

Array& Node::getArray()
{
	if (! isArray())
		throw std::runtime_error { "getArray(): Node is not an Array!" };

	return std::get<Array> (data);
}

const Array& Node::getArray() const
{
	if (! isArray())
		throw std::runtime_error { "getArray(): Node is not an Array!" };

	return std::get<Array> (data);
}

Node& Node::operator= (const Array& value)
{
	getArray() = value;
	return *this;
}

Node& Node::addChildArray (std::string_view childName)
{
	return addChildInternal (childName, ObjectType::Array);
}

Node& Node::addChildArray (const Array& value, std::string_view childName)
{
	auto& child = addChildArray (childName);

	child.getArray() = value;

	return child;
}

bool Node::isObject() const noexcept
{
	return type == ObjectType::Object;
}

Object& Node::getObject()
{
	if (! isObject())
		throw std::runtime_error { "getObject(): Node is not an Object!" };

	return std::get<Object> (data);
}

const Object& Node::getObject() const
{
	if (! isObject())
		throw std::runtime_error { "getObject(): Node is not an Object!" };

	return std::get<Object> (data);
}

Node& Node::operator= (const Object& value)
{
	getObject() = value;
	return *this;
}

Node& Node::addChildObject (std::string_view childName)
{
	return addChildInternal (childName, ObjectType::Object);
}

Node& Node::addChildObject (const Object& value, std::string_view childName)
{
	auto& child = addChildObject (childName);

	child.getObject() = value;

	return child;
}

template <typename Type>
Type& Node::get()
{
	return std::get<Type> (data);
}

template double&	  Node::get<double>();
template std::string& Node::get<std::string>();
template bool&		  Node::get<bool>();
template Array&		  Node::get<Array>();
template Object&	  Node::get<Object>();

bool Node::isNull() const noexcept
{
	return type == ObjectType::Null;
}

Node& Node::addChildNull (const std::string_view& childName)
{
	return addChildInternal (childName, ObjectType::Null);
}

Node* Node::getParent() const noexcept
{
	return parent;
}

Node& Node::getRoot() const noexcept
{
	if (parent == nullptr)
		return *this;

	auto* curr = parent;

	while (curr->parent != nullptr)
	{
		curr = curr->parent;
	}

	return *curr;
}

bool Node::hasParent() const noexcept
{
	return parent != nullptr;
}

bool Node::hasName() const noexcept
{
	if (parent == nullptr)
		return false;

	return parent->isObject();
}

std::string_view Node::getName() const noexcept
{
	if (! hasName())
		return "";

	for (const auto& pair : std::get<Object> (parent->data))
		if (&pair.second == this)  // cppcheck-suppress useStlAlgorithm
			return pair.first;

	return "";
}

Node Node::createNumber (double value)
{
	auto result = Node { ObjectType::Number };

	result.getNumber() = value;

	return result;
}

Node Node::createString (const std::string_view& value)
{
	auto result = Node { ObjectType::String };

	result.getString() = value;

	return result;
}

Node Node::createBoolean (bool value)
{
	auto result = Node { ObjectType::Boolean };

	result.getBoolean() = value;

	return result;
}

Node Node::createNull()
{
	return Node { ObjectType::Null };
}

}  // namespace limes::serializing

namespace std
{

size_t hash<limes::serializing::Node>::operator() (const limes::serializing::Node& n) const noexcept
{
	namespace Serial = limes::serializing;

	return hash<string> {}(Serial::KnownFormats::get()
							   .getFormatForString (Serial::formats::JSON)
							   ->createPrinter (false)
							   ->print (n));
}

size_t hash<limes::serializing::SerializableData>::operator() (const limes::serializing::SerializableData& d) const noexcept
{
	return hash<limes::serializing::Node> {}(d.serialize());
}

}  // namespace std
