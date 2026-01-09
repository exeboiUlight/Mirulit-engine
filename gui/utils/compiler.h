#include "../../tcc.h"
#include "../../core/utils/json.h"

int compile() {

    JsonValue CompileSettings = json_parse_file("compile.settings");

    if (CompileSettings.type == JSON_NULL) {
        printf("Faled parsing compile settings");
        return 1;
    }

    json_print(&CompileSettings, 0);

    json_free(&CompileSettings);

    return 0;
}