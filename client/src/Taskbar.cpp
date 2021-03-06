/******************************************************************************\
*  client/src/Taskbar.cpp                                                      *
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

#include "Taskbar.h"
#include "Options.h"
#include "UserList.h"
#include "Settings.h"
#include "LogWidget.h"
#include "DevilWindow.h"
#include "SkillWindow.h"
#include "MashouWindow.h"
#include "ImportExport.h"
#include "ajax.h"

#include <QtGui/QSystemTrayIcon>
#include <QtGui/QAction>
#include <QtGui/QMenu>

Taskbar::Taskbar(QWidget *parent_widget) : QWidget(parent_widget), mOptions(0),
	mUserList(0), mLogWidget(0), mDevilWindow(0), mSkillWindow(0),
	mMashouWindow(0), mImportExportWindow(0), mAdminSep(0), mUsersAction(0)
{
	ui.setupUi(this);

	// Hide the admin tab unless you have admin rights
	ui.tabWidget->removeTab(1);

	// Now that the registration is done, let's enable the tray icon
	// and disable this setting.
	QApplication::setQuitOnLastWindowClosed(false);

	QMenu *menu = new QMenu( tr("MegatenDB") );
	QAction *act;

	// Devils Action
	act = menu->addAction(/* icon, */tr("&Devils") );
	connect(act, SIGNAL(triggered()), this, SLOT(showDevilWindow()));
	connect(ui.devilsButton, SIGNAL(clicked(bool)),
		this, SLOT(showDevilWindow()));

	// Skills Action
	act = menu->addAction(/* icon, */tr("&Skills") );
	connect(act, SIGNAL(triggered()), this, SLOT(showSkillWindow()));
	connect(ui.skillsButton, SIGNAL(clicked(bool)),
		this, SLOT(showSkillWindow()));

	// Mashou Action
	act = menu->addAction(/* icon, */tr("&Mashou") );
	connect(act, SIGNAL(triggered()), this, SLOT(showMashouWindow()));
	connect(ui.mashouButton, SIGNAL(clicked(bool)),
		this, SLOT(showMashouWindow()));

	mAdminSep = menu->addSeparator();
	mAdminSep->setEnabled(false);
	mAdminSep->setVisible(false);

	// Users Action
	mUsersAction = menu->addAction(/* icon, */tr("&Users") );
	mUsersAction->setEnabled(false);
	mUsersAction->setVisible(false);
	connect(mUsersAction, SIGNAL(triggered()), this, SLOT(showUsersWindow()));
	connect(ui.usersButton, SIGNAL(clicked(bool)),
		this, SLOT(showUsersWindow()));

	// Import / Export Action
	mImportExportAction = menu->addAction(/* icon, */tr("&Import / Export") );
	mImportExportAction->setEnabled(false);
	mImportExportAction->setVisible(false);
	connect(mImportExportAction, SIGNAL(triggered()),
		this, SLOT(showImportExportWindow()));
	connect(ui.importExportButton, SIGNAL(clicked(bool)),
		this, SLOT(showImportExportWindow()));

	act = menu->addSeparator();

	// Options Action
	act = menu->addAction(/* icon, */tr("&Options") );
	connect(act, SIGNAL(triggered()), this, SLOT(showOptionsWindow()));
	connect(ui.optionsButton, SIGNAL(clicked(bool)),
		this, SLOT(showOptionsWindow()));

	// Log Action
	act = menu->addAction(/* icon, */tr("&Log") );
	connect(act, SIGNAL(triggered()), this, SLOT(showLogWindow()));
	connect(ui.logButton, SIGNAL(clicked(bool)),
		this, SLOT(showLogWindow()));

	act = menu->addSeparator();

	// Quit Action
	act = menu->addAction(/* icon, */tr("&Quit") );
	connect(act, SIGNAL(triggered()), qApp, SLOT(quit()));

	QSystemTrayIcon *tray = new QSystemTrayIcon( QApplication::windowIcon() );
	tray->setContextMenu(menu);
	tray->show();

	setWindowTitle( tr("%1 - Taskbar").arg(
		tr("Shin Megami Tensei IMAGINE DB") ) );

	ajax::getSingletonPtr()->subscribe(this);

	{
		QVariantMap action;
		action["action"] = "auth_query_perms";
		action["user_data"] = "auth_query_perms";

		ajax::getSingletonPtr()->request(settings->url(), action);
	}
}

void Taskbar::showLogWindow()
{
	if(!mLogWidget)
		mLogWidget = LogWidget::getSingletonPtr();

	Q_ASSERT(mLogWidget != 0);

	mLogWidget->show();
	mLogWidget->activateWindow();
	mLogWidget->raise();
}

void Taskbar::showDevilWindow()
{
	if(!mDevilWindow)
		mDevilWindow = new DevilWindow;

	Q_ASSERT(mDevilWindow != 0);

	mDevilWindow->show();
	mDevilWindow->activateWindow();
	mDevilWindow->raise();
}

void Taskbar::showSkillWindow()
{
	if(!mSkillWindow)
		mSkillWindow = new SkillWindow;

	Q_ASSERT(mSkillWindow != 0);

	mSkillWindow->show();
	mSkillWindow->activateWindow();
	mSkillWindow->raise();
}

void Taskbar::showUsersWindow()
{
	if(!mUserList)
		mUserList = new UserList;

	Q_ASSERT(mUserList != 0);

	if( !mUserList->isVisible() )
		mUserList->refresh();

	mUserList->show();
	mUserList->activateWindow();
	mUserList->raise();
}

void Taskbar::showMashouWindow()
{
	if(!mMashouWindow)
		mMashouWindow = new MashouWindow;

	Q_ASSERT(mMashouWindow != 0);

	mMashouWindow->show();
	mMashouWindow->activateWindow();
	mMashouWindow->raise();
}

void Taskbar::showOptionsWindow()
{
	if(!mOptions)
		mOptions = Options::getSingletonPtr();

	Q_ASSERT(mOptions != 0);

	if( !mOptions->isVisible() )
		mOptions->loadSettings();

	mOptions->show();
	mOptions->activateWindow();
	mOptions->raise();
}

void Taskbar::showImportExportWindow()
{
	if(!mImportExportWindow)
		mImportExportWindow = new ImportExport;

	Q_ASSERT(mImportExportWindow != 0);

	mImportExportWindow->show();
	mImportExportWindow->activateWindow();
	mImportExportWindow->raise();
}

void Taskbar::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();
	if( result.contains("error") )
		return;

	if(result.value("user_data").toString() == "auth_query_perms")
	{
		if( result.value("perms").toMap().value("admin").toBool()
			&& !mAdminSep->isVisible() )
		{
			ui.tabWidget->insertTab(1, ui.adminTab, tr("&Administration"));
			mAdminSep->setVisible(true);
			mAdminSep->setEnabled(true);
			mUsersAction->setEnabled(true);
			mUsersAction->setVisible(true);
			mImportExportAction->setEnabled(true);
			mImportExportAction->setVisible(true);
		}
	}
}
