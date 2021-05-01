// Generated automatically from src/modules/*.wren. Do not edit.
static const char* essentialsModuleSource =
"#module=essentials\n"
"class Strings {\n"
"    static upcase(s) {\n"
"        return s.bytes.map { |x|\n"
"            if ((97..122).contains(x))  x = x - 32\n"
"            return String.fromByte(x)\n"
"        }.join(\"\")\n"
"    }\n"
"    static downcase(s) {\n"
"        return s.bytes.map { |x|\n"
"            if ((65..90).contains(x))  x = x + 32\n"
"            return String.fromByte(x)\n"
"        }.join(\"\")\n"
"    }\n"
"}\n"
"\n\n"
"#module=essentials\n"
"class Time {\n"
"    foreign static now()\n"
"    foreign static highResolution()\n"
"}";

// Generated automatically from src/modules/*.wren. Do not edit.
static const char* timeModuleSource =
"#module=time\n"
"class Delorian {\n"
"\n"
"}";

