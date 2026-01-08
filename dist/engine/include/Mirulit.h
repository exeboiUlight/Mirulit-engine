#ifdef _WIN32
    #ifdef MIRULIT_EXPORTS
        #define MIRULIT_API __declspec(dllexport)
    #else
        #define MIRULIT_API __declspec(dllimport)
    #endif
#else
    #define MIRULIT_API
#endif