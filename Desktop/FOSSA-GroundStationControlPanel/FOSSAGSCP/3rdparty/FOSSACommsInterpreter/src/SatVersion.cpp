#include "SatVersion.h"

SatVersion SatVersionFromString(std::string str) {
    SatVersion version = V_NA;

    if (str == "FOSSASAT-1B")
    {
        version = SatVersion::V_FOSSASAT1B;
    }
    else if (str == "FOSSASAT-2")
    {
        version = SatVersion::V_FOSSASAT2;
    }

    return version;
}

std::string SatVersionToString(SatVersion version) {
    std::string str;
    switch (version)
    {
    case V_NA:
        str = "Unknown version";
        break;
    case V_FOSSASAT1B:
        str = "FOSSASAT-1B";
        break;
    case V_FOSSASAT2:
        str = "FOSSASAT-2";
        break;
    }
    return str;
}
