# QtOrm

QtOrm is an unofficial object-relational mapping (ORM) module for the Qt Framework (http://qt.io).

QtOrm is designed for small to medium-sized databases and datasets, such as those used in embedded systems. In practice, it performs well with tables containing several thousand rows.

See qws19.pdf for more examples.

## License

Copyright (C) 2019-2025 Dmitriy Purgin <dpurgin@gmail.com>

Copyright (C) 2019-2025 Dmitriy Purgin <dmitriy.purgin@sequality.at>

Copyright (C) 2019-2025 sequality software engineering e.U. <office@sequality.at>

QtOrm is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser 
General Public License as published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.

QtOrm is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.

## Prerequisites

QtOrm supports Qt 5.12, 5.15, and 6.8. It requires a C++17-compliant compiler with standard library support. QtOrm depends on QtCore and QtSql.

The library has been used and tested on the following platforms:
 * MinGW 7 on x86_64
 * GCC 8 on ARM32
 * GCC 9 on x86_64
 * GCC 11 on AARCH64
 
Other compilers, platforms, and Qt versions may also be supported.

## Using in a CMake Project

### Using FetchContent

The recommended way to use QtOrm is to add it as a dependency with FetchContent: 

```
FetchContent_Declare(qtorm
    GIT_REPOSITORY https://github.com/dpurgin/qtorm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(qtorm)
```

### Using as a Subdirectory

Alternatively, you can clone the project from GitHub into a subdirectory and add it to your project like this:

```
add_subdirectory(../qtorm qtorm.build)  

target_link_libraries(mytarget PUBLIC qtorm)
```

### Build Options and Parameters

The following CMake options are available:

* `QTORM_BUILD_EXAMPLES` – Build the example applications included with QtOrm. Defaults to `OFF` if a parent CMakeLists file is detected.
* `QTORM_BUILD_TESTS` – Build unit tests. Also defaults to `OFF` if a parent CMakeLists file is detected.
* `QTORM_BUILD_SHARED_LIBS` – Build the library as a shared library (the default, to comply with LGPLv3). If set to `OFF`, the library is built as a static library, which may require you to fulfill additional obligations under LGPLv3.
* `QTORM_QT_VERSION_HINT` – Specify the major Qt version to use. Possible values: `auto`, `5`, or `6`. The default is `auto`, which tries to find Qt 6 first.

## Installing as a Qt Module (Qt 5 Only, Deprecated)

* Open qtorm.pro with Qt Creator
* Configure with the required Qt kit (Qt 5.12 or later, C++17-compliant compiler)
* Build
* Deploy to the Qt installation folder with `make install`

## Using as a qmake Subproject (Qt 5 Only, Deprecated)

* Copy the qtorm repo into your project 
* Add `SUBDIRS += qtorm`
* Provide `INCLUDEPATH` and linker flags explicitly. The libraries will have the common `bin`, `lib`, and `include` structure in the build folder. 

For example, with the following project structure: 

```
 + myproject
 |-- myproject.pro
 +-- qtorm
 |----- qtorm.pro
 +-- src
 |----- src.pro
 |----- main.cpp
```

The contents of `myproject.pro` should be:

```qmake
TEMPLATE = subdirs
SUBDIRS = qtorm src
src.depends = qtorm
```

The contents of `src.pro` should be:

```qmake
TEMPLATE = app

SOURCES += main.cpp

INCLUDEPATH += \
    $$shadowed(../qtorm/include) \
    $$shadowed(../qtorm/include/QtOrm) \
    $$shadowed(../qtorm/include/QtOrm/$${QT_VERSION})

*g++* {
    # Linker flags for g++ or MinGW
    LIBS += -L$$shadowed(../qtorm/lib) -lQt5Orm
}
*msvc* {
    LIBS += $$shadowed(../qtorm/lib/Qt5Orm.lib)
}
```

Note: The runtime library (`libQt5Orm.so` on Linux or `Qt5Orm.dll` on Windows) should be available under `LD_LIBRARY_PATH` (Linux) or `PATH` (Windows) when running the application.

## Current Status

QtOrm currently supports the SQLite backend with the following features:

* Creating or updating the database schema according to the registered entities in the ORM
* Reading data from a table, with optional filtering and ordering
* Inserting and/or updating rows
* Removing single rows or sets of rows using filters
* Transaction support
* 1:n and n:1 relationship support 

## Usage

### Domain Classes 

Domain classes representing table entities must inherit from `QObject`. By default, class names are mapped to database tables with the same name. All properties declared with `Q_PROPERTY()` are mapped to database columns with the same names.

All non-transient properties must be readable, writable, and notifyable. 

Every entity must have an identity property. By default, the property `id` is considered the identity. This property will be set as the primary key and autogenerated by the SQLite backend. 

Every entity must have a default constructor declared with Q_INVOKABLE, otherwise the ORM will not be able to instantiate it when fetching data. 

A minimal entity declaration with all-default settings looks like this: 

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

All entities must be registered (at most once) before the first `QOrmSession` is created: 

```cpp
qRegisterOrmEntity<Community, Province, Town, ...>();
```

#### Mapping Customization

You can override the default mapping using `Q_ORM_CLASS()` and `Q_ORM_PROPERTY()`:

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
  * `SCHEMA <recreate|update|bypass|append>`: override schema mode for this entity 
* `Q_ORM_CLASS(<propertyName> ...)`:
  * `COLUMN <columnName>`: override the column name 
  * `IDENTITY [true|false]`: mark the property as the identity
  * `AUTOGENERATED [true|false]`: mark the property as autogenerated by the database backend
  * `TRANSIENT [true|false]`: mark the property as transient

Restrictions and requirements: 

* There can be only one `IDENTITY` 
* `IDENTITY` is required for `AUTOGENERATED` 
* `TRANSIENT` cannot be combined with `IDENTITY`
* Renaming columns and tables to names containing QtOrm keywords (`IDENTITY`, `COLUMN`, `TRANSIENT`, etc.) is not supported.

#### Relationships 

A 1:n relationship can be created by declaring a `QVector` of related entities as follows: 

```cpp
class Town;

class Province : public QObject
{
    Q_OBJECT 
    
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QVector<Town*> towns READ towns WRITE setTowns NOTIFY townsChanged)
    
    // ...rest of the class...
};
```

In this case, the `towns` property will be transient and not mapped to a database column. This requires an n:1 back-reference to `Province` in the `Town` entity. When updating the reference, remember to update both sides. For example, when adding a Town to a Province, add the object to the `towns` vector and set the Town's back-reference to the corresponding Province.

An n:1 relationship can be created by declaring a pointer to the referenced entity as follows: 

```cpp
class Province; 

class Town : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(Province* province READ province WRITE setProvince NOTIFY provinceChanged)
    
    // ...rest of the class...
};
```

The SQLite provider maps the `province` property to a database column `province_id`, with the column type set to the mapped type of `Province::id`. The back-reference in Province is optional. 

### Enums in Properties

You can use enumerations as property types. Both `enum` and `enum class` are supported. The enumeration type must be registered with `Q_DECLARE_METATYPE()` and `qRegisterOrmEnum()`, and its type must be fully qualified in `Q_PROPERTY()`. The helper function `qRegisterOrmEnum()` registers converters from/to `QString` and `int`. If you provide custom converters, you do not need to call this function.

```cpp
enum class CommunitySize
{
    Small, Medium, Large
};
Q_DECLARE_METATYPE(CommunitySize);

namespace Classifier
{
    enum FederalProvince
    {
        Burgenland,
        Carinthia,
        LowerAustria,
        UpperAustria,
        Salzburg,
        Styria,
        Tyrol,
        Vorarlberg,
        Vienna
    }
}
Q_DECLARE_METATYPE(Classifier::FederalProvince);

class Community : public QObject
{
    Q_OBJECT

    Q_PROPERTY(long id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(CommunitySize communitySize READ communitySize WRITE setCommunitySize NOTIFY communitySizeChanged)
    Q_PROPERTY(Classifier::FederalProvince province READ province WRITE setProvince NOTIFY provinceChanged)

public:
    Q_INVOKABLE Community(QObject* parent = nullptr);
    
    // ...
}

int main() 
{
    qRegisterOrmEntity<Community>();
    qRegisterOrmEnum<CommunitySize, Classifier::FederalProvince>();
    // ... 
}
```


### `QOrmSession` 

A `QOrmSession` instance is the entry point to the ORM. All database operations should be performed using a single `QOrmSession` instance. It is recommended to have only one `QOrmSession` in your application.

A `QOrmSession` can be configured either with a `QOrmSessionConfiguration` instance or automatically from a `qtorm.json` file located in the resources root, working directory, or the application executable directory.

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

Possible values for `schemaMode`: `recreate`, `bypass`, `update`, `append`.

Any other JSON keys are silently ignored.

### Schema Mode 

When an entity is first accessed, QtOrm processes its database schema. The `QOrmSessionConfiguration::schemaMode` property specifies the default processing mode: 
 
 * `recreate`: Drop the table in the database if it exists, and create a new one
 * `bypass`: Do not modify or verify the schema. This may result in errors when using `QOrmSession`
 * `update`: If the table does not exist, create it. Otherwise, if columns do not match in names or data types, update the schema while preserving existing data if possible. For the SQLite backend, the [generalized 12-step ALTER TABLE procedure](https://sqlite.org/lang_altertable.html#otheralter) is used.
 * `append`: Add new columns to existing tables and create tables if they don't exist. 
 
The default processing mode can be overridden for each entity individually using the `Q_ORM_CLASS(SCHEMA ...)` declaration. 

### Inserting or Updating

Both insert and update operations are handled by `QOrmSession::merge()`. For example:

```c++
QOrmSession session;

Community* hagenberg = new Community{};
hagenberg->setName("Hagenber");

// An entry will be inserted as it is not in the session cache yet.
// The session object will take ownership of the entity.
// The entity's id will be set to the one generated by the database.
session.merge(hagenberg);

hagenberg->setName("Hagenberg");
// The entity will be updated, as it is already in the session cache.
session.merge(hagenberg);
```

The entry will be inserted if it has not been read from the database before. Otherwise, it will be updated. Note: There is currently no way to detect if the entity already exists in the database. One could query the database by `id` first, but since the `id` property is usually a primitive integral type, there is no safe default value to rely on. It is possible that the initial value of the `id` property of a new entity unintentionally matches an existing database row.

### Querying Data

Data queries are similar to .NET LINQ. For example:

```c++
QOrmSession session;

// Select all communities
{
    QOrmQueryResult result = session.from<Community>().select();

    QVector<Community*> communities = result.toVector();
    // The entities are owned by QOrmSession; do not delete them.
}

// Select with filter
{
    QOrmQueryResult result = session.from<Community>()
                                    .filter(Q_ORM_CLASS_PROPERTY(name) == "Hagenberg")
                                    .select();
}

// Logical and comparison operators are supported in filters.
//
// Note: Comparison to a list is equivalent to:
//   (Q_ORM_CLASS_PROPERTY(name) == cities[0] || 
//    Q_ORM_CLASS_PROPERTY(name) == cities[1] || 
//    Q_ORM_CLASS_PROPERTY(name) == cities[2])
{
    QStringList cities{"Linz", "Graz", "Salzburg"};

    QOrmQueryResult result = session.from<Community>()
                                    .filter((Q_ORM_CLASS_PROPERTY(population) >= 5000 && Q_ORM_CLASS_PROPERTY(population) < 10000) || 
                                            Q_ORM_CLASS_PROPERTY(name) == cities)
                                    .select();
}
```

You can limit data using the `limit` and/or `offset` methods:

```c++
// Query the first ten communities, sorted by name.
QOrmQueryResult result = session.from<Community>()
                                .order(Q_ORM_CLASS_PROPERTY(name))
                                .limit(10)
                                .select();

```

```c++
// Query the first ten communities, sorted by name, starting from the third one.
QOrmQueryResult result = session.from<Community>()
                                .order(Q_ORM_CLASS_PROPERTY(name))
                                .offset(2)
                                .limit(10)
                                .select();
```

### Removing a Single Entity

You can remove a single existing entity using the `remove()` method of `QOrmSession`. This method removes the corresponding row from the database and returns ownership of the entity to the caller, wrapped in a `std::unique_ptr`:

```c++
QOrmSession session;

QOrmQueryResult result = session.from<Community>()
                                .filter(Q_ORM_CLASS_PROPERTY(name) == "Hagenberg")
                                .select();

if (!result.hasError())
{
    // Ownership of the entity is returned in a smart pointer.
    std::unique_ptr<Community> hagenberg = session.remove(result.first());
    
    qDebug() << "Removed entity:" << hagenberg->name();
}
```

### Removing with a Query

The following remove query works with SQLite >= 3.35:

```c++
QOrmSession session;

QOrmQueryResult result = session.from<Community>()
                                .filter(Q_ORM_CLASS_PROPERTY(population) >= 5000)
                                .remove();

if (!result.hasError())
{
    // Ownership of the entities is returned to the caller.
    qDeleteAll(result.toVector());    
}
```

Ownership of the removed entities is returned to the caller. The entities in the query result must be deleted using `delete`.

