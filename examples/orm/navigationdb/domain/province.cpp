/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
 * All rights reserved.
 *
 * This file is part of the examples of the QtOrm library
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the sequality software engineering e.U. nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "province.h"

#include <QDebug>
#include <QDebugStateSaver>

QDebug operator<<(QDebug dbg, const Province& province)
{
    QDebugStateSaver saver{dbg};
    dbg.noquote().nospace()
        << "Province(" << province.id() << ", \"" << province.name() << "\")";
    return dbg;
}

Province::Province(QObject* parent)
    : QObject(parent)
{
}

Province::Province(const QString& name, QObject* parent)
    : QObject{parent}
    , m_name{name}
{
}

int Province::id() const
{
    return m_id;
}

QString Province::name() const
{
    return m_name;
}

void Province::setId(int id)
{
    if (m_id == id)
    {
        return;
    }

    m_id = id;
    emit idChanged(m_id);
}

void Province::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

QVector<Community*> Province::communityList() const
{
    return m_communityList;
}

void Province::setCommunityList(const QVector<Community*>& communityList)
{
    if (m_communityList == communityList)
        return;

    m_communityList = communityList;
    emit communityListChanged();
}
