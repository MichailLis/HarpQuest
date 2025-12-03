/**
 * NormalGameState.h - Обычное состояние игры
 * 
 * Этот класс представляет нормальное (основное) состояние игры,
 * в котором игрок взаимодействует со струнами арфы и пытается
 * повторить правильную последовательность.
 * 
 * Основные функции:
 * - Отслеживание нажатий на струны арфы
 * - Запись последовательности нажатий игрока
 * - Проверка введенной последовательности
 * - Переход в состояние завершения, если последовательность верна
 */

#ifndef NORMALGAMESTATE_H
#define NORMALGAMESTATE_H

#include "IGameState.h"
#include "FinishGameState.h"

class NormalGameState : public IGameState
{
private:
    // Флаг завершения игры
    bool m_Finished = false;
    
    /**
     * Сбрасывает игру в начальное состояние
     * Очищает последовательность нажатий игрока
     */
    void Reset();
public:
    /**
     * Конструктор NormalGameState
     * 
     * @param manager указатель на менеджер игры
     */
    NormalGameState(GameManager* manager);
    
    /**
     * Деструктор NormalGameState
     */
    ~NormalGameState();

    /**
     * Основной цикл состояния
     * Обрабатывает нажатия на струны и проверяет последовательность
     */
    void Run();
};

#endif