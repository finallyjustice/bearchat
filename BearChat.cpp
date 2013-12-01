/** File:		BearChat.cpp
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

#include "BearChat.h"
#include <iostream>
#include "BearParam.h"
#include <QTextEdit>
#include <QAction>
#include <QNetworkInterface>
#include <QTextCodec>

using namespace std;

BearChat::BearChat()
{
	setupUi(this);

	connect(sendButton, SIGNAL(clicked()), this, SLOT(slotSend()));
	connect(disconButton, SIGNAL(clicked()), this, SLOT(slotDisconnect()));
	connect(actionWait, SIGNAL(triggered()), this, SLOT(slotWait()));
	connect(actionConn, SIGNAL(triggered()), this, SLOT(slotConnShow()));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(&server, SIGNAL(newConnection()), this, SLOT(slotSrvConnection()));
	connect(&conn, SIGNAL(signalConnButton(QString)), this, SLOT(slotConnectRemote(QString)));
	connect(conn.exitButton, SIGNAL(clicked()), this, SLOT(slotConnHide()));

	sendButton->setEnabled(false);
	disconButton->setEnabled(false);

	peername = "Unknown";
	updatePeerInfoUserName();

	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
}

BearChat::~BearChat()
{

}

void BearChat::setUserName(QString username)
{
	this->username = username;
	updateLocalInfoUserName();
	updateTitle();
}

void BearChat::updateLocalInfoUserName()
{
	this->localInfo->setText("Local: "+username);
}

void BearChat::updatePeerInfoUserName()
{
	this->peerInfo->setText("Remote: "+peername);
}

void BearChat::updateTitle()
{
	this->setWindowTitle("BearChat - "+username);
}

void BearChat::procRecvMsg(QString& msg)
{
	if(msg.left(2).compare("MT") == 0)
	{
		messageText->append(peername+": "+msg.right(msg.size()-2)+"\n");
	}

	if(msg.left(2).compare("SN") == 0)
	{
		peername = msg.right(msg.size()-2);
		updatePeerInfoUserName();
	}
}

void BearChat::slotSend()
{
	if(role == SRVROLE)
	{
		server.socket->write(QString("MT"+inputText->toPlainText()).toStdString().c_str());
	}
	if(role == CLIROLE)
	{
		client.write(QString("MT"+inputText->toPlainText()).toStdString().c_str());
	}
	messageText->append(username+": "+inputText->toPlainText()+"\n");
	inputText->clear();
}

void BearChat::slotDisconnect()
{
	if(role == SRVROLE)
	{
		server.socket->close();
		server.close();
	}
	if(role == CLIROLE)
	{
		client.close();
	}

	sendButton->setEnabled(false);
	disconButton->setEnabled(false);
	actionConn->setEnabled(true);
	actionWait->setEnabled(true);
}

void BearChat::slotWait()
{
	QList<QHostAddress> ql;
	QNetworkInterface in;
	ql = in.allAddresses();
	list<QHostAddress> sl = ql.toStdList();
	this->messageText->append("List of Host Address:");
	for(list<QHostAddress>::iterator it=sl.begin(); it!=sl.end(); it++)
	{
		if(it->protocol() & QAbstractSocket::IPv6Protocol) continue;
		this->messageText->append("  "+it->toString());
	}
	this->messageText->append("\n");

	server.close();
	if(!server.listen(QHostAddress::Any, SERVPORT))
	{
		this->messageText->append("Failed to bind on port "+QString::number(SERVPORT, 10)+"...");
	}
	this->messageText->append("Listening on port "+QString::number(SERVPORT, 10)+"...");

	role = SRVROLE;
	actionConn->setEnabled(false);
	actionWait->setEnabled(false);
}

void BearChat::slotConnShow()
{
	conn.show();
}

void BearChat::slotSrvConnection()
{
	server.socket = server.nextPendingConnection();
	connect(server.socket, SIGNAL(readyRead()), this, SLOT(slotSrvReadMessage()));
	connect(server.socket, SIGNAL(disconnected()), this, SLOT(slotSrvDisconnect()));
	server.close();
	
	messageText->append("Connection Established from "+server.socket->peerAddress().toString()+":"+QString::number(server.socket->peerPort(), 10));
	sendButton->setEnabled(true);
	disconButton->setEnabled(true);

	server.socket->write(QString("SN"+username).toStdString().c_str());
}

void BearChat::slotSrvReadMessage()
{
	QString buf;
	while(server.socket->bytesAvailable() > 0)
	{
		buf = buf + server.socket->readLine();
	}
	this->procRecvMsg(QString(buf));
}

void BearChat::slotSrvDisconnect()
{
	this->messageText->append("Disconnected from Remote Host");
	sendButton->setEnabled(false);
	disconButton->setEnabled(false);
	actionConn->setEnabled(true);
	actionWait->setEnabled(true);

	disconnect(server.socket, SIGNAL(readyRead()), this, SLOT(slotSrvReadMessage()));
	disconnect(server.socket, SIGNAL(disconnected()), this, SLOT(slotSrvDisconnect()));
}

void BearChat::slotConnectRemote(QString ip)
{
	this->messageText->append("Connecting to "+ip);
	connect(&client, SIGNAL(connected()), this, SLOT(slotCliConnected()));
	connect(&client, SIGNAL(disconnected()), this, SLOT(slotCliDisconnected()));
	connect(&client, SIGNAL(readyRead()), this, SLOT(slotCliReadyRead()));
	connect(&client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotCliError(QAbstractSocket::SocketError)));
	client.connectToHost(ip, SERVPORT);
	
	role = CLIROLE;
	actionConn->setEnabled(false);
	actionWait->setEnabled(false);
}

void BearChat::slotConnHide()
{
	conn.hide();
}

void BearChat::slotCliConnected()
{
	this->messageText->append("Connected to Remote Host");
	sendButton->setEnabled(true);
	disconButton->setEnabled(true);

	client.write(QString("SN"+username).toStdString().c_str());
}

void BearChat::slotCliDisconnected()
{
	this->messageText->append("Disconnected from Remote Host");
	sendButton->setEnabled(false);
	disconButton->setEnabled(false);
	actionConn->setEnabled(true);
	actionWait->setEnabled(true);

	disconnect(&client, SIGNAL(connected()), this, SLOT(slotCliConnected()));
	disconnect(&client, SIGNAL(disconnected()), this, SLOT(slotCliDisconnected()));
	disconnect(&client, SIGNAL(readyRead()), this, SLOT(slotCliReadyRead()));
	disconnect(&client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotCliError(QAbstractSocket::SocketError)));
}
	
void BearChat::slotCliReadyRead()
{
	QString buf;
	while(client.bytesAvailable() > 0)
	{
		buf = buf + client.readLine();
	}
	this->procRecvMsg(QString(buf));
}

void BearChat::slotCliError(QAbstractSocket::SocketError)
{
	this->messageText->append(client.errorString());

	sendButton->setEnabled(false);
	disconButton->setEnabled(false);
	actionConn->setEnabled(true);
	actionWait->setEnabled(true);

	disconnect(&client, SIGNAL(connected()), this, SLOT(slotCliConnected()));
	disconnect(&client, SIGNAL(disconnected()), this, SLOT(slotCliDisconnected()));
	disconnect(&client, SIGNAL(readyRead()), this, SLOT(slotCliReadyRead()));
	disconnect(&client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotCliError(QAbstractSocket::SocketError)));
}
