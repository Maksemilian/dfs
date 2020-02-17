/*! \mainpage Отчет
 * \section intro Выполняемы задачи прогрыммы
 * Данное программное обеспечение позволяет:
 * - 1 Принимать сигнал двух радиоканалов приемников, расположенных на удаленных компьютерах;
 * - 2 Синхронизовать два принимаемых сигнала в реальном времени;
 * - 3 Производить удаленную настройку приемников;
 * - 4 Строить графики принимаемых сигналов.
 * \section plan План отчета
 * Выберете пункт из плана для детального рассмотрения:
 * - \subpage p1 "1.1 Задача"
 * - \subpage p2 "1.2 Структура ПО"
 * - \subpage p3 "1.3 Описание отдельных модулей"
 * - \subpage p4 "1.4 Перечень разработанных модулей"
 * - \subpage p5 "1.5 Как проверялась их работоспособность"
 * - \subpage p6 "1.6 Сколько и какие модули требуют разработки,требуемое время"
 * - \subpage p7 "1.7 Какие трудности и проблемы при разработке требуют внешнего вмешательства"
 */

/*! \page p1 1.1 Задача
* \subsection s1_1 Синхронизация
* Разработать модуль синхронизации одного и двух радио-каналов по огибающей,
* относительно опорного канала.
* \subsection s1_2 Мониторинг удаленных приемных устройств
* - Вывод информации о приемных устройствах , размещенных на удаленых компьютерах
* в виде дерева;
* - Проверка состояния работы устройств по сети;
* - Выбор устройств для синхронизоции их радиоданных и отображение их в виде списка.
* \subsection s1_3 Тестирование
* - Тестирование модуля синхронизации; 
* - Тестирование сетевого модуля. \n
* Перейти к разделу \ref p2
*/

/*! \page p2 1.2 Структура ПО 
* \image html class_full.jpg "Диаграмма классов ПО"
* 	На диаграмме классов представлены структура программного обеспечения состоящая из 7 модулей. \n
* Два модуля (client и server) являются исполняемыми приложениями. Модуль client является 
* графическим прилжением , а модуль server является консольным приложением. \n
* 	Остальные 5 модулей (base,sync,proto,net,receiver) являются статическими библиотеками. \n
* Пунктирными стрелками изображена зависиость модулей (стрелка направлена от зависимого модуля). \n
* Перейти к разделу \ref p3 
*/
 
/*! \page p3 1.3 Описание отдельных модулей
*  \subsection s3_1 Модуль client
*  \image html class_client.jpg "Диаграмма классов модуля client"
*  Модуль графического приложения клиента . Позволяет удаленно настраивать и управлять
*  приемными устройствами , получать данные радиосигнала с выбранных устройств и 
*  строить графики полученных сигналов.
* \n Подробное описание классов модуля \ref client
*  \subsection s3_2 Модуль server
*  \image html class_server.jpg "Диаграмма классов модуля server"
*  Консольное приложением, которое управляет подключившимися клиентами на стороне сервера.
*  В задачи управления клиентами входит:
* -  ожидание и создание новых клиетов;
* -  прием и выполненение команд управления приемными устройствам от подключенных клиентов.
* \n Подробное описание классов модуля \ref server
*  \subsection s3_3 Модуль net 
*  \image html class_net.jpg "Диаграмма классов модуля net"
* Статическая библиотека сетевого взаимодействия по протоколу TCP. В ее задачи входит 
* отправка и прием сетевых команд и пакетов данных радиосигнала ddc. 
*\n Подробное описание классов модуля \ref net
* \subsection s3_4 Модуль receiver 
* \image html class_receiver.jpg "Диаграмма классов модуля receiver"
* Статическая библиотека для управления приемниками Winradio WRG35DDC и Trimble Thunderbolt.
* \n Подробное описание классов модуля \ref receiver
* \subsection s3_5  Модуль sync
* \image html class_sync.jpg "Диаграмма классов модуля sync"
* Статическая библиотека для синхронизации двух радиоканалов ddc.
* \n Подробное описание классов модуля \ref sync
* \subsection s3_6 Модуль base
* \image html class_base.jpg "Диаграмма классов модуля base"
* Статическая библиотека вспомогательных классов.
* \n Подробное описание классов модуля \ref base
* \subsection s3_7 Модуль proto
* Статическая библиотека , содержащая сгенерированные компилятором protobuf классы сообщений команд и
* пакетов данных радиосигнала, отправляемые по сети. \n
* Перейти к  разделу \ref p4
*/

/*! \page p4 1.4 Перечень разработанных модулей
* На данный момент разработаны следующие модули:
* - base 
* - receiver
* - server
* - client
* - proto 
* - net
* - sync \n
* Перейти к разделу \ref p5
*/

/*! \page p5 1.5 Как проверялась работоспособность модулей
* Работоспособность отдельных модулей не тестировалась. \n
* На данный момент проверялся функционал синхронизации радиоканалов выбранных устройств.
* Проверка производилась с помощью построения графика элипса двух радиосигналов сумма-разностным методом. \n
* Подтверждение правильности работы синхронизации являются следующие критерии:
* - Построение графика элипса. Построения окружности означает ,что каналы не синхронизованны;
* - При перезапуске синхронизации элипс должен находиться в
* стабильном вертикальном положении(в лабораторных условиях из-за неидентичности кабелей и задержки
* оборудывания возможно отклонение от вертикали). \n
* На данный момент был выполнен только первый критерий. Причина не выполнения второго критерия 
* в том , что алгоритм синхронизации выравнивал произвольный радиоканал. Решением данной проблемы может служить 
* выбор опорного канала , относительно которого будет выравниваться второй канал. \n
* Перейти к разделу \ref p6
*/

/*! \page p6 1.6 Сколько, какие модули требуют разработки ,требуемое время
* На данный момент основной доработки требует модуль sync. В нем требуется разработать синхронизацию
* для трех каналов , а также провести тестирование синхронизации для двух и трех каналов. \n
* Требуемое время разаработки 2 недели; \n   
* Перейти к разделу \ref p7 
*/

/*! \page p7 1.7 Какие трудности и проблемы при разработке требуют внешнего вмешательства
* Трудности требующие внешнего вмешательства:
* - Учет дробной части разности PPS двух каналов;
* - Понимание суммаразностного метода. \n
* Перейти к разделу \ref p1 
*/
