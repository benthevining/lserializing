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

/** @defgroup limes_serializing limes_serializing
	Serialization utilities.

	@anchor lib_limes_serializing

	@tableofcontents{HTML,LaTeX,XML}

	@section limes_serializing_overview Overview

	All classes and functions in this module are accessible after linking to the \c limes::lserializing
	library and including \c lserializing.h.

	This library provides a class for representing data in an arbitrary tree structure, and provides functions
	for converting these objects to and from various serialization formats. The formats themselves are treated
	in an object oriented way, and this library can be easily extended with custom formats.

	@section limes_serializing_design Design goals

	This serialization library has two primary tasks:
	- To provide a well-structured way to encode any C++ object as an arbitrary tree-like data structure. This
	type erasure is necessary to work with the data from a high level while allowing interoperability with any
	arbitrary C++ type.
	- To provide ways to serialize and deserialize these tree structures to and from strings encoded in different
	serialization formats. This is the end goal of the library, and goal #1 is simply a means to this end.

	The first task is accomplished with the \c serializing::Node class. This class is designed around the JSON
	specification; any \c Node can be a null, number, boolean, string, array of other Nodes, or an Object, which
	is a map of key-value pairs (the keys are strings, and the values are any other kind of \c Node value). This
	allows a \c Node object to represent any C++ object or complex data structure. This library's API is designed
	to easily allow converting any arbitrary C++ type to and from a \c Node in a deterministic, reproducible way.
	The two primary parts of this conversion API are the \c SerializableData base class and the \c NodeConverter
	template. For simple objects, your data class needs no knowledge of this serializing library at all; you can
	simply implement a \c NodeConverter specialization for your type. For more complex data structures, or if the
	serialization logic is fairly involved, your type can inherit from \c SerializableData instead. Only one of
	these is needed to allow your types to be integrated with this library's API. See the docs for \c Node ,
	\c SeriaizableData , and \c NodeConverter for more information and examples.

	The second task is done using the \c serializing::Format class. This base class represents a serialization format
	(such as JSON, XML, etc) and defines several methods for parsing a string to a \c Node , encoding a \c Node as a
	string, etc. This library provides several built in formats, but custom formats can be easily added by implementing
	the \c Format interface. Actual \c Format objects are accessed through the \c KnownFormats class. The \c KnownFormats
	singleton maintains a set of available formats, and provides methods for selecting a specific one, or for finding
	the likely best fit for some input, etc. Through the \c KnownFormats interface, third party code can easily define
	new \c Format types and register them with the global \c KnownFormats object, allowing them to be used by the Limes
	code itself as if these custom formats were built in to Limes.

	@section limes_serializing_features Features

	This library includes the following serialization formats:
	- JSON
	- XML
	- YAML
	- TOML
	- INI

	This library is easily extendable, both by application code or by other middleware libraries.

	@section limes_serializing_examples Examples

	// TODO


	@todo JSON encoder - options for pretty printing
	@todo XML parser, encoder
	@todo YAML parser, encoder
	@todo TOML parser, encoder
	@todo INI parser, encoder
	@todo Parser base class?
	@todo add supportsSchema() to format class
	@todo JSON/XML validate schema
	@todo write unit tests for encoding/decoding all formats
	@todo API for preserving and accessing comments in formats that support it?
	@todo SerializedNumber - store the value as a double, but retain info about number of decimal places, etc
	@todo MusicXML wrapper library
	@todo nlohmann json integration
	@todo protobuf integration
	@todo docs
	@todo make all format classes public and document them
	@todo CLI tool for converting serialization formats
 */

/** @file
	The main header for the @ref lib_limes_serializing "limes_serializing" library.

	@ingroup limes_serializing
 */

/** @namespace limes::time
	Serialization utilities.

	This namespace contains all code of the @ref lib_limes_serializing "limes_serializing" library.

	@ingroup limes_serializing
 */

#pragma once

// IWYU pragma: begin_exports
#include "./lserializing_Enums.h"
// #include "./lserializing_KnownFormats.h"
#include "./lserializing_Node.h"
#include "./lserializing_Printer.h"
#include "./lserializing_Schema.h"
#include "lserializing_SerializableData.h"
// #include "lserializing_SerializingFormat.h"
// IWYU pragma: end_exports
