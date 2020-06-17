#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QWaitCondition>
#include <QReadLocker>
#include <QWriteLocker>

/*!
 * \addtogroup base
 */

///@{
/*!
 * \brief Шаблонный класс кольцевого буфера
 *
 * Работает по следующему принципу. Добавляющиеся данные
 * смещают указатель записи на единицу вправо. Как только
 * указатель записи станет указывать на последний элемент
 * буфера то он перемещается на начало кольцевого буфера.
 *
 * Чтение данных происходит по схожему принципу.Только при чтении
 * используется указатель чтения. Если указатель чтения становится
 * равен указателю записи это значит , что новых данных нет и поток
 * вызвавший чтение данных засыпает на 2 милисекнды.
 * \attention Данный класс использует барьер синхронизации -
 * условную переменную QWaitCondition. Это может приводить к замиранию
 * графического интерфейса при вызове в главном потоке.
 * \todo RingBuffer размер буфер должен приводится к числу
 *   кратному степени двойки если число нечетное
 */

template<class T>
class RingBuffer
{
  public:
    //TODO размер буфер должен приводится к числу
    //кратному степени двойки если число нечетное

    RingBuffer(int bufferSize):
        _array(new T[bufferSize]),
        _writeCounter(0),
        _readCounter(0),
        _mask(bufferSize - 1),
        _size(bufferSize) {  }

    /*!
     * \brief добавляет данные в кольцевой буфер и выводит поток
     * вызвавший данный метод из состояния ожидания
     * \param packet данные любого типа для помещения в буфер
     */
    void push(const T& packet)
    {
        QWriteLocker writeLocker(&_mutex);
        _array[_writeCounter++ &_mask] = packet;
        _notEmpty.wakeOne();
    }
    /*!
     * \brief извлекает данные из кольцевого буфера
     * \param packet переменная для хранения считанных данных из буфер
     * \return возвращает true если данные успешно считаны,
     * в противном случае возвращает false и переводит
     * поток вызвавший данный метод в состояние ожидания на 2 милисекнды
     * либо пока поток не будет выведен из данного состояния
     */
    bool pop(T& packet)
    {
        QReadLocker readLocker(&_mutex);
        if( _readCounter < _writeCounter)
        {
            packet = _array[_readCounter++ &_mask];
            return true;
        }
        else
        {
            _notEmpty.wait(&_mutex, 2);
            return false;
        }
    }
    /*!
     * \brief перемещает указатель чтения и записи на начало кольцевого буфера
     */
    void reset()
    {
        QReadLocker readLocker(&_mutex);
        _writeCounter = 0;
        _readCounter = 0;
    }

    int getIndexRead()
    {
        return _readCounter;
    }
    int getIndexWrite()
    {
        return _writeCounter;
    }
    inline bool isReadable()
    {
        return _readCounter < _writeCounter;
    }
    int size()
    {
        return _size;
    }
  private:
    T* _array;
    std::atomic<int> _writeCounter;
    std::atomic<int> _readCounter;
    std::atomic<int> _mask;
    int _size = 0;
    QReadWriteLock _mutex;
    QWaitCondition _notEmpty;
};
///@}
#endif // RINGBUFFER_H
