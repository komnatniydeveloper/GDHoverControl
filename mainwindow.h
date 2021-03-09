#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "gdhover.h"
#include "serialportsettingsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct SerialPacket {
  QByteArray Buffer;
  bool PackBegin;
  bool PackEnd;
};

class MainWindow : public QMainWindow,
                   public IHoverTransmitter,
                   public IHoverTelemetryVisualiser {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public:
  // IHoverTransmitter interface
  /**
   * @brief Производит отрисовку состояния подключения
   */
  void updateConnection();

  // IHoverTransmitter interface
  /**
   * @brief Отправляет данные в ведущую плату гироскутера
   * @param data отправляемые данные
   */
  void transmitMaster(QByteArray data);

  /**
   * @brief Отправляет данные в ведомую плату гироскутера
   * @param data отправляемые данные
   */
  void transmitSlave(QByteArray data);

  // IHoverTelemetryVisualiser interface
  /**
   * @brief Производит отрисовку показаний телеметрии
   */
  void telemetryUpdate();

private slots:
  /**
   * @brief Открывает окно настроек последовательного порта ведущей платы
   */
  void portMasterClick();

  /**
   * @brief Закрывает последовательный порт ведущей платы
   */
  void portMasterCloseClick();

  /**
   * @brief Открывает окно настроек последовательного порта ведомой платы
   */
  void portSlaveClick();

  /**
   * @brief Закрывает последовательный порт ведомой платы
   */
  void portSlaveCloseClick();

  /**
   * @brief Открывает окно отображения пакетов для ведущей платы
   */
  void packetsMasterClick();

  /**
   * @brief Открывает окно отображения пакетов для ведомой платы
   */
  void packetsSlaveClick();

  /**
   * @brief Вызывается при изменении значения скорости
   * @param value новое значение скорости
   */
  void sliderSpeedChange(int value);

  /**
   * @brief Сбрасывает текущее значение скорости в нулевое значение
   */
  void sliderSpeedReset();

  /**
   * @brief Вызывается при изменении значения руля
   * @param value новое значение руля
   */
  void sliderSteerChange(int value);

  /**
   * @brief Сбрасывает текущее значение руля в нулевое значение
   */
  void sliderSteerReset();

  /**
   * @brief Визуализирует ошибку последовательного порта ведущей платы
   * @param error тип ошибки
   */
  void portErrorMaster(QSerialPort::SerialPortError error);

  /**
   * @brief Визуализирует ошибку последовательного порта ведомой платы
   * @param error тип ошибки
   */
  void portErrorSlave(QSerialPort::SerialPortError error);

  /**
   * @brief Вызывается при получении данных по последовательному порту ведущей платой
   */
  void readyReadMaster();

  /**
   * @brief Вызывается при получении данных по последовательному порту ведомой платой
   */
  void readyReadSlave();

private:
  /**
   * @brief Проверяет состояние пакета данных
   * @param packet указатель на проверяемый пакет
   * @param byte новый байт данных
   */
  void checkPacket(SerialPacket *packet, char byte);

private:
  Ui::MainWindow *ui;
  GDHoverControl HoverControl;
  GDHoverTelemetry HoverTelemetry;
  QSerialPort PortMaster, PortSlave;

  SerialPacket PacketMaster, PacketSlave;

  int TimerControl, TimerControlPeriodic, TimerTelemetry;

protected:
  // QObject interface
  /**
   * @brief Вызывается периодически при срабатывании таймеров
   * @param event событие таймера
   */
  void timerEvent(QTimerEvent *event);
};
#endif // MAINWINDOW_H
