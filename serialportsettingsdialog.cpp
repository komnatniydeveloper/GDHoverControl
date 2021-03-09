#include "serialportsettingsdialog.h"
#include "ui_serialportsettingsdialog.h"

#include <QSerialPortInfo>
#include <QDialogButtonBox>

struct PortParam {
  QString name;
  int value;
};

#define PARAMS_PARITY_CNT 5
const PortParam ParamsParity[PARAMS_PARITY_CNT] = {
    {"No", 0}, {"Even", 2}, {"Odd", 3}, {"Space", 4}, {"Mark", 5}};

#define PARAMS_DATABITS_CNT 4
const PortParam ParamsDataBits[PARAMS_DATABITS_CNT] = {
    {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}};

#define PARAMS_FLOWCONTROL_CNT 3
const PortParam ParamsFlowControl[PARAMS_FLOWCONTROL_CNT] = {
    {"No", 0}, {"Hardware", 1}, {"Software", 2}};

#define PARAMS_STOPBITS_CNT 3
const PortParam ParamsStopBits[PARAMS_FLOWCONTROL_CNT] = {
    {"1", 1}, {"1.5", 3}, {"2", 2}};

SerialPortSettingsDialog::SerialPortSettingsDialog(QSerialPort *port,
                                                   QWidget *parent)
    : QDialog(parent), ui(new Ui::SerialPortSettingsDialog) {
  ui->setupUi(this);

  Port = port;

  ui->cbParity->setCurrentIndex(-1);
  for (int i = 0; i < PARAMS_PARITY_CNT; i++) {
    ui->cbParity->addItem(ParamsParity[i].name, ParamsParity[i].value);

    if (Port->parity() == ParamsParity[i].value)
      ui->cbParity->setCurrentIndex(i);
  }

  ui->cbDataBits->setCurrentIndex(-1);
  for (int i = 0; i < PARAMS_DATABITS_CNT; i++) {
    ui->cbDataBits->addItem(ParamsDataBits[i].name, ParamsDataBits[i].value);

    if (Port->dataBits() == ParamsDataBits[i].value)
      ui->cbDataBits->setCurrentIndex(i);
  }

  ui->cbFlowControl->setCurrentIndex(-1);
  for (int i = 0; i < PARAMS_FLOWCONTROL_CNT; i++) {
    ui->cbFlowControl->addItem(ParamsFlowControl[i].name, ParamsFlowControl[i].value);

    if (Port->flowControl() == ParamsFlowControl[i].value)
      ui->cbFlowControl->setCurrentIndex(i);
  }

  ui->cbStopBits->setCurrentIndex(-1);
  for (int i = 0; i < PARAMS_STOPBITS_CNT; i++) {
    ui->cbStopBits->addItem(ParamsStopBits[i].name, ParamsStopBits[i].value);

    if (Port->stopBits() == ParamsStopBits[i].value)
      ui->cbStopBits->setCurrentIndex(i);
  }

  ui->sbBaudRate->setValue(Port->baudRate());

  ui->cbName->setCurrentIndex(-1);
  updatePortNames();
  ui->cbName->setCurrentText(Port->portName());

  connect (ui->pbUpdateNames, &QPushButton::clicked, this, &SerialPortSettingsDialog::updatePortNames);
  connect (ui->buttonBox, &QDialogButtonBox::accepted, this, &SerialPortSettingsDialog::applyParams);
}

SerialPortSettingsDialog::~SerialPortSettingsDialog() { delete ui; }

void SerialPortSettingsDialog::updatePortNames()
{
  ui->cbName->clear();
  const auto infos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &info : infos) {
      ui->cbName->addItem(info.portName());
  }
}

void SerialPortSettingsDialog::applyParams()
{
  Port->close();

  Port->setPortName(ui->cbName->currentText());
  Port->setBaudRate(ui->sbBaudRate->value());
  Port->setFlowControl((QSerialPort::FlowControl)ui->cbFlowControl->currentData().toInt());
  Port->setDataBits((QSerialPort::DataBits)ui->cbDataBits->currentData().toInt());
  Port->setParity((QSerialPort::Parity)ui->cbParity->currentData().toInt());
  Port->setStopBits((QSerialPort::StopBits)ui->cbStopBits->currentData().toInt());

  Port->open(QIODevice::ReadWrite);
}
