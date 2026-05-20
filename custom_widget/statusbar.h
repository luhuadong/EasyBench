#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QLabel>
#include <QWidget>

class StatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

    void setMessage(const QString &text);

private:
    QLabel *messageLabel = nullptr;
};

#endif /* STATUSBAR_H */
