#ifndef SYNC_SHIFT_FINDER_H
#define SYNC_SHIFT_FINDER_H

#include "radio_channel.h"
#include "observer_packet.h"

/*! \defgroup sync Sync
 * \brief Модуль статической бибилотеки синхронизации двух каналов.
 * Определяет межканальную разницу между двумя каналами и синхронизует
 * данные второго канала относительно первого на основе определенной
 * разницы между каналами.
 * \image html class_sync.jpg "Диаграмма классов модуля sync"
 */

///@{

/*! \brief Класс синхронизации двух каналов
 * Данный класс синхронизует канал channel2 с опорным каналом channel1.
 * \attention для коррекнтой работы класса нужно чтобы опорный канал был
 * запущен раньше второго канала.
 */
class Sync2D: public Subject
{
  public:
    Sync2D(const ShPtrRadioChannel& channel1,
           const ShPtrRadioChannel& channel2,
           const ChannelData& data);
    ~Sync2D();
    /*!
     * \brief Запускает выполнение синхронизации каналов в отдельном потоке
     * с помощью класса QtConcurrent. Синхронизация выполняется в 3 этапа.
     * На этапе 1 определяется разность двух каналов.
     * На этапе 2 инициализируется буфер сдвига shiftBuffer
     * в классе ChannelEqualizer.
     * На этапе 3 происходит последовательное чтение данных из обоих каналов.
     * Как только получены одновременно данные из каналов - канал channel2
     * выравнивается на полученную разность на этапе1.
     * Этап 3 повторяется до тех пор пока не будет остановлен процесс синхронизации.
     */
    void start();
    /*!
     * \brief останавливает процесс синхронизации
      */
    void stop();
  private:
    void process();
  private:
    struct Impl;
    std::unique_ptr<Impl>d;
};
/// @}

#endif // SYNC_SHIFT_FINDER_H
