/*
* Copyright (C) 2019-2024 Dmitriy Purgin <dmitriy.purgin@sequality.at>
* Copyright (C) 2019-2024 sequality software engineering e.U. <office@sequality.at>
*
* This file is part of QtOrm library.
*
* QtOrm is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* QtOrm is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <QObject>

namespace MyNamespace
{
    enum MyEnum
    {
        MyEnumValue0,
        MyEnumValue1,
        MyEnumValue2
    };

    enum class MyEnumClass
    {
        MyEnumClassValue0,
        MyEnumClassValue1,
        MyEnumClassValue2
    };
}

Q_DECLARE_METATYPE(MyNamespace::MyEnum);
Q_DECLARE_METATYPE(MyNamespace::MyEnumClass);

class WithEnum : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(MyNamespace::MyEnum myEnum READ myEnum WRITE setMyEnum NOTIFY myEnumChanged)
    Q_PROPERTY(MyNamespace::MyEnumClass myEnumClass READ myEnumClass WRITE setMyEnumClass NOTIFY myEnumClassChanged)

public:
    Q_INVOKABLE WithEnum(QObject* parent = nullptr)
        : QObject{parent}
    {
    }

    [[nodiscard]] int id() const;
    void setId(int id);

    [[nodiscard]] MyNamespace::MyEnum myEnum() const;
    void setMyEnum(MyNamespace::MyEnum myEnum);

    [[nodiscard]] MyNamespace::MyEnumClass myEnumClass() const;
    void setMyEnumClass(MyNamespace::MyEnumClass myEnumClass);

signals:
    void idChanged();
    void myEnumChanged();
    void myEnumClassChanged();

private:
    int m_id{0};
    MyNamespace::MyEnum m_myEnum{MyNamespace::MyEnumValue0};
    MyNamespace::MyEnumClass m_myEnumClass{MyNamespace::MyEnumClass::MyEnumClassValue0};
};
