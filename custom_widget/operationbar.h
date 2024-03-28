#ifndef OPERATIONBAR_H
#define OPERATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>

class OperationBar : public QWidget
{
    Q_OBJECT
public:
    explicit OperationBar(QWidget *parent = 0);

    QButtonGroup *buttonGroup;

    QAbstractButton *firstButton() const;
    QAbstractButton *secondButton() const;
    QAbstractButton *thirdButton() const;
    QAbstractButton *fourthButton() const;
    QAbstractButton *fifthButton() const;

signals:

public slots:

private:
    void createButtons();
    QWidget *mainWidget;

};

#endif // OPERATIONBAR_H
