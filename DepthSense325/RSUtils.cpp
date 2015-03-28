#include "RSUtils.h"

#include <iostream>

namespace mobamas {

void ReportPxcBadStatus(const pxcStatus& status) {
	switch (status) {
	case PXC_STATUS_NO_ERROR:
		std::cout << "PXC_STATUS_NO_ERROR: Indicates the operation succeeded without any warning" << std::endl;
		break;
	case PXC_STATUS_FEATURE_UNSUPPORTED:
		std::cout << "PXC_STATUS_FEATURE_UNSUPPORTED Unsupported feature" << std::endl;
		break;
	case PXC_STATUS_PARAM_UNSUPPORTED:
		std::cout << "PXC_STATUS_PARAM_UNSUPPORTED Unsupported parameter(s)" << std::endl;
		break;
	case PXC_STATUS_ITEM_UNAVAILABLE:
		std::cout << "PXC_STATUS_ITEM_UNAVAILABLE Item not found/not available" << std::endl;
		break;

    case PXC_STATUS_HANDLE_INVALID:
		std::cout << "PXC_STATUS_HANDLE_INVALID Invalid session, algorithm instance, or pointer" << std::endl;
		break;
    case PXC_STATUS_ALLOC_FAILED:
		std::cout << "PXC_STATUS_ALLOC_FAILED Memory allocation failure" << std::endl;
		break;

    case PXC_STATUS_DEVICE_FAILED:
		std::cout << "PXC_STATUS_DEVICE_FAILED device failed due to malfunctioning" << std::endl;
		break;
    case PXC_STATUS_DEVICE_LOST:
		std::cout << "PXC_STATUS_DEVICE_LOST device failed due to unplug or unavailability" << std::endl;
		break;
    case PXC_STATUS_DEVICE_BUSY:
		std::cout << "PXC_STATUS_DEVICE_BUSY device busy" << std::endl;
		break;

    case PXC_STATUS_EXEC_ABORTED:
		std::cout << "PXC_STATUS_EXEC_ABORTED Execution aborted due to errors in upstream components" << std::endl;
		break;
    case PXC_STATUS_EXEC_INPROGRESS:
		std::cout << "PXC_STATUS_EXEC_INPROGRESS Asynchronous operation is in execution" << std::endl;
		break;
    case PXC_STATUS_EXEC_TIMEOUT:
		std::cout << "PXC_STATUS_EXEC_TIMEOUT Operation time out" << std::endl;
		break;

    case PXC_STATUS_FILE_WRITE_FAILED:
		std::cout << "PXC_STATUS_FILE_WRITE_FAILED Failure in open file in WRITE mode" << std::endl;
		break;
    case PXC_STATUS_FILE_READ_FAILED:
		std::cout << "PXC_STATUS_FILE_READ_FAILED Failure in open file in READ mode" << std::endl;
		break;
    case PXC_STATUS_FILE_CLOSE_FAILED:
		std::cout << "PXC_STATUS_FILE_CLOSE_FAILED Failure in close a file handle" << std::endl;
		break;

    case PXC_STATUS_DATA_UNAVAILABLE:
		std::cout << "PXC_STATUS_DATA_UNAVAILABLE Data not available for MW model or processing" << std::endl;
		break;

    case PXC_STATUS_DATA_NOT_INITIALIZED:
		std::cout << "PXC_STATUS_DATA_NOT_INITIALIZED Data failed to initialize" << std::endl;
		break;
    case PXC_STATUS_INIT_FAILED:
		std::cout << "PXC_STATUS_INIT_FAILED Module failure during initialization" << std::endl;
		break;

    case PXC_STATUS_STREAM_CONFIG_CHANGED:
		std::cout << "PXC_STATUS_STREAM_CONFIG_CHANGED Configuration for the stream has changed" << std::endl;
		break;

    case PXC_STATUS_POWER_UID_ALREADY_REGISTERED:
		std::cout << "PXC_STATUS_POWER_UID_ALREADY_REGISTERED" << std::endl;
		break;
	case PXC_STATUS_POWER_UID_NOT_REGISTERED:
		std::cout << "PXC_STATUS_POWER_UID_NOT_REGISTERED" << std::endl;
		break;
	case PXC_STATUS_POWER_ILLEGAL_STATE:
		std::cout << "PXC_STATUS_POWER_ILLEGAL_STATE" << std::endl;
		break;
	case PXC_STATUS_POWER_PROVIDER_NOT_EXISTS:
		std::cout << "PXC_STATUS_POWER_PROVIDER_NOT_EXISTS" << std::endl;
		break;

    case PXC_STATUS_CAPTURE_CONFIG_ALREADY_SET:
		std::cout << "PXC_STATUS_CAPTURE_CONFIG_ALREADY_SET parameter cannot be changed since configuration for capturing has been already set" << std::endl;
		break;
	case PXC_STATUS_COORDINATE_SYSTEM_CONFLICT :
		std::cout << "PXC_STATUS_COORDINATE_SYSTEM_CONFLICT  Mismatched coordinate system between modules" << std::endl;
		break;

    /* warnings */
    case PXC_STATUS_TIME_GAP:
		std::cout << "PXC_STATUS_TIME_GAP [WARNING] time gap in time stamps" << std::endl;
		break;
    case PXC_STATUS_PARAM_INPLACE:
		std::cout << "PXC_STATUS_PARAM_INPLACE [WARNING] the same parameters already defined" << std::endl;
		break;
    case PXC_STATUS_DATA_NOT_CHANGED:
		std::cout << "PXC_STATUS_DATA_NOT_CHANGED [WARNING] Data not changed (no new data available)" << std::endl;
		break;
    case PXC_STATUS_PROCESS_FAILED:
		std::cout << "PXC_STATUS_PROCESS_FAILED [WARNING] Module failure during processing" << std::endl;
		break;
    case PXC_STATUS_VALUE_OUT_OF_RANGE:
		std::cout << "PXC_STATUS_VALUE_OUT_OF_RANGE [WARNING] Data value(s) out of range" << std::endl;
		break;
	}
}

}