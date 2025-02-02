# Storj Tools Service

Storj Tools Service is an automated service designed for managing and maintaining the decentralized storage system Storj.
The service reduces log files, regularly checks the status and versions of nodes, and attempts to restore any non-functional nodes.
If the restoration fails, it automatically sends a warning message via private message on Discord.

The service is fully automated and configurable through the config.yaml file or GUI(currently in development).
Changes in config.yaml will take effect upon the execution of any of the service tasks.
If you want to apply the changes immediately, restart the service.

## Features
- Supports multiple Storj nodes on a single server
- Log file reduction
- Node updates
- Node health checks

## Configuration

The service uses a config.yaml file for its configuration, located in the same directory where the service is installed, by default: "C:\Program Files\Storj Tools" , or can be configured through the GUI (currently in development).
Below is an example of the configuration file:

- `Reduce Log Interval`: The interval in hours at which the log reduction process will be executed. (Default: 24)
- `Max Log File Size`: The maximum size of the log file in gigabytes before reduction is triggered. (Default: 5)
- `Reduce Log File to Size`: The size to which the log file will be reduced. (Default: 0.5)
- `Check StorjNodes Interval`: The interval in hours at which the status of all StorjNodes will be checked. This process verifies whether the nodes are running and active. (Default: 1)
- `Discord User ID`: The ID of the Discord user to whom the messages will be sent. (Default: "YOUR_USER_ID")
- `Discord Bot Token`: The token of the Discord bot that will send messages to the user. (Default: "YOUR_BOT_TOKEN")
- `Update StorjNode Interval`: The interval in hours at which the update status of StorjNodes will be checked. This process verifies that the nodes are updated to the latest version. (Default: 12)

## License

This project is licensed under the MIT License. See the [![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) file for details.

## Installation

1. Download the installer from the [releases page](https://github.com/Akutapi/StorJLogService/releases).
	1. ⚠ Note: The installer uses a private certificate for signing, so it may be flagged as untrusted software.
	1. To proceed, you might need to manually confirm the installation.
2. Run the installer and follow the on-screen instructions to complete the installation.
3. The installer will automatically set up and start the Storj Tools Service.

## Usage

1. Ensure the `config.yaml` file is properly configured.
2. The service will start automatically after installation. You can manage the service using standard service management tools.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## Contact

For any questions or issues, please open an issue on GitHub.

   