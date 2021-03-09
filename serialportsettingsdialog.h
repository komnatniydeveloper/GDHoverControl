#ifndef SERIALPORTSETTINGSDIALOG_H
#define SERIALPORTSETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class SerialPortSettingsDialog;
}

class SerialPortSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortSettingsDialog(QSerialPort *port, QWidget *parent = nullptr);
    ~SerialPortSettingsDialog();

private slots:
    void updatePortNames ();
    void applyParams ();

private:
    Ui::SerialPortSettingsDialog *ui;
    QSerialPort *Port;
};

#endif // SERIALPORTSETTINGSDIALOG_H
