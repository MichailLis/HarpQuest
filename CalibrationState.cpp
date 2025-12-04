#include "CalibrationState.h"

/**
 * Конструктор CalibrationState
 * Инициализирует состояние калибровки
 * 
 * @param manager указатель на менеджер игры
 */
CalibrationState::CalibrationState(GameManager* manager) : IGameState(manager)
{
#ifdef DEBUG_LOG
    // Выводим отладочную информацию о переходе в режим калибровки
    Serial.println();
    Serial.println("Set calibration mode");
    Serial.println();
#endif
    // Проигрываем звуковой сигнал начала калибровки
    Mp3Player::PlaySample(SAMPLE_START_CALIBRATION);
    Mp3Player::WaitUntilFinish();

    // Очищаем текущую последовательность игры, чтобы задать новую
    m_GameManager->GetGameSequence()->Clear();
}

/**
 * Сохраняет текущую последовательность в энергонезависимую память (EEPROM)
 * 
 * @param address адрес в памяти, куда сохранять последовательность
 * @return true если сохранение прошло успешно, false в противном случае
 */
bool CalibrationState::SaveSequenceToEEPROM(int address)
{
#ifdef DEBUG_LOG
    // Выводим отладочную информацию о начале сохранения
    Serial.println();
    Serial.println("Start saving sequence to EEPROM");
    Serial.print("Code address: ");
    Serial.println(address);
#endif

    // Получаем указатель на игровую последовательность
    Sequence* gameSequence = m_GameManager->GetGameSequence();

    // Получаем количество элементов в последовательности
    int numElements = gameSequence->Length();

    // Проверяем, что последовательность не пуста
    if (!numElements)
    {
#ifdef DEBUG_LOG
        Serial.println("ERROR: code sequence is empty");
        Serial.println();
#endif
        return false;
    }

    // Сохраняем количество элементов в первый байт
    EEPROM[address] = numElements;
    
    // Сохраняем каждый элемент последовательности
    for (int i = 0; i < gameSequence->Length(); ++i)
        EEPROM[address + i + 1] = gameSequence->GetValue(i);

#ifdef DEBUG_LOG
    // Выводим отладочную информацию о сохраненной последовательности
    Serial.print("Loaded Data ");
    for (int i = 0; i < gameSequence->Length(); ++i)
        Serial.print(gameSequence->GetValue(i));
    Serial.println();
#endif
    return true;
}

/**
 * Основной цикл состояния калибровки
 * Выполняется постоянно, пока это состояние активно
 */
void CalibrationState::Run()
{
    // Получаем доступ к необходимым объектам
    Sequence* gameSequence = m_GameManager->GetGameSequence();       // Игровая последовательность
    HarpString* harpString = m_GameManager->GetHarpStringBuffer();   // Массив струн
    int numHarpString = m_GameManager->GetNumHarpStrings();          // Количество струн

    // Если последовательность не пуста и прошло достаточно времени бездействия,
    // завершаем калибровку и сохраняем последовательность
    if (gameSequence->Length() && millis() - m_Timer > END_CALIBRATION_INTERVAL)
    {
#ifdef DEBUG_LOG
        // Выводим отладочную информацию о выбранной последовательности
        Serial.println();
        Serial.println("Code is selected");
        for (int i = 0; i < gameSequence->Length(); ++i)
            Serial.print(gameSequence->GetValue(i));
        Serial.println();
#endif

        // Проигрываем всю последовательность, чтобы пользователь услышал, что он задал
        for (int j = 0; j < gameSequence->Length(); ++j)
        {
            Mp3Player::PlaySample(gameSequence->GetValue(j), true);
            Mp3Player::WaitUntilFinish();
        }

        // Проигрываем звуковой сигнал завершения калибровки
        Mp3Player::PlaySample(SAMPLE_END_CALIBRAION);
        Mp3Player::WaitUntilFinish();
        
        // Сохраняем последовательность в EEPROM
        SaveSequenceToEEPROM(EEPROM_DATA_START_ADDRESS);

        // Возвращаемся в нормальное состояние игры
        m_GameManager->SetState(new NormalGameState(m_GameManager));
        return;
    }

    // Проверяем каждую струну на срабатывание
    for (int i = 0; i < numHarpString; ++i)
    {
        // Проверяем, была ли струна задета (с учетом дебаунса)
        if (harpString[i].IsTriggered())
        {
#ifdef DEBUG_LOG
            // Выводим отладочную информацию о нажатой струне
            Serial.println();
            Serial.print("Set data ");
            Serial.println(i);
#endif
            
            // Добавляем струну в последовательность (нумерация с 1, поэтому i+1)
            gameSequence->AddValue(i + 1);
            
            // Проигрываем звук соответствующей струны
            Mp3Player::PlaySample(i + 1, true);
            
            // Сбрасываем таймер бездействия
            m_Timer = millis();
            break;
        }
    }
}