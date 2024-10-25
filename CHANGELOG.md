# 
# Changelog of elants_hid_tools.
# 
# Copyright (c) 2024 ELAN Microelectronics Corp. All Rights Reserved.

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1] - 2024-03-21

The first release of elants_hid_tools project.
Sources of this project is based on elan_i2chid_tools v1.18 2022/10/28.

### Added
- Add support of FW update to SPI-HID devices.

### Removed
- Remove result log function.
- Remove read ghost data function.

### Fixed
- Fix unit of length parameter of Gen8 Read ROM Command to word.

### Changed
- Change name of error code to "ERR_${Name}".

## [0.2] - 2024-05-08

### Added
- Add CHANGELOG.md.

## [0.3] - 2024-05-22

### Change
- Change the order of reading FW information in sequence for timing control.

### Fixed
- Change unit of length parameter of Gen8 Read ROM Command to byte.
- Reduce delay after Write Flash Key Command & Enter IAP Command in Gen8 FW Update Flow.
- Remove redundant CRLFs in debug log.

## [0.4] - 2024-10-25

### Added
- Add copyright notice to all sources and makefiles.

### Change
- Refactor all makefiles, changing used variables to the ones appeared in "The GNU Make Reference Manual: Version 4.2".
- Update fwid_mapping_table.txt to current version.

### Fixed
- Update copyright year of all sources and makefiles.

