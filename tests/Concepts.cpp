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

namespace serial = limes::serializing;

struct Serializable : public serial::SerializableData
{
	serial::Node serialize() const final
	{
		return serial::Node::createNull();
	}

	void deserialize (const serial::Node&) final
	{
	}
};

static_assert (serial::ImplementsSerializableData<Serializable>);
static_assert (serial::CanSerialize<Serializable>);

struct AlsoSerializable final
{
	serial::Node serialize() const
	{
		return serial::Node::createNull();
	}

	void deserialize (const serial::Node&)
	{
	}
};

static_assert (serial::ImplementsSerializableData<AlsoSerializable>);
static_assert (serial::CanSerialize<AlsoSerializable>);

// check that wrong return type of serialize() doesn't satisfy concept
struct WrongReturnType final
{
	int serialize() const
	{
		return 0;
	}

	void deserialize (const serial::Node&)
	{
	}
};

static_assert (! serial::ImplementsSerializableData<WrongReturnType>);
// static_assert (! serial::CanSerialize<WrongReturnType>);

struct NotDirectlySerializable final
{
};

static_assert (! serial::ImplementsSerializableData<NotDirectlySerializable>);

template <>
struct serial::NodeConverter<NotDirectlySerializable> final
{
	[[nodiscard]] static Node serialize (const NotDirectlySerializable&)
	{
		return serial::Node::createNull();
	}

	static void deserialize (const Node&, NotDirectlySerializable&)
	{
	}
};

static_assert (serial::CanSerialize<NotDirectlySerializable>);

// all enum types should be serializable due to the library's built-in enum NodeConverter
static_assert (serial::CanSerialize<serial::ObjectType>);
