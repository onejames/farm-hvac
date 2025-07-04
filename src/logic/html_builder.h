#ifndef HTML_BUILDER_H
#define HTML_BUILDER_H

#include "hvac_data.h" // For String type

// Forward declaration
struct HVACData;

class HtmlBuilder {
public:
    static String build();
};
#endif // HTML_BUILDER_H