#include <assert.h>
#include <HD\hd.h>
#include <HDU\hdu.h>
#include <HDU\hduError.h>
#include <HDU\hduVector.h>

static HHD ghHD = HD_INVALID_HANDLE;
static HDSchedulerHandle hUpdateDeviceCallback = HD_INVALID_HANDLE;

static HDdouble workspaceModel[16];

static double kCursorScreenSize = 20.0;
static GLuint gCursorDisplayList = 0;
static double gCursorScale = 1.0;

struct HapticDisplayState
{
	hduVector3Dd position;
	HDdouble transform[16];
};