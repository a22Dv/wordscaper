/**
 * profiling.hpp
 * Profiling-related declarations and macros.
 */

#include "utilities/config.hpp"

#ifdef WSR_PROFILE

#include <tracy/Tracy.hpp>

#endif

#ifdef WSR_PROFILE

#define WSR_PROFILESCOPE() ZoneScopedN(__func__)
#define WSR_PROFILESCOPEN(name) ZoneScopedN(name)

#else

#define WSR_PROFILESCOPE()
#define WSR_PROFILESCOPEN(name)

#endif