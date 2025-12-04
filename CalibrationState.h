/**
 * CalibrationState.h - Состояние калибровки
 * 
 * Этот класс представляет режим калибровки, в котором можно
 * настроить чувствительность "струн" арфы и задать новую
 * правильную последовательность нажатий.
 * 
 * В этом режиме все нажатия на струны записываются и могут быть
 * сохранены как новая последовательность для игры.
 */

#ifndef CALIBRATIONSTATE_H
#define CALIBRATIONSTATE_H

#include "IGameState.h"
#include "NormalGameState.h"
#include <EEPROM.h>

class CalibrationState : public IGameState
{
private:
    // Таймер для отслеживания времени бездействия
    unsigned long m_Timer;

    /**
     * Сохраняет текущую последовательность в энергонезависимую память (EEPROM)
     * 
     * @param address адрес в памяти, куда сохранять последовательность
     * @return true если сохранение прошло успешно, false в противном случае
     */
    bool SaveSequenceToEEPROM(int address);
public:
    /**
     * Конструктор CalibrationState
     * 
     * @param manager указатель на менеджер игры
     */
    CalibrationState(GameManager* manager);

    /**
     * Основной цикл состояния калибровки
     * Обрабатывает нажатия на струны, записывает последовательность
     * и автоматически завершает калибровку после таймаута бездействия
     */
    void Run();
};

#endif