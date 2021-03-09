#include "mainwindow.h"
#include "ui_mainwindow.h"

// if (byte == '/')
//  PackBegin = true;

// if (PackBegin) {
//  if (byte == 0x0A) {
//    handlePacket();
//    ReadBuffer.clear();
//    return;
//  }

//  ReadBuffer.append(byte);
//}

#define STATUS_MSG_TIMEOUT 5000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  HoverControl.setTransmitter(this);

  HoverTelemetry.setTelemetryVisualiser(this);
  HoverTelemetry.setTransmitter(this);

  TimerControl = startTimer(100);
  TimerControlPeriodic = startTimer(2000);
  TimerTelemetry = startTimer(300);

  PortMaster.setBaudRate(19200);
  PortSlave.setBaudRate(19200);

  connect(ui->pbPortMaster, &QPushButton::clicked, this,
          &MainWindow::portMasterClick);
  connect(ui->pbCloseMaster, &QPushButton::clicked, this,
          &MainWindow::portMasterCloseClick);
  connect(ui->pbPortSlave, &QPushButton::clicked, this,
          &MainWindow::portSlaveClick);
  connect(ui->pbCloseSlave, &QPushButton::clicked, this,
          &MainWindow::portSlaveCloseClick);
  connect(ui->pbPacketsMaster, &QPushButton::clicked, this,
          &MainWindow::packetsMasterClick);
  connect(ui->pbPacketsSlave, &QPushButton::clicked, this,
          &MainWindow::packetsSlaveClick);

  connect(ui->slSpeed, &QSlider::valueChanged, this,
          &MainWindow::sliderSpeedChange);
  connect(ui->slSteer, &QSlider::valueChanged, this,
          &MainWindow::sliderSteerChange);
  connect(ui->tbSpeedReset, &QToolButton::clicked, this,
          &MainWindow::sliderSpeedReset);
  connect(ui->tbSteerReset, &QToolButton::clicked, this,
          &MainWindow::sliderSteerReset);

  connect(&PortMaster, &QSerialPort::errorOccurred, this,
          &MainWindow::portErrorMaster);
  connect(&PortSlave, &QSerialPort::errorOccurred, this,
          &MainWindow::portErrorSlave);
  connect(&PortMaster, &QSerialPort::readyRead, this,
          &MainWindow::readyReadMaster);
  connect(&PortSlave, &QSerialPort::readyRead, this,
          &MainWindow::readyReadSlave);
}

MainWindow::~MainWindow() {
  portMasterCloseClick();
  portSlaveCloseClick();
  delete ui;
}

void MainWindow::updateConnection() {
  ui->cbMasterConnected->setChecked(HoverControl.isConnected());
  ui->cbSlaveConnected->setChecked(false);
}

void MainWindow::transmitMaster(QByteArray data) { PortMaster.write(data); }

void MainWindow::transmitSlave(QByteArray data) { PortSlave.write(data); }

void MainWindow::telemetryUpdate() {
  ui->lcdBateryVoltage->display(HoverTelemetry.getBateryVoltage());
  ui->lcdCurrentMaster->display(
      HoverTelemetry.getCurrent(GDHoverTelemetry::btMaster));
  ui->lcdCurrentSlave->display(
      HoverTelemetry.getCurrent(GDHoverTelemetry::btSlave));
  ui->lcdSpeedMaster->display(
      HoverTelemetry.getRealSpeed(GDHoverTelemetry::btMaster));
  ui->lcdSpeedSlave->display(
      HoverTelemetry.getRealSpeed(GDHoverTelemetry::btSlave));
}

void MainWindow::portMasterClick() {
  SerialPortSettingsDialog *dlg =
      new SerialPortSettingsDialog(&PortMaster, this);
  dlg->show();
  dlg->exec();

  sliderSpeedReset();
  sliderSteerReset();
  ui->cbControlOn->setChecked(false);
}

void MainWindow::portMasterCloseClick() {
  PortMaster.close();
  statusBar()->showMessage("Master: порт закрыт", STATUS_MSG_TIMEOUT);
}

void MainWindow::portSlaveClick() {
  SerialPortSettingsDialog *dlg =
      new SerialPortSettingsDialog(&PortSlave, this);
  dlg->show();
  dlg->exec();
}

void MainWindow::portSlaveCloseClick() {
  PortSlave.close();
  statusBar()->showMessage("Slave: порт закрыт", STATUS_MSG_TIMEOUT);
}

void MainWindow::packetsMasterClick() {}

void MainWindow::packetsSlaveClick() {}

void MainWindow::sliderSpeedChange(int value) { ui->lcdSpeed->display(value); }

void MainWindow::sliderSpeedReset() { ui->slSpeed->setValue(0); }

void MainWindow::sliderSteerChange(int value) { ui->lcdSteer->display(value); }

void MainWindow::sliderSteerReset() { ui->slSteer->setValue(0); }

void MainWindow::portErrorSlave(QSerialPort::SerialPortError error) {
  Q_UNUSED(error)
  statusBar()->showMessage("Slave: " + PortSlave.errorString(),
                           STATUS_MSG_TIMEOUT);
}

void MainWindow::readyReadMaster() {
  QByteArray data = PortMaster.readAll();
  for (int i = 0; i < data.length(); i++) {
    checkPacket(&PacketMaster, data.at(i));

    if (PacketMaster.PackBegin && PacketMaster.PackEnd) {
      HoverControl.handlePacket(PacketMaster.Buffer);
      PacketMaster.PackBegin = false;
      PacketMaster.PackEnd = false;
    }
  }
}

void MainWindow::readyReadSlave() {
  QByteArray data = PortSlave.readAll();
  for (int i = 0; i < data.length(); i++) {
    checkPacket(&PacketSlave, data.at(i));

    if (PacketSlave.PackBegin && PacketSlave.PackEnd) {
      HoverTelemetry.handlePacket(PacketSlave.Buffer);
      PacketSlave.PackBegin = false;
      PacketSlave.PackEnd = false;
    }
  }
}

void MainWindow::checkPacket(SerialPacket *packet, char byte) {
  if (byte == '/') {
    packet->Buffer.clear();
    packet->PackBegin = true;
    packet->PackEnd = false;
  }

  if (packet->PackBegin) {
    if (byte == 0x0A) {
      packet->PackEnd = true;
      return;
    }

    packet->Buffer.append(byte);
  }
}

void MainWindow::portErrorMaster(QSerialPort::SerialPortError error) {
  Q_UNUSED(error)

  statusBar()->showMessage("Master: " + PortMaster.errorString(),
                           STATUS_MSG_TIMEOUT);
}

void MainWindow::timerEvent(QTimerEvent *event) {
  if (event->timerId() == TimerControl)
    if (ui->cbControlOn->isChecked()) {
      int16_t speed = (int16_t)ui->slSpeed->value();
      int16_t steer = (int16_t)ui->slSteer->value();

      HoverControl.steering(speed, steer);
    }

  if (event->timerId() == TimerControlPeriodic)
    HoverControl.handlePeriodic();

  if (event->timerId() == TimerTelemetry)
    if (ui->cbTelemetryOn->isChecked()) {
    }
}
