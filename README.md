# QtOrm

QtOrm is an unofficial object-relational mapping module for the Qt Framework (http://qt.io).

The current version is a prototype and a proof-of-concept. 

## License

Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>

Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>

QtOrm is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser 
General Public License as published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.

QtOrm is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.

## Prerequisites

The QtOrm library is being built on top of Qt 5.12 LTS and requires a C++17-compliant compiler with C++
 standard library support. QtOrm depends on Qt Core and Qt SQL. 

The library is currently being developed and tested with MinGW 7 on Windows 10, other compilers and
platforms might be supported but not guaranteed.

## Installation

* Open qtorm.pro with Qt Creator
* Configure with the required Qt kit (Qt 5.12 at least, C++17-compliant compiler)
* Build 
* Deploy with `make install`

## Current Status

QtOrm currently supports SQLite backend with the following operations:

* Creating database schema according to the registered entities in the OR-mapper
* Reading data from a table, optionally filtered and ordered
* Inserting and/or updating rows
* Removing single rows
* Transaction support
* 1:n, n:1 relations support 

