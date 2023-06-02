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
#include "lserializing/lserializing_SerializableData.h"
#include "lserializing/lserializing_Export.h"

/** @file
	This file defines the Node class.

	@ingroup limes_serializing
 */

namespace limes::serializing
{

/** Represents the type of a serialization node.
	These types align with the JSON specification.

	@see Node, DataType
	@ingroup limes_serializing
 */
LSERIAL_EXPORT enum class ObjectType {
	Number = 0,	 ///< A number. Numbers are always represented using doubles.
	String,		 ///< A string. Strings are stored using \c std::string.
	Boolean,	 ///< A boolean.
	Array,		 ///< An array of other objects. Arrays can contain any kind of child ObjectTypes.
	Object,		 ///< An Object is a collection of key/value pairs, where the keys are string identifiers and the values are any ObjectType.
	Null		 ///< Represents a null or nonexistent value or node.
};

class Node;

/** The representation of Array types.

	@ingroup limes_serializing
 */
LSERIAL_EXPORT using Array = std::vector<Node>;

/** The representation of Object types.

	@ingroup limes_serializing
 */
LSERIAL_EXPORT using Object = std::map<std::string, Node>;

/** A special empty type to represent a null object.

	@ingroup limes_serializing
 */
struct LSERIAL_EXPORT NullType final
{
};

/** This typedef evaluates to the appropriate C++ object type for the given ObjectType.

	The types evaluate as follows:

	ObjectType    | C++ type
	------------- | -------------------------------
	\c Number     | \c double
	\c String     | \c std::string
	\c Boolean    | \c bool
	\c Array      | \c std::vector<Node>
	\c Object     | \c std::map<std::string,Node>
	\c Null       | \c NullType

	@ingroup limes_serializing
	@see ObjectType, Node
 */
template <ObjectType Type>
using DataType = std::conditional_t<Type == ObjectType::Number, double,
												 std::conditional_t<Type == ObjectType::String, std::string,
																	std::conditional_t<Type == ObjectType::Boolean, bool,
																					   std::conditional_t<Type == ObjectType::Array, Array,
																										  std::conditional_t<Type == ObjectType::Object, Object, NullType>>>>>;

#pragma mark Node

/** Represents any kind of serializable object.

	This class is designed around the JSON specification, but can also be parsed from or serialized to other formats, like XML.

	This class is called "node" because it represents a single node of a serializable tree structure. Nodes of type \c Array or
	\c Object can hold child nodes. The entire tree is traversable via the root node object.

	A Node object must be assigned an ObjectType upon construction, which will be constant for its lifetime.

	Node objects should be created on demand when serializing a data structure; I wouldn't use them as your object's primary
	storage of member data, though it is possible to do so.

	The basic mechanics of achieving serializing with this library are to convert your object into a \c Node in order to serialize
	it, and to convert a \c Node back in to your object to deserialize it.

	Node's object storage is implemented using \c std::variant. You can conceptualize this class as a wrapper around a variant
	with an API designed for easy construction and parsing of the tree structure it's a part of.

	@ingroup limes_serializing

	@see ObjectType, DataType, NodeConverter

	@todo refactor -- don't need to store ObjectType, just query which type the variant is holding?
 */
class LSERIAL_EXPORT Node final
{
public:
	/** Creates a Node with a specified type. */
	explicit Node (ObjectType typeToUse) noexcept;

	/** Creates a Null Node. */
	Node() = default;

	/** @name Copying */
	///@{
	/** Copy constructor. */
	Node (const Node& other);

	/** Copy assignment operator. */
	Node& operator= (const Node& other);
	///@}

	/** @name Moving */
	///@{
	/** Move constructor. */
	Node (Node&& other) noexcept;

	/** Move assignment operator. */
	Node& operator= (Node&& other) noexcept;
	///@}

	/** @name Subscript operators */
	///@{

	/** For Object nodes, finds and returns the child %node with the specified name.

		@throws std::runtime_error An exception is thrown if the node is not an Object, or if no child %node
		with the specified name exists.
	 */
	Node& operator[] (std::string_view childName);

	/** For Array nodes, returns the child %node at the given index in the array.

		@throws std::runtime_error An exception is thrown if the node is not an Array.

		@throws std::out_of_range An exception is thrown if the requested index is out of range of the array.
	 */
	Node& operator[] (size_t idx);

	///@}

	/** @name Querying child nodes */
	///@{

	/** For arrays or objects, returns the number of child nodes this %node contains.
		If this %node is not an array or object, returns 0.
	 */
	size_t getNumChildren() const noexcept;

	/** For Object nodes, returns true if this %node has a child with the specified name.
		If this %node is not an object, this always returns false.
	 */
	bool hasChildWithName (std::string_view childName) const noexcept;

	///@}

	/** @name Querying parent nodes */
	///@{

	/** Returns this node's parent %node, if it has one.
		This may return nullptr.

		@see hasParent()
	 */
	Node* getParent() const noexcept;

	/** Returns true if this %node is a child of another %node.

		@see getParent()
	 */
	bool hasParent() const noexcept;

	/** Traverses all parent nodes until a %node is found that does not have a parent.
		If this %node doesn't have a parent, returns a reference to this %node.
	 */
	Node& getRoot() const noexcept;

	///@}

	/** @name Querying this node's name */
	///@{

	/** Returns true if this %node is a child of an Object %node.

		@see getName()
	 */
	bool hasName() const noexcept;

	/** Returns this node's name.
		If \c hasName() returns false, this will return an empty string.

		@see hasName()
	 */
	std::string_view getName() const noexcept;

	///@}

	/** @name Type queries */
	///@{

	/** Returns true if this %node is a number. */
	bool isNumber() const noexcept;

	/** Returns true if this %node is a string. */
	bool isString() const noexcept;

	/** Returns true if this %node is a boolean. */
	bool isBoolean() const noexcept;

	/** Returns true if this %node is an array. */
	bool isArray() const noexcept;

	/** Returns true if this %node is an object. */
	bool isObject() const noexcept;

	/** Returns true if this %node is null. */
	bool isNull() const noexcept;

	/** Returns the type of this %node. */
	ObjectType getType() const noexcept;

	/** Returns a string representation of this node's type. */
	[[nodiscard]] std::string_view getTypeAsString() const noexcept;

	///@}

	/** @name Member accessors */
	///@{

	/** Returns a reference to the internal number object stored by this %node.

		@throws std::runtime_error An exception will be thrown if this Node is not a Number.
	 */
	double&				 getNumber();
	[[nodiscard]] double getNumber() const;

	/** Returns a reference to the internal string object stored by this %node.

		@throws std::runtime_error An exception will be thrown if this Node is not a String.
	 */
	std::string&				   getString();
	[[nodiscard]] std::string_view getString() const;

	/** Returns a reference to the internal boolean object stored by this %node.

		@throws std::runtime_error An exception will be thrown if this Node is not a Boolean.
	 */
	bool&			   getBoolean();
	[[nodiscard]] bool getBoolean() const;

	/** Returns a reference to the internal array object stored by this %node.

		@throws std::runtime_error An exception will be thrown if this Node is not an Array.
	 */
	Array&		 getArray();
	const Array& getArray() const;

	/** Returns a reference to the internal object stored by this %node.

		@throws std::runtime_error An exception will be thrown if this Node is not an Object.
	 */
	Object&		  getObject();
	const Object& getObject() const;

	/** Templated function that accesses the stored data of the specified type.
		If you attempt to access a datatype that this Node isn't currently storing, an exception will be thrown.
	 */
	template <typename Type>
	Type& get();

	/** Access the stored data, templated based upon the ObjectType enum instead of the underlying C++ type.
		If you attempt to access a datatype that this Node isn't currently storing, an exception will be thrown.
	 */
	template <ObjectType Type>
	DataType<ObjectType>& get()
	{
		static_assert (! std::is_same_v<Type, Null>);
		return get<DataType<ObjectType>>();
	}

	///@}

	/** @name Assignment operators */
	///@{

	/** Assigns this %node to a new number value.

		@throws std::runtime_error An exception will be thrown if this Node is not a Number.
	 */
	Node& operator= (double value);

	/** Assigns this %node to a new string value.

		@throws std::runtime_error An exception will be thrown if this Node is not a String.
	 */
	Node& operator= (std::string_view value);

	/** Assigns this %node to a new boolean value.

		@throws std::runtime_error An exception will be thrown if this Node is not a Boolean.
	 */
	Node& operator= (bool value);

	/** Assigns this %node to a new Array value.

		@throws std::runtime_error An exception will be thrown if this Node is not an Array.
	 */
	Node& operator= (const Array& value);

	/** Assigns this %node to a new Object value.

		@throws std::runtime_error An exception will be thrown if this Node is not an Object.
	 */
	Node& operator= (const Object& value);

	///@}

	/** @name Adding child nodes */
	///@{

	/** Adds a new child %node that holds a number value.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildNumber (std::string_view childName = "");

	/** Adds a new child %node that holds the specified number value.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildNumber (double value, std::string_view childName = "");

	/** Adds a new child %node that holds a string value.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildString (std::string_view childName = "");

	/** Adds a new child %node that holds a boolean value.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildBoolean (std::string_view childName = "");

	/** Adds a new child %node that holds the specified boolean value.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildBoolean (bool value, std::string_view childName = "");

	/** Adds a new child %node that holds an Array.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildArray (std::string_view childName = "");

	/** Adds a new child %node that holds the specified Array.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildArray (const Array& value, std::string_view childName = "");

	/** Adds a new child %node that holds an Object.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildObject (std::string_view childName = "");

	/** Adds a new child %node that holds the specified Object.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildObject (const Object& value, std::string_view childName = "");

	/** Adds a new child %node that is null.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChild()
	 */
	Node& addChildNull (std::string_view childName = "");

	/** Creates a new child %node of the specified type, as a child of this node.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChildNumber(), addChildString(), addChildBoolean(), addChildArray(), addChildObject(),
		addChildNull()
	 */
	Node& addChild (ObjectType childType, std::string_view childName = "");

	/** Adds the passed %node as a child of this one.
		Internally, a copy of the passed node will be created.
		Child nodes can only be added to Array or Object nodes.

		@returns A reference to the new child %node

		@throws std::runtime_error An exception is thrown if the Node is not an Object or an Array.
		When adding a child to an Object node, an exception will also be thrown if \c childName is
		empty or if the object already contains a child node with a duplicate name.

		@see addChildNumber(), addChildString(), addChildBoolean(), addChildArray(), addChildObject(),
		addChildNull()
	 */
	Node& addChild (const Node& childNode, std::string_view childName = "");

	///@}

	/** @name Creation functions

		@todo createArray(), createObject()
	 */
	///@{
	/** Creates a number Node. */
	[[nodiscard]] static Node createNumber (double value);

	/** Creates a string Node. */
	[[nodiscard]] static Node createString (std::string_view value);

	/** Creates a boolean Node. */
	[[nodiscard]] static Node createBoolean (bool value);

	/** Creates a null Node. */
	[[nodiscard]] static Node createNull();
	///@}

private:
	Node& addChildInternal (std::string_view childName, ObjectType childType);

	ObjectType type { ObjectType::Null };

	std::variant<double, std::string, bool, Array, Object> data;

	Node* parent { nullptr };
};

#pragma mark NodeConverter

/** This template class is used to convert specific types to and from Node objects.

	In order to compose types for serialization, they either need to inherit from
	\c SerializableData or there needs to be a specialization of this template for
	your type.

	This is a template interface, not a base clas. You should specialize this template
	for your type; see the specialization for the \c ImplementsSerializableData
	concept for an example.

	@see Node, SerializableData
	@ingroup limes_serializing
 */
template <typename Type>
struct LSERIAL_EXPORT NodeConverter final
{
public:
	/** Serializes the given object and returns a \c Node containing its state. */
	[[nodiscard]] static Node serialize (const Type&);

	/** Deserializes the \c Node and updates the given object with the new state. */
	static void deserialize (const Node&, Type&);
};

/** A template specialization for NodeConverter that serializes and deserializes
	any SerializableData object using the SerializableData API.

	@see SerializableData
	@ingroup limes_serializing
 */
template <ImplementsSerializableData Type>
struct LSERIAL_EXPORT NodeConverter<Type> final
{
	/** Calls \c SerializableData::serialize() on the given object to convert it to a \c Node . */
	[[nodiscard]] static Node serialize (const Type& t)
	{
		return t.serialize();
	}

	/** Calls \c SerializableData::deserialize() on the given object to restore its state from the given \c Node . */
	static void deserialize (const Node& n, Type& t)
	{
		t.deserialize (n);
	}
};

/** This concept evaluates to true for any type that inherits from \c SerializableData
	or has an available specialization of \c NodeConverter .

	@ingroup limes_serializing
	@see SerializableData, NodeConverter, ImplementsSerializableData
 */
template <typename T>
concept CanSerialize = ImplementsSerializableData<T> || requires (const Node& n, T& t)
{
	{ NodeConverter<T>::serialize() }; // TODO: verify return type is a Node
	{ NodeConverter<T>::deserialize (n, t) };
};

}  // namespace limes::serializing

namespace std
{

/** A specialization of \c std::hash for Node objects.
	The hash value is computed based on the JSON string created from the Node.

	@ingroup limes_serializing
	@see Node
 */
template <>
struct LSERIAL_EXPORT hash<limes::serializing::Node> final
{
	size_t operator() (const limes::serializing::Node& n) const noexcept;
};

/** A specialization of \c std::hash for SerializableData objects.
	The hash value is computed based on the JSON string created from serializing
	the object.

	@ingroup limes_serializing
	@see SerializableData, Node
 */
template<>
struct LSERIAL_EXPORT hash<limes::serializing::SerializableData> final
{
	size_t operator() (const limes::serializing::SerializableData& d) const noexcept;
};

}  // namespace std
