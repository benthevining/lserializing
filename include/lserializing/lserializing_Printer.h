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
#include "lserializing/lserializing_Export.h"
#include "lserializing/lserializing_Node.h"

/** @file
	This file defines the serializing::Printer class.

	@ingroup limes_serializing
 */

namespace limes::serializing
{

/** This class prints some data into a string in a specific serialization
	format, and offers an API for controlling pretty printing, etc.

	Formats can define their own printer types, and users can also choose to
	create a custom printer to use instead of a format's default printer.

	Printer objects should generally be temporary; you should create one, call
	its \c print() method to get your string, and then create a new printer
	object the next time you need to serialize to a string.

	@ingroup limes_serializing

	@see Format
 */
class LSERIAL_EXPORT Printer
{
public:
	/** Destructor. */
	virtual ~Printer() = default;

	/** Serializes the \c Node into a string, using the formatting rules of this
		printer.

		The default implementation traverses the tree of Nodes and calls all of the
		private virtual methods to print each node in the tree, but implementations
		can override this method to provide entirely custom behavior. Note that if
		you override this, you are responsible for calling \c arrayBegin() and friends,
		they normally get called by the base class's implementation of this method.
	 */
	[[nodiscard]] virtual std::string print (const Node& node);

private:
	/** Must output a representation of a "null" Node. */
	virtual std::string printNull() = 0;

	/** Must print the given number. */
	virtual std::string printNumber (double) = 0;

	/** Must print the given string. */
	virtual std::string printString (const std::string_view&) = 0;

	/** Must print the given boolean. */
	virtual std::string printBoolean (bool) = 0;

	/** Must print the given array.

		@see arrayBegin(), arrayEnd()

		@todo provide a default implementation of this?
	 */
	virtual std::string printArray (const Array&) = 0;

	/** Must print the given object.

		@see objectBegin(), objectEnd()

		@todo provide a default implementation of this?
	 */
	virtual std::string printObject (const Object&) = 0;

	/** Called before \c printArray() ; implementations can override this to be
		informed when they're about to begin printing an array.

		@see arrayEnd(), printArray()
	 */
	virtual void arrayBegin() { }

	/** Called after \c printArray() ; implementations can override this to be
		informed when they've just finished printing an array.

		@see arrayBegin(), arrayBegin()
	 */
	virtual void arrayEnd() { }

	/** Called before \c printObject() ; implementations can override this to be
		informaed when they're about to being printing an object.

		@see objectEnd(), printObject()
	 */
	virtual void objectBegin() { }

	/** Called after \c printObject() ; implementations can override this to be
		informed when they've just finished printing an object.

		@see objectBegin(), printObject()
	 */
	virtual void objectEnd() { }
};

}  // namespace limes::serializing
