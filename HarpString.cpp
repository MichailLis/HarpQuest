#include "HarpString.h"

// Время в миллисекундах для подавления дребезга
#define DEBOUNCE_DELAY 50

/**
 * Конструктор HarpString
 * Инициализирует пин как вход
 * 
 * @param pin номер аналогового пина, к которому подключен фоторезистор
 */
HarpString::HarpString(int pin)
{
    // Сохраняем номер пина
    m_Pin = pin;
    
    // Настраиваем пин как вход
    pinMode(m_Pin, INPUT);

    // Инициализируем переменные для дебаунса
    m_DebounceTimer = 0;
    m_LastPhysicalState = false;
    m_StableState = false;
}

/**
 * Получает текущее значение с аналогового пина
 * 
 * @return текущее значение с фоторезистора (0-1023)
 */
int HarpString::GetValue()
{
    // Читаем аналоговое значение с пина (0-1023)
    return analogRead(m_Pin);
}

/**
 * Проверяет, "задета" ли струна в данный момент
 * Использует алгоритм подавления дребезга (debounce)
 * Возвращает true ТОЛЬКО в момент срабатывания (rising edge),
 * то есть один раз на каждое нажатие.
 * 
 * @return true если струна только что была задета, false в остальных случаях
 */
bool HarpString::IsTriggered()
{
    // Определяем текущее физическое состояние
    bool currentPhysicalState;

#ifdef HARPSTRING_HIGHLIGHTED_DEFAULT
    // Струна задета, если значение БОЛЬШЕ порога
    currentPhysicalState = (GetValue() > HARPSTIRNG_THRESHOLD);
#else
    // Струна задета, если значение МЕНЬШЕ порога
    currentPhysicalState = (GetValue() < HARPSTIRNG_THRESHOLD);
#endif

    // Если физическое состояние изменилось с момента последнего опроса
    if (currentPhysicalState != m_LastPhysicalState)
    {
        // Сбрасываем таймер дебаунса
        m_DebounceTimer = millis();
    }

    // Запоминаем текущее физическое состояние как "последнее" для следующего цикла
    m_LastPhysicalState = currentPhysicalState;

    // Проверяем, прошло ли достаточно времени для стабилизации сигнала
    if ((millis() - m_DebounceTimer) > DEBOUNCE_DELAY)
    {
        // Если стабильное состояние отличается от текущего физического
        // (значит произошло реальное изменение состояния)
        if (currentPhysicalState != m_StableState)
        {
            // Обновляем стабильное состояние
            m_StableState = currentPhysicalState;

            // Если новое состояние - "активно" (нажато), возвращаем true
            if (m_StableState)
            {
                return true;
            }
        }
    }

    return false;
}
