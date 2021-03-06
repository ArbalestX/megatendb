/******************************************************************************\
*  client/src/ajax.cpp                                                         *
*  Copyright (C) 2008 John Eric Martin <john.eric.martin@gmail.com>            *
*                                                                              *
*  This program is free software; you can redistribute it and/or modify        *
*  it under the terms of the GNU General Public License version 2 as           *
*  published by the Free Software Foundation.                                  *
*                                                                              *
*  This program is distributed in the hope that it will be useful,             *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*  GNU General Public License for more details.                                *
*                                                                              *
*  You should have received a copy of the GNU General Public License           *
*  along with this program; if not, write to the                               *
*  Free Software Foundation, Inc.,                                             *
*  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                   *
\******************************************************************************/

#include "ajax.h"
#include "json.h"
#include "sha1.h"
#include "ajaxTransfer.h"

#include "Settings.h"
#include "LogWidget.h"

#include <QtCore/QTimer>
#include <QtCore/QMetaMethod>
#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidget>

//#include <iostream>
//using namespace std;

static ajax *g_ajax_inst = 0;

ajax::ajax(QObject *parent_object) : QObject(parent_object)
{
	Q_ASSERT(g_ajax_inst == 0);

	mLog = LogWidget::getSingletonPtr();
	mRequestQueueTimer = new QTimer;
	mRequestQueueTimer->setSingleShot(true);

	connect(mRequestQueueTimer, SIGNAL(timeout()), this, SLOT(dispatchQueue()));

	g_ajax_inst = this;
}

ajax::~ajax()
{
	g_ajax_inst = 0;
}

void ajax::showLog()
{
	mLog->show();
}

ajax* ajax::getSingletonPtr()
{
	if(!g_ajax_inst)
		new ajax;

	Q_ASSERT(g_ajax_inst != 0);

	return g_ajax_inst;
}

void ajax::request(const QUrl& url, const QVariant& req)
{
	if( mRequestQueue.value(url).count() >= 4 )
		dispatchQueue(url);

	mRequestQueue[url].append(req);
	mRequestQueueTimer->start();
}

void ajax::dispatchQueue(const QUrl& url)
{
	if( url.isEmpty() )
	{
		QMapIterator<QUrl, QVariantList> i(mRequestQueue);

		while( i.hasNext() )
		{
			i.next();

			QVariantMap req;
			req["actions"] = i.value();

			dispatchRequest(i.key(), req);
		}

		mRequestQueue.clear();
		mRequestQueueTimer->stop();
	}
	else
	{
		QVariantMap m_request;
		m_request["actions"] = mRequestQueue.take(url);

		dispatchRequest(url, m_request);

		if( mRequestQueue.isEmpty() )
			mRequestQueueTimer->stop();
	}
}

void ajax::dispatchRequest(const QUrl& url, const QVariant& req)
{
	// TODO: Error checking here
	QString json_request = json::toJSON(req);

	//QMessageBox::information(0, "JSON Request", json_request);
	//cout << json_request.toLocal8Bit().data() << endl << endl;

	QMap<QString, QString> post;
	post["request"] = json_request;

	QByteArray data;
	if( !settings->email().isEmpty() && !settings->pass().isEmpty() )
	{
		unsigned char checksum[20];
		QString pass, hash = settings->pass() + json_request;

		sha1((uchar*)hash.toAscii().data(), hash.toAscii().size(), checksum);

		for(int i = 0; i < 20; i++)
		{
			int byte = (int)checksum[i];
			pass += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
		}

		post["email"] = settings->email();
		post["pass"] = pass;
	}

	ajaxTransfer *transfer = ajaxTransfer::start(url, post);

	connect(transfer, SIGNAL(transferFinished(const QVariant&)),
		this, SIGNAL(response(const QVariant&)) );

	mLog->registerRequest(transfer, req);
}

void ajax::subscribe(QObject *obj)
{
	if(!obj)
		return;

	if(obj->metaObject()->indexOfSlot("ajaxResponse(QVariant)") == -1)
	{
		QMessageBox::critical(0, tr("AJAX Subscribe Error"),
			tr("Failed to find ajaxResponse() on object %1").arg(
			obj->metaObject()->className() ));

		return;
	}

	connect(this, SIGNAL(response(const QVariant&)), obj,
		SLOT(ajaxResponse(const QVariant&)));
}
