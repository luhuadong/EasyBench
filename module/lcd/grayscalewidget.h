#ifndef GRAYSCALEWIDGET_H
#define GRAYSCALEWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QVector>
#include <QList>

class GrayscaleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GrayscaleWidget(QWidget *parent = 0);
    ~GrayscaleWidget();

signals:

public slots:

protected:
    void mousePressEvent(QMouseEvent *);

private:
    //QVector<QLabel> grayscaleVector;
    //QList<QLabel> grayscaleList;
    QLabel *grayscaleTab[16];

};

#endif // GRAYSCALEWIDGET_H
