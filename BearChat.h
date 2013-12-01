/** File:		BearChat.h
 ** Author:		Dongli Zhang
 ** Contact:	dongli.zhang0129@gmail.com
 **
 ** Copyright (C) Dongli Zhang 2013
 **
 ** This program is free software;  you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY;  without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 ** the GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program;  if not, write to the Free Software 
 ** Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __BEARCHAT_H__
#define __BEARCHAT_H__

#include <QMainWindow>
#include "GeneratedFiles\ui_BearChat.h"
#include "BearServer.h"
#include "BearClient.h"
#include "ConnWidget.h"

class BearChat : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

private:
	QString username;
	QString peername;
	BearServer server;
	BearClient client;
	ConnWidget conn;
	int role;

public:
	BearChat();
	~BearChat();
	void setUserName(QString username);

private:
	void updateLocalInfoUserName();
	void updatePeerInfoUserName();
	void updateTitle();
	void procRecvMsg(QString& msg);

private slots:
	void slotSend();
	void slotDisconnect();
	void slotWait();
	void slotConnShow();
	void slotSrvConnection();
	void slotSrvReadMessage();
	void slotSrvDisconnect();
	void slotConnectRemote(QString ip);
	void slotConnHide();
	void slotCliConnected();
	void slotCliDisconnected();
	void slotCliReadyRead();
	void slotCliError(QAbstractSocket::SocketError);
};

#endif
