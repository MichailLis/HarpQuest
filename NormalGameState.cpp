#include "NormalGameState.h"

/**
 * Конструктор NormalGameState
 * Инициализирует основное состояние игры
 * 
 * @param manager указатель на менеджер игры
 */
NormalGameState::NormalGameState(GameManager* manager) : IGameState(manager)
{
#ifdef DEBUG_LOG
    // Выводим отладочную информацию
    Serial.println();
    Serial.println("Set normal mode");
    Serial.print("Correct sequence: ");
    Sequence* gameSequence = m_GameManager->GetGameSequence();
    for (int i = 0; i < gameSequence->Length(); ++i)
        Serial.print(gameSequence->GetValue(i));
    Serial.println();
#endif

    // Сбрасываем состояние игры
    Reset();
}

/**
 * Деструктор NormalGameState
 */
NormalGameState::~NormalGameState()
{
    // Освобождаем ресурсы, если необходимо
}

/**
 * Сбрасывает игру в начальное состояние
 * Очищает последовательность нажатий игрока и сбрасывает последнюю нажатую струну
 */
void NormalGameState::Reset()
{
    // Сбрасываем последнюю нажатую струну
    m_LastPressedHarp = -1;
    
    // Очищаем последовательность нажатий игрока
    m_GameManager->GetPlayerSequence()->Clear();
}

/**
 * Основной цикл состояния
 * Выполняется непрерывно в цикле loop() и обрабатывает нажатия на струны
 */
void NormalGameState::Run()
{
    // Получаем доступ к необходимым объектам
    auto playerSequence = m_GameManager->GetPlayerSequence();   // Последовательность игрока
    auto gameSequence = m_GameManager->GetGameSequence();       // Правильная последовательность
    auto hartStringBuffer = m_GameManager->GetHarpStringBuffer(); // Массив струн
    auto numHarpString = m_GameManager->GetNumHarpStrings();     // Количество струн

    // Если была нажата какая-то струна, проверяем, отпущена ли она
    if (m_LastPressedHarp != -1)
    {
        // Если струна больше не активна, сбрасываем флаг последней нажатой струны
        if (!hartStringBuffer[m_LastPressedHarp].IsTriggered())
        {
            m_LastPressedHarp = -1;
        }
    }

    // Проверяем каждую струну
    for (int i = 0; i < numHarpString; ++i)
    {
        // Если эта струна отличается от последней нажатой И струна активирована
        if (i != m_LastPressedHarp && hartStringBuffer[i].IsTriggered())
        {
            // Запоминаем последнюю нажатую струну
            m_LastPressedHarp = i;
            
            // Добавляем струну в последовательность игрока (нумерация с 1, поэтому i+1)
            playerSequence->AddValue(i + 1);
            
            // Проигрываем звук соответствующей струны
            Mp3Player::PlaySample(i + 1, true);
            
            // Если длина последовательности игрока совпадает с длиной правильной последовательности
            // значит игрок закончил ввод и нужно проверить результат
            if (playerSequence->Length() == gameSequence->Length())
            {
#ifdef DEBUG_LOG
                // Выводим отладочную информацию о введенной последовательности
                Serial.print("Selected sequence: ");
                for (int j = 0; j < playerSequence->Length(); ++j)
                {
                    Serial.print(playerSequence->GetValue(j));
                }
                Serial.println();
#endif
                // Ждем окончания проигрывания последнего звука
                Mp3Player::WaitUntilFinish();
                
                // Небольшая пауза перед воспроизведением введенной последовательности
                delay(WAIT_AFTER_END_SEQUENCE);
                
                // Проигрываем введенную последовательность
                for (int j = 0; j < playerSequence->Length(); ++j)
                {
                    Mp3Player::PlaySample(playerSequence->GetValue(j), true);
                    Mp3Player::WaitUntilFinish();
                }

                // Сравниваем введенную последовательность с правильной
                if (gameSequence->Compare(*playerSequence))
                {
#ifdef DEBUG_LOG
                    Serial.println("Sequence is correct");
#endif
                    // Если последовательность верна, переходим в состояние завершения
                    m_GameManager->SetState(new FinishGameState(m_GameManager));
                    return;
                }
#ifdef DEBUG_LOG
                Serial.print("incorrect sequence");
                Serial.println();
#endif
                
                // Если последовательность неверна
                delay(1000);  // Пауза для осознания ошибки
                Reset();      // Сбрасываем игру для новой попытки
            }
        }
    } 
}