# dark-kill
A user-mode code and its rootkit that will Kill EDR Processes permanently by leveraging the power of Process Creation Blocking Kernel Callback Routine registering and ZwTerminateProcess. 

## Setup

### Building the client

To compile the client, you will need to have [Visual Studio 2022](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=16) installed and then just build the project like any other Visual Studio project.

### Building the driver

To compile the project, you will need the following tools:

- [Visual Studio 2022](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=16)
- [Windows Driver Kit](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)
- In the project properties, go to “Linker > Command Line” and add the following option which is going to disable the integrity check:
<img width="384" alt="Image" src="https://github.com/user-attachments/assets/54744e1a-0fb3-4e3b-8498-7b9a28d68bcb" />


Clone the repository and build the driver.

### Driver Testing

To test it in your testing environment run those commands with elevated cmd:

```cmd
bcdedit /set testsigning on
```

After rebooting, create a service and run the driver:

```cmd
sc create dark type= kernel binPath= C:\Path\To\Driver\dark.sys
sc start dark
```


## Demo Video
https://github.com/user-attachments/assets/6364a1bc-aa91-4b88-b5b8-4f6944b10078
