/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     wangchunlin<wangchunlin@uniontech.com>
 *
 * Maintainer: wangchunlin<wangchunlin@uniontech.com>
 *             houchengqiu<houchengqiu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "matchcontroller_p.h"
#include "matchcontroller.h"
#include "gui/datadefine.h"
#include "global/builtinsearch.h"

#include "contacts/interface/daemongrandsearchinterface.h"

using namespace GrandSearch;

MatchControllerPrivate::MatchControllerPrivate(MatchController *parent)
    : q_p(parent)
{
    m_daemonDbus = new DaemonGrandSearchInterface(this);

    initConnect();
}

void MatchControllerPrivate::initConnect()
{
    connect(m_daemonDbus, &DaemonGrandSearchInterface::Matched, this, &MatchControllerPrivate::onMatched);
    connect(m_daemonDbus, &DaemonGrandSearchInterface::SearchCompleted, this, &MatchControllerPrivate::onSearchCompleted);
}

void MatchControllerPrivate::onMatched(const QString &missionId)
{
    if (missionId != m_missionId || m_missionId.isEmpty())
        return;

    // 获取数据解析
    qDebug() << QString("m_daemonDbus->MatchedBuffer begin missionId:%1").arg(m_missionId);
    QByteArray matchedBytes = m_daemonDbus->MatchedBuffer(m_missionId);
    qDebug() << QString("m_daemonDbus->MatchedBuffer end   missionId:%1").arg(m_missionId);

    QDataStream stream(&matchedBytes, QIODevice::ReadOnly);

    MatchedItemMap items;
    stream >> items;

    emit q_p->matchedResult(items);

    qDebug() << QString("matched %1 groups.").arg(items.size());
}

void MatchControllerPrivate::onSearchCompleted(const QString &missionId)
{
    if (missionId != m_missionId || m_missionId.isEmpty())
        return;

    //通知界面刷新，若界面有数据不做处理，否则就刷新
    emit q_p->searchCompleted();

    qDebug() << QString("search Completed.");
}

MatchController::MatchController(QObject *parent)
    : QObject(parent)
    , d_p(new MatchControllerPrivate(this))
{

}

MatchController::~MatchController()
{

}

void MatchController::onMissionIdChanged(const QString &missionId)
{
    d_p->m_missionId = missionId;
}

