# Smart-Fan-Timer
Двухканальный "умный" таймер для двух 12-вольтовых вентиляторов с встроенным мониторингом их исправности для применения в системе вытяжки на кухне, в гараже и т.п.

Это ещё не работает! Только пишется код.

Название проекта: Smart Fan Timer
Ревизия кода: 20160312 (initial)
Авторы: Vlady (инициатор проекта)
Компания: Home, Sweet Home
Микроконтроллер: ATMega328P (32-lead TQFP)
Частота ядра и источник тактировния: внутренний RC-генератор 8 MHz

Фьюзы:
ОТКЛЮЧАЕМ внутренний делитель на 8
Ставим BODLEVEL на Vcc = 4,3V
Остальное as shipped
0xE2 - LOW
0xD9 - HIGH
0xFC - EXTENDED

Описание:

Это двухканальный "умный" таймер для двух компьютерных (12-вольтовых) вентиляторов с
встроенным мониторингом их исправности для применения в системе вытяжки на кухне, в
гараже и т.п.

Предназначен для 
- включения вентиляторов на заданный вручную период времени,
- ручного управления скоростью вращения,
- автоматического контроля исправности вентиляторов с индикацией светодиодами и бипером.

Допустимое количество управляемых и контролируемых каналов/вентиляторов - от 1 до 2. 

ОРГАНЫ УПРАВЛЕНИЯ
- три кнопки управления.
- три потенциометра.

Кнопки RUN, STOP и SELECTOR наружные, легкодоступные для пользователя. 
Кнопка RUN при коротком нажатии включает оба вентилятора на определённый
промежуток времени. При работающих вентиляторах повторное короткое нажатие на RUN в
любой момент повторно запускает таймер с начала.
Кнопка STOP в любой момент времени обнуляет время таймера и выключает оба вентилятора.
Кнопка SELECTOR (только при запущенном RUN) позволяет выбрать для работы конкретный
вентилятор или оба сразу.

Два потенциометра управляют скоростью вращения вентиляторов (регулируют
параметры FB преобразователей DC-DC), а третий потенциометр "Время" устанавливает выдержку
таймера (через АЦП микроконтроллера).

Включение вентиляторов осуществляется подачей питания (по входам ENABLE
преобразователей DC-DC), а скорость вращения вентиляторов управляется величиной
напряжения питания (цепями FB преобразователей DC-DC).

Для отслеживания исправности вентиляторов используются мониторинг потребляемого
вентиляторами тока и встроенные в вентиляторы таходатчики.
