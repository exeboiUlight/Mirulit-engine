#pragma once

#include <libtcc.h>
#include <iostream>
#include "../core/core.h"

namespace MirulitGUI {
    class Jit {
        private:
            TCCState* state;

            typedef int (*main_function)();

            void MirulitInit() {
                
            }
        
        public:
            Jit() {
                state = tcc_new();

                if (!state) {
                    std::cout << "Error in tcc init";
                }

                tcc_set_lib_path(state, "assets/lib");
                tcc_add_include_path(state, "assets/include");
                tcc_add_sysinclude_path(state, "assets/include");

                tcc_add_library(state, "c");
                tcc_add_library(state, "m");

                tcc_set_output_type(state, TCC_OUTPUT_MEMORY);

                const char* code = R"(
                    #include "project.h"

                    int main() {
                        return 0;
                    }
                )";

                tcc_compile_string(state, code);

                if (tcc_relocate(state) < 0) {
                    std::cout << "Error in tcc compile";
                }

                main_function _main = (main_function)tcc_get_symbol(state, "main");

                _main();

            }

            ~Jit() {
                tcc_delete(state);
            }
    };

    class Build {};
}