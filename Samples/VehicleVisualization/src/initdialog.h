#pragma once

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class InitDialog : public QDialog
{
    Q_OBJECT
public:
    InitDialog(QWidget* parent = 0);

    void setSettings();
signals:
    void submitOK();
private:
    QLineEdit * projectPathEdit;
    QLineEdit * receivingCommandLineEdit;
    QLineEdit * loadingCommandLineEdit;
    QLineEdit * GPSDhostLineEdit;
    QLineEdit * GPSDPortLineEdit;

    QLabel * info;
    QPushButton * submitButton;
};
