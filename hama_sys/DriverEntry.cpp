/*-----------------------------------------------------------------------------
 * DriverEntry.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * - 10.11.2010 created
**---------------------------------------------------------------------------*/
#include "DriverHeaders.h"
#include "DriverDebug.h"
#include "fc_drv_util.h"


//#include "arch.h"
#include "start_vm.h"

// 
// global
// 
PDEVICE_EXTENSION		g_dev_ext = NULL;

// nt dispatch functions
NTSTATUS	__stdcall DispatchDummy(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp);
NTSTATUS	__stdcall DispatchDeviceIoControl(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp);
VOID		__stdcall DispatchUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS	__stdcall DispatchCleanup(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp);


/**----------------------------------------------------------------------------
    \brief  DriverEntry

    \param  
    \return         
    \code
    \endcode        
-----------------------------------------------------------------------------*/
extern "C" 
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS            status;
	PDEVICE_OBJECT      deviceObject;
	UNICODE_STRING      ntName;
	UNICODE_STRING      win32Name;

	log_info
		"\n\n===============================================================================\n"\
		"driver Compiled at %s on %s \n"\
		"===============================================================================\n", 
		__TIME__, __DATE__
	log_end

	



    //> device 이름 생성
    RtlInitUnicodeString(&ntName, _nt_device_name);
    status = IoCreateDevice(
                    DriverObject, 
					sizeof(DEVICE_EXTENSION), 
					&ntName, 
					FILE_DEVICE_UNKNOWN, 
					FILE_DEVICE_SECURE_OPEN, 
					TRUE, 
					&deviceObject
                    );
    if (FALSE == NT_SUCCESS(status))
    {		    
        return status;
    }

	//> add your own initialization routine
	g_dev_ext = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;
	RtlZeroMemory(g_dev_ext, sizeof(DEVICE_EXTENSION));
	
    //> symbolic link 생성
    RtlInitUnicodeString(&win32Name, _dos_device_name);	
    status = IoCreateSymbolicLink ( &win32Name, &ntName);
	if (FALSE == NT_SUCCESS(status))
    {
        IoDeleteDevice( DriverObject ->DeviceObject );
		return status;
    }

    // initialize function pointers
    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) 
    {
	    DriverObject->MajorFunction[i] = DispatchDummy;
    }
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceIoControl;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DispatchCleanup;   
    DriverObject->DriverUnload = DispatchUnload;


	start_vm();

    g_dev_ext->initialized = true;
	return STATUS_SUCCESS;
}

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
NTSTATUS __stdcall DispatchDummy(
	IN  PDEVICE_OBJECT  DeviceObject,
	IN  PIRP            Irp
	)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp ->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;	
	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return STATUS_SUCCESS;
}

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
NTSTATUS 
__stdcall DispatchDeviceIoControl(
	IN  PDEVICE_OBJECT  DeviceObject,
	IN  PIRP            Irp
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(DeviceObject);
	
	/*
	ULONG_PTR bytes_returned = 0;
	
	NTSTATUS status = fc_drv_iocontrol(DeviceObject, Irp, &bytes_returned);
	if (TRUE != NT_SUCCESS(status))
	{
		log_err
			"fc_drv_iocontrol(DeviceObject=0x%08p, Irp=0x%08p) failed, status=0x%08x", 
			DeviceObject, Irp, status
		log_end
	}
	status = CompleteRequest(Irp, status, bytes_returned);
	*/
	
	Irp ->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;	
	IoCompleteRequest( Irp, IO_NO_INCREMENT );
	return status;
}

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
NTSTATUS 
__stdcall 
DispatchCleanup(
	IN  PDEVICE_OBJECT  DeviceObject,
	IN  PIRP            Irp
	)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	// do something
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	UNREFERENCED_PARAMETER(pdx);
	
	Irp ->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;	
	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	log_info "driver cleanup called successfully" log_end
	return STATUS_SUCCESS;
}


/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
VOID 
__stdcall 
DispatchUnload(
	IN  PDRIVER_OBJECT  DriverObject
	)
{
	UNICODE_STRING Win32NameString;
	PDEVICE_OBJECT DevObj = DriverObject->DeviceObject;

	// 
	// ADD DRIVER UNLOAD CODE
	// 

	VM_DispatchUnload(DriverObject);

	
	// symbolic link 제거 및 디바이스 객체 삭제
	//
	RtlInitUnicodeString (&Win32NameString , _dos_device_name);	
	IoDeleteSymbolicLink (&Win32NameString);	
	IoDeleteDevice( DevObj );
	log_info "driver unloaded successfully" log_end
}
