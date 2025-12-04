#include "FinishGameState.h"
#include "NormalGameState.h"

/**
 * Конструктор FinishGameState
 * Инициализирует состояние завершения игры
 * 
 * @param manager указатель на менеджер игры
 * @param playSequence если true, то перед открытием замка будет проиграна выигрышная мелодия
 */
FinishGameState::FinishGameState(GameManager* manager, bool playSequence) : IGameState(manager)
{
    m_PlaySequence = playSequence;
#ifdef DEBUG_LOG
    // Выводим отладочную информацию о переходе в режим завершения
    Serial.println();
    Serial.println("Set finish mode");
    Serial.println();
#endif
}

/**
 * Основной цикл состояния завершения игры
 * Выполняется постоянно, пока это состояние активно
 */
void FinishGameState::Run()
{
    // Если нужно проиграть мелодию (например, при открытии админом)
    if (m_PlaySequence)
    {
        Sequence* gameSequence = m_GameManager->GetGameSequence();

        // Проигрываем всю последовательность
        for (int j = 0; j < gameSequence->Length(); ++j)
        {
            Mp3Player::PlaySample(gameSequence->GetValue(j), true);
            Mp3Player::WaitUntilFinish();
        }
    }

#ifdef DEBUG_LOG
    // Выводим отладочную информацию о времени активации замка
    Serial.println("Enable lock for ");
    Serial.print(FINISH_GAME_TIMEOUT);
    Serial.print(" ms");
#endif
    
    // Активируем замок (HIGH - открыт, LOW - закрыт)
    digitalWrite(LOCKER_PIN, HIGH);
    
    // Ждем заданное время, в течение которого замок будет открыт
    delay(FINISH_GAME_TIMEOUT);
    
    // Закрываем замок
    digitalWrite(LOCKER_PIN, LOW);

    // Возвращаемся в нормальное состояние игры
    SetState(new NormalGameState(m_GameManager));
}