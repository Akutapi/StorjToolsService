﻿# Storj Tools Service

Storj Tools Service is an automated service designed to reduce log files for the decentralized storage system Storj.
The service is fully automatic and configurable via the `config.yaml` file.
The service updates its configuration upon startup, restart, or before performing log reduction.
Therefore, if the configuration change is not urgent, there is no need to restart the service.

## Features
- Automated log file reduction
- Configurable via `config.yaml`
- Supports multiple Stroj nodes on a single server

## Configuration

The service uses a `config.yaml` file for its configuration. Below is an example of the configuration file:

- `Max Log Size In GB`: The maximum size of the log file in gigabytes before reduction is triggered. (Default: 5 Gb)
- `Reduce Log Size In GB`: The size to which the log file will be reduced. (Default: 0.5 Gb)
- `Reduce Log Time In Hours`: The interval in hours at which the log reduction process will be executed. (Default: 24H)
- `Check StrojNodes Time In Hours`: The interval in hours at which the status of all StrojNodes will be checked. This process verifies whether the nodes are running and active. (Default: 1H)
                                   (The methods for reporting the status of nodes have not been implemented yet.)
- `Check StrojNode Update Time In Hours`: The interval in hours at which the update status of StrojNodes will be checked. This process verifies that the nodes are updated to the latest version. (Default: 12H)

## License

This project is licensed under the MIT License. See the [![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) file for details.

## Installation

1. Download the installer from the [releases page](https://github.com/Akutapi/StroJLogService/releases).
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

   