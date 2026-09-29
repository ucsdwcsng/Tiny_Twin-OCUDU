/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief this thread is to process the communication between the simulator and the visualisor
 */

#ifndef COMMUNICATIONTHREAD_H
#define COMMUNICATIONTHREAD_H

#include <QThread>
#include <QDebug>
#include <QTextEdit>
#include "mywindow.h"
#include "structures.h"

class CommunicationThread : public QThread
{
  Q_OBJECT

public:
  CommunicationThread(MyWindow* window);

signals:
  void newData(QString data, int frame);
  void newPosition();
  void endOfTheSimulation();

private:
  void run();
  MyWindow* window;
};

#endif // COMMUNICATIONTHREAD_H
