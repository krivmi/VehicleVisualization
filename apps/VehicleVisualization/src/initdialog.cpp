#include "initdialog.h"

#include <QSettings>

InitDialog::InitDialog(QWidget* parent) : QDialog(parent)
{
    // Set the dialog flags and title
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowTitle(tr("Info Dialog"));

    QLabel *projectPathLabel = new QLabel("Project path:");
    projectPathEdit = new QLineEdit;
    QLabel *receivingCommandLabel = new QLabel("Receiving command:");
    receivingCommandLineEdit = new QLineEdit;
    QLabel *loadingCommandLabel = new QLabel("Loading command:");
    loadingCommandLineEdit = new QLineEdit;
    QLabel *GPSDhostLabel = new QLabel("GPSD host");
    GPSDhostLineEdit = new QLineEdit;
    QLabel *GPSDPortLabel = new QLabel("GPSD port:");
    GPSDPortLineEdit = new QLineEdit;

    info = new QLabel("Info:");
    submitButton = new QPushButton("Submit");
    QObject::connect(submitButton, &QPushButton::clicked, this, &InitDialog::setSettings);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(projectPathLabel, projectPathEdit);
    formLayout->addRow(receivingCommandLabel, receivingCommandLineEdit);
    formLayout->addRow(loadingCommandLabel, loadingCommandLineEdit);
    formLayout->addRow(GPSDhostLabel, GPSDhostLineEdit);
    formLayout->addRow(GPSDPortLabel, GPSDPortLineEdit);
    formLayout->addRow(info, submitButton);

    QSettings settings("krivmi", "VehicleVisualization");
    QString path = settings.value("projectPath").toString();
    QString recCommand = settings.value("receivingCommand").toString();
    QString loadCommand = settings.value("loadingCommand").toString();
    QString host = settings.value("GPSD/host").toString();
    QString port = settings.value("GPSD/port").toString();

    projectPathEdit->setText(path);
    receivingCommandLineEdit->setText(recCommand);
    loadingCommandLineEdit->setText(loadCommand);
    GPSDhostLineEdit->setText(host);
    GPSDPortLineEdit->setText(port);

    setLayout(formLayout);

    resize(500, this->height());
}

void InitDialog::setSettings()
{
    QString path = projectPathEdit->text();
    QString recCommand = receivingCommandLineEdit->text();
    QString loadCommand = loadingCommandLineEdit->text();
    QString host = GPSDhostLineEdit->text();
    QString port = GPSDPortLineEdit->text();

    // if the settings is left empty and the app is restarted,
    // it will load default settings
/*
    if (path.isEmpty() || recCommand.isEmpty() || loadCommand.isEmpty() ||
            host.isEmpty() || port.isEmpty()) {
        info->setText("Please fill in all fields");
        info->setStyleSheet("color: red");

        return;
    }*/

    QSettings settings("krivmi", "VehicleVisualization");
    settings.setValue("projectPath", path);
    settings.setValue("receivingCommand", recCommand);
    settings.setValue("loadingCommand", loadCommand);
    settings.setValue("GPSD/host", host);
    settings.setValue("GPSD/port", port);

    emit submitOK();
    accept();
}
