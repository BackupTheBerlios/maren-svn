#include <stddef.h>
#include <mpll/plugin_interface.h>

static char some_data[] = "Hello, world!";

MPLL_BEGIN_DATA
MPLL_DATA( some_data )
MPLL_END_DATA

static bool
foo( void* data, const void* fact )
{

}

MPLL_BEGIN_SINGLE_CHECKS
MPLL_SINGLE_CHECK( foo )
MPLL_END_SINGLE_CHECKS
