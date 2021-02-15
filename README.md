# QtOrm

QtOrm is an unofficial object-relational mapping module for the Qt Framework (http://qt.io).

The current version is a prototype and a proof-of-concept. 

Refer to qws19.pdf for more examples.

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

## Usage

### Domain classes 

Domain classes representing table entities must be derived from `QObject`. By default, class names will be mapped 
to database tables with the same name. All properties declared with `Q_PROPERTY()` will be mapped 
to database columns with the same names.

All non-transient properties must be readable, writable, and notifyable. 

Every entity must have an identity property. By default, the property `id` is considered to be identity. This property 
will be set to primary key and autogenerated by the SQLite backend. 

Every entity must have a default constructor declared with Q_INVOKABLE, otherwise the OR mapper will 
not be able to instantiate it when fetching data. 

A minimal entity declaration with all-default settings is as follows: 

```cpp
class Community : public QObject
{
    Q_OBJECT

    Q_PROPERTY(long id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    Q_INVOKABLE Community(QObject* parent = nullptr);
    
    long id() const { return m_id; }
    void setCommunityId(long id)
    {
        if (m_id != id)
        {
            m_id = id;
            emit idChanged();
        }
    }

    QString name() const { return m_name; }
    void setName(QString name)
    {
        if (m_name != name)
        {
            m_name = name;
            emit nameChanged();
        }
    }
    
signals:
    void idChanged();
    void nameChanged();

private:
    long m_id{0};
    QString m_name;
}
```

This entity is mapped using the following SQLite statement: `CREATE TABLE Community(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)`.

All entities must be registered at most once before the first `QOrmSession` instantiation: 

```cpp
qRegisterOrmEntity<Community, Province, Town, ...>();
```

#### Mapping Customization

The mapping defaults can be overriden by using `Q_ORM_CLASS()` and `Q_ORM_PROPERTY()`:

```cpp
class Community : public QObject
{
    Q_OBJECT

    Q_PROPERTY(long communityId READ communityId WRITE setCommunityId NOTIFY communityIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int population READ population WRITE setPopulation NOTIFY populationChanged)
    Q_PROPERTY(Province* province READ province WRITE setProvince NOTIFY provinceChanged)
    // STORED false specifies a transient property
    Q_PROPERTY(bool hasLargePopulation READ hasLargePopulation STORED false)
    Q_PROPERTY(bool hasSmallPopulation READ hasSmallPopulation)

    // Map to table 'communities' instead of 'Community'
    Q_ORM_CLASS(TABLE communities)
    // Map to column 'community_id' instead of 'communityId' and mark the column as primary key. 
    Q_ORM_PROPERTY(communityId COLUMN community_id IDENTITY)
    // Alternative to STORED false
    Q_ORM_PROPERTY(hasSmallPopulation TRANSIENT)

public:
    Q_INVOKABLE Community(QObject* parent = nullptr);
    
    // ...
}
```

Possible customizations: 

* `Q_ORM_CLASS(...)`:
  * `TABLE <tableName>`: override table name 
* `Q_ORM_CLASS(<propertyName> ...)`:
  * `COLUMN <columnName>`: override the column name 
  * `IDENTITY [true|false]`: mark the property as identity
  * `AUTOGENERATED [true|false]`: mark the property as autogenerated by the database backend
  * `TRANSIENT [true|false]`: mark the property as transient

Restrictions and requirements: 

* There can be only one `IDENTITY` 
* `IDENTITY` is required for `AUTOGENERATED` 
* `TRANSIENT` cannot be combined with `IDENTITY`
* Renaming columns and tables to anything containing one of the QtOrm keywords (`IDENTITY`, `COLUMN`, `TRANSIENT`, ...) is not supported.

#### Relations 

A 1:n relation can be created by declaring a `QVector` of related entities as follows: 

```cpp
class Town;

class Province : public QObject
{
    Q_OBJECT 
    
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QVector<Town*> towns READ towns WRITE setTowns NOTIFY townsChanged)
    
    // the rest of the class skipped
};
```

In this case the property `towns` will be transient and will not be mapped to a database column.
This requires that there is an n:1 back-reference to `Province` in the `Town` entity. When updating 
the reference, keep in mind that change is required on both sides. For example, when adding a Town 
to a Province, add this object to the vector `towns` and set the back-reference of Town to the 
corresponding Province.

An n:1 relation can be created by declaring a pointer to the referenced entity as follows: 

```cpp
class Province; 

class Town : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(Province* province READ province WRITE setProvince NOTIFY provinceChanged)
    
    // the rest of the class skipped
};
```

The SQLite provider maps the property `province` to a database column `province_id` with the column 
type set to the mapped type of `Province::id`. Back-reference in Province is optional. 

### `QOrmSession` 

An instance of `QOrmSession` is the entry point to the OR mapper. All database operations should be 
performed using a single instance of a `QOrmSession`.

An instance of QOrmSession can be configured either by using an instance of `QOrmSessionConfiguration`
or automatically from `qtorm.json` file located either in resources root, working directory, or 
the application executable directory.

#### `qtorm.json` Example

```json
{
    "provider": "sqlite",
    "verbose": true, 
    "sqlite": {
        "databaseName": "database.sqlite",
        "schemaMode": "recreate",
        "verbose": true
    }
}
```

Possible values for `schemaMode`: `recreate`, `bypass`.

Any other JSON keys are silently ignored.
