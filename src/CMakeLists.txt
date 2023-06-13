# ======================================================================================
#  __    ____  __  __  ____  ___
# (  )  (_  _)(  \/  )( ___)/ __)
#  )(__  _)(_  )    (  )__) \__ \
# (____)(____)(_/\/\_)(____)(___/
#
#  This file is part of the Limes open source library and is licensed under the terms of the GNU Public License.
#
#  Commercial licenses are available; contact the maintainers at ben.the.vining@gmail.com to inquire for details.
#
# ======================================================================================

target_sources (
    lserializing
    PRIVATE # lserializing_Formats.cpp # lserializing_INI.cpp lserializing_JSON.cpp
            # lserializing_KnownFormats.cpp
            lserializing_Node.cpp
    # lserializing_Printer.cpp lserializing_TOML.cpp lserializing_XML.cpp lserializing_YAML.cpp
    )
