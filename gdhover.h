#ifndef GDHOVER_H
#define GDHOVER_H

#include <QObject>
#include <QElapsedTimer>

/**
 * @brief Производит вычисление CRC16
 * @param ptr Указатель на данные
 * @param count Количество данных в байтах
 * @return
 */
uint16_t CalcCRC(uint8_t *ptr, int count);

class IHoverTransmitter {
public:
  /**
   * @brief Вызывается для отправки данных
   * @param data данные, которые необходимо отправить
   */
  virtual void transmitMaster(QByteArray data) = 0;
  virtual void transmitSlave(QByteArray data) = 0;
  virtual void updateConnection () = 0;
};

class IHoverTelemetryVisualiser
{
  virtual void telemetryUpdate () = 0;
};

class GDHoverControl {
public:
  GDHoverControl ();

  void setTransmitter(IHoverTransmitter *transmitter);

  void steering(int16_t speed, int16_t steer);

  bool isConnected ();

  void handlePacket (QByteArray &packet);
  void handlePeriodic ();

private:
  IHoverTransmitter *Transmitter;
  QElapsedTimer TimerConnection;

  bool ConnState;
};

class GDHoverTelemetry {
public:
  enum BoardType {
    btMaster,
    btSlave
  };

  GDHoverTelemetry ();

  void setTransmitter(IHoverTransmitter *transmitter);
  void setTelemetryVisualiser (IHoverTelemetryVisualiser *visualiser);

  float getBateryVoltage();
  float getCurrent(BoardType board);
  float getRealSpeed(BoardType board);

  void cmdReadBateryVoltage ();
  void cmdReadCurrent(BoardType board);
  void cmdReadSpeed(BoardType board);

  void handlePacket (QByteArray &packet);
  void handlePeriodic ();

private:
  void handleBatteryVoltage (QByteArray &data);
  void handleCurrent (QByteArray &data, BoardType board);
  void handleRealSpeed (QByteArray &data, BoardType board);

private:
  IHoverTransmitter *Transmitter;
  IHoverTelemetryVisualiser *Visualizer;

  float BateryVoltage;
  float CurrentMaster, CurrentSlave;
  float SpeedMaster, SpeedSlave;

  QElapsedTimer TimeBattery;
  QElapsedTimer TimeCurrentMaster, TimeCurrentSlave;
  QElapsedTimer TimeSpeedMaster, TimeSpeedSlave;
};

#endif // GDHOVER_H
