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

#include <sstream>
#include "lserializing/lserializing_Printer.h"
#include "lserializing/lserializing_Node.h"

namespace limes::serializing
{

std::string Printer::print (const Node& node)
{
	if (node.isNull())
		return printNull();

	if (node.isNumber())
		return printNumber (node.getNumber());

	if (node.isString())
		return printString (node.getString());

	if (node.isBoolean())
		return printBoolean (node.getBoolean());

	if (node.isArray())
	{
		arrayBegin();

		const auto str = printArray (node.getArray());

		arrayEnd();

		return str;
	}

	objectBegin();

	const auto str = printObject (node.getObject());

	objectEnd();

	return str;
}

}  // namespace limes::serializing
