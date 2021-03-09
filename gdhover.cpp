#include "gdhover.h"
#include <QTimerEvent>

#define TIME_ELAPSED_BATTERY_VOLTAGE 500
#define TIME_ELAPSED_CURRENT_MASTER TIME_ELAPSED_BATTERY_VOLTAGE
#define TIME_ELAPSED_CURRENT_SLAVE TIME_ELAPSED_BATTERY_VOLTAGE
#define TIME_ELAPSED_SPEED_MASTER TIME_ELAPSED_BATTERY_VOLTAGE
#define TIME_ELAPSED_SPEED_SLAVE TIME_ELAPSED_BATTERY_VOLTAGE

#define TIME_ELAPSED_CONTROL_CONNECTION 600

uint16_t CalcCRC(uint8_t *ptr, int count) {
  uint16_t crc;
  uint8_t i;
  crc = 0;
  while (--count >= 0) {
    crc = crc ^ (uint16_t)*ptr++ << 8;
    i = 8;
    do {
      if (crc & 0x8000) {
        crc = crc << 1 ^ 0x1021;
      } else {
        crc = crc << 1;
      }
    } while (--i);
  }
  return (crc);
}

GDHoverControl::GDHoverControl() { ConnState = false; }

void GDHoverControl::setTransmitter(IHoverTransmitter *transmitter) {
  Transmitter = transmitter;
}

void GDHoverControl::steering(int16_t speed, int16_t steer) {
  QByteArray packet;
  packet.append('/');

  packet.append((speed >> 8) & 0xFF);
  packet.append((speed >> 0) & 0xFF);

  packet.append((steer >> 8) & 0xFF);
  packet.append((steer >> 0) & 0xFF);

  uint16_t crc = CalcCRC((uint8_t *)packet.data(), packet.length());
  packet.append((crc >> 8) & 0xFF);
  packet.append((crc >> 0) & 0xFF);

  packet.append(0x0A);
  Transmitter->transmitMaster(packet);
}

bool GDHoverControl::isConnected() { return ConnState; }

void GDHoverControl::handlePacket(QByteArray &packet) {
  Q_UNUSED(packet)

  TimerConnection.start();
  ConnState = true;
}

void GDHoverControl::handlePeriodic() {
  if (TimerConnection.elapsed() > TIME_ELAPSED_CONTROL_CONNECTION) {
    ConnState = false;
  }

  Transmitter->updateConnection();
}

GDHoverTelemetry::GDHoverTelemetry() {}

void GDHoverTelemetry::setTransmitter(IHoverTransmitter *transmitter) {
  Transmitter = transmitter;
}

void GDHoverTelemetry::setTelemetryVisualiser(
    IHoverTelemetryVisualiser *visualiser) {
  Visualizer = visualiser;
}

float GDHoverTelemetry::getBateryVoltage() { return BateryVoltage; }

float GDHoverTelemetry::getCurrent(GDHoverTelemetry::BoardType board) {
  if (board == btMaster)
    return CurrentMaster;
  else
    return CurrentSlave;

  return 0;
}

float GDHoverTelemetry::getRealSpeed(GDHoverTelemetry::BoardType board) {
  if (board == btMaster)
    return SpeedMaster;
  else
    return SpeedMaster;

  return 0;
}

void GDHoverTelemetry::cmdReadBateryVoltage() {
  /// @todo
}

void GDHoverTelemetry::cmdReadCurrent(GDHoverTelemetry::BoardType board) {
  Q_UNUSED(board)
  /// @todo
}

void GDHoverTelemetry::cmdReadSpeed(GDHoverTelemetry::BoardType board) {
  Q_UNUSED(board)
  /// @todo
}

void GDHoverTelemetry::handlePacket(QByteArray &packet) {
  Q_UNUSED(packet)
  /// @todo
}

void GDHoverTelemetry::handlePeriodic() {
  if (TimeBattery.elapsed() > TIME_ELAPSED_BATTERY_VOLTAGE)
    BateryVoltage = 0;

  if (TimeCurrentMaster.elapsed() > TIME_ELAPSED_CURRENT_MASTER)
    CurrentMaster = 0;
  if (TimeCurrentSlave.elapsed() > TIME_ELAPSED_CURRENT_SLAVE)
    CurrentSlave = 0;

  if (TimeSpeedMaster.elapsed() > TIME_ELAPSED_SPEED_MASTER)
    SpeedMaster = 0;
  if (TimeSpeedSlave.elapsed() > TIME_ELAPSED_SPEED_SLAVE)
    SpeedSlave = 0;
}

void GDHoverTelemetry::handleBatteryVoltage(QByteArray &data) {
  Q_UNUSED(data)
  TimeBattery.start();
}

void GDHoverTelemetry::handleCurrent(QByteArray &data,
                                     GDHoverTelemetry::BoardType board) {
  Q_UNUSED(data)
  if (board == btMaster)
    TimeCurrentMaster.start();
  else if (board == btSlave)
    TimeCurrentSlave.start();
}

void GDHoverTelemetry::handleRealSpeed(QByteArray &data,
                                       GDHoverTelemetry::BoardType board) {
  Q_UNUSED(data)
  if (board == btMaster)
    TimeSpeedMaster.start();
  else if (board == btSlave)
    TimeSpeedSlave.start();
}
