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

#include "lserializing/lserializing_Export.h"

/** @file
	This file defines the serializing::Schema class.

	@ingroup limes_serializing
 */

namespace limes::serializing
{

class Node;

/** This class represents a schema specification for a certain kind of
	data in a certain serialization format -- for example, a JSON schema
	or XML schema. This class can be used to verify data as it is being
	parsed.

	@see Format
	@ingroup limes_serializing
	@todo docs
 */
class LSERIAL_EXPORT Schema
{
public:
	virtual ~Schema() = default;

	virtual bool validateNext (const Node&) = 0;

private:
};

}  // namespace limes::serializing
