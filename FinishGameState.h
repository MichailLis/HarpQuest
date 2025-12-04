/**
 * FinishGameState.h - Состояние завершения игры
 * 
 * Этот класс представляет состояние, в которое переходит игра
 * после успешного ввода правильной последовательности.
 * В этом состоянии открывается замок и воспроизводится
 * звуковой сигнал успешного завершения.
 */

#ifndef FINISHGAMESTATE_H
#define FINISHGAMESTATE_H

#include "IGameState.h"

class FinishGameState : public IGameState
{
private:
    // Флаг, указывающий, нужно ли проигрывать мелодию перед открытием
    bool m_PlaySequence;

public:
    /**
     * Конструктор FinishGameState
     * 
     * @param manager указатель на менеджер игры
     * @param playSequence если true, то перед открытием замка будет проиграна выигрышная мелодия
     */
    FinishGameState(GameManager* manager, bool playSequence = false);

    /**
     * Основной цикл состояния завершения игры
     * Открывает замок, воспроизводит звук и через заданное время
     * возвращает игру в нормальное состояние
     */
    void Run();
};

#endif