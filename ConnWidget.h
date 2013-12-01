/** File:		ConnWidget.h
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

#ifndef __CONNWIDGET_H__
#define __CONNWIDGET_H__

#include <QWidget>
#include "GeneratedFiles\ui_ConnWidget.h"

class ConnWidget : public QWidget, public Ui::ConnWidget
{
	Q_OBJECT

public:
	ConnWidget(QWidget *parent = 0);
	~ConnWidget();

signals:
	void signalConnButton(QString ip);

private slots:
	void slotButtonConn();
};

#endif