#include "IGameState.h"
#include "GameManager.h"

/**
 * Конструктор IGameState
 * Сохраняет указатель на менеджер игры
 * 
 * @param manager указатель на менеджер игры
 */
IGameState::IGameState(GameManager* manager)
{
    // Сохраняем указатель на менеджер игры
    m_GameManager = manager;
}

/**
 * Виртуальный деструктор
 */
IGameState::~IGameState()
{
}

/**
 * Изменяет текущее состояние игры
 * Метод-помощник, который передает управление менеджеру
 * 
 * @param state новое состояние игры
 */
void IGameState::SetState(IGameState* state)
{
    // Передаем управление смены состояния менеджеру игры
    m_GameManager->SetState(state);
}