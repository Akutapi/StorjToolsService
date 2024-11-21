# Stroj Tools Service

Stroj Tools Service is an automated service designed to reduce log files for the decentralized storage system Stroj.
The service is fully automatic and configurable via the `config.yaml` file.
The service updates its configuration upon startup, restart, or before performing log reduction.
Therefore, if the configuration change is not urgent, there is no need to restart the service.

## Features
- Automated log file reduction
- Configurable via `config.yaml`
- Supports multiple Stroj nodes on a single server

## Configuration

The service uses a `config.yaml` file for its configuration. Below is an example of the configuration file:

- `Max Log Size In GB`: The maximum size of the log file in gigabytes before reduction is triggered.
- `Reduce Log Size In GB`: The size to which the log file will be reduced.
- `Reduce Log Time In Hours`: The interval in hours at which the log reduction process will be executed.

## License

This project is licensed under the MIT License. See the [![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) file for details.

## Installation

1. Download the installer from the [releases page](https://github.com/Akutapi/StroJLogService/releases).
2. Run the installer and follow the on-screen instructions to complete the installation.
3. The installer will automatically set up and start the Stroj Tools Service.

## Usage

1. Ensure the `config.yaml` file is properly configured.
2. The service will start automatically after installation. You can manage the service using standard service management tools.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## Contact

For any questions or issues, please open an issue on GitHub.

   