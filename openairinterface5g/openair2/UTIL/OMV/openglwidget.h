/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief area devoted to draw the nodes and their connections
 */


#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QGLWidget>
#include <QString>
#include "qgl.h"
#include "structures.h"

class OpenGLWidget : public QGLWidget
{
  Q_OBJECT

public:
  OpenGLWidget();
  void drawGrid();
  void drawNodes();
  void loadTexture();
  void drawConnections();
  void drawSquare(int digit, int back, int w, int h, int sw, int sh);
  void drawBaseStation(int digit);
  void setDrawConnections(int draw);
  void setUseMap(int use);
  void setUsedMap(int map);
  void setUsedMap(QString map_path);
  void setLinksColor(int index);
  void setNodesColor(int index);
  void updateNodeSize(int size);
  ~OpenGLWidget();

protected:
  void paintGL();

public slots:
  void drawNewPosition();

private:
  GLuint textures[9];
  bool draw_connections;
  QImage b_station;
};

#endif // OPENGLWIDGET_H
