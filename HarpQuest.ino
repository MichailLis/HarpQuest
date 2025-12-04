/**
 * HarpQuest - Игра-головоломка на основе арфы для Arduino
 * 
 * Это основной файл проекта HarpQuest. Проект представляет собой
 * интерактивную игру, где игрок должен нажимать на "струны" арфы
 * в правильной последовательности.
 * 
 * Как это работает:
 * 1. Игрок должен повторить последовательность нажатий на "струны" арфы
 * 2. Если последовательность правильная, открывается замок
 * 3. С помощью кнопки можно перейти в режим калибровки
 * 4. Кнопка администратора позволяет сразу открыть замок
 * 
 * Схема подключения:
 * - Аналоговые пины A0-A4: подключены к "струнам" арфы (фоторезисторам)
 * - Пин 8: кнопка игрока
 * - Пин 7: кнопка администратора
 * - Пин 13: замок
 * - Пин 2: пин Busy для MP3-плеера
 * - Serial (пины 0,1): связь с MP3-плеером
 */

#include "Mp3Player.h"
#include "HarpString.h"

#include "GameManager.h"
#include "NormalGameState.h"
#include "CalibrationState.h"
#include "FinishGameState.h"

#include "Button.h"

// Создаем массив из 5 "струн" арфы, подключенных к аналоговым пинам A0-A4
HarpString harpString[] = 
{
  HarpString(A0),  // Струна 1, подключена к пину A0
  HarpString(A1),  // Струна 2, подключена к пину A1
  HarpString(A2),  // Струна 3, подключена к пину A2
  HarpString(A3),  // Струна 4, подключена к пину A3
  HarpString(A4)   // Струна 5, подключена к пину A4
};

// Создаем кнопки - для игрока и администратора
Button btn(BUTTON_PIN);              // Кнопка игрока (пин определен в config.h)
Button adminButton(ADMIN_BUTTON_PIN); // Кнопка администратора

// Создаем менеджер игры, который управляет всем процессом
GameManager manager;

/**
 * Функция setup вызывается один раз при включении Arduino
 * Здесь мы настраиваем все компоненты и подготавливаем игру
 */
void setup() {
  // Настраиваем пин для замка и сначала закрываем его
  pinMode(LOCKER_PIN, OUTPUT);
  digitalWrite(LOCKER_PIN, LOW);

  // Запускаем последовательный порт для отладки и связи с MP3-плеером
  Serial.begin(9600);
  
  // Инициализируем MP3-плеер и устанавливаем громкость
  Mp3Player::Init(Serial, MP3_BUSY_PIN);
  Mp3Player::SetVolume(SOUND_VOLUME);

  // Настраиваем менеджер игры и загружаем последовательность из памяти
  manager.Init(harpString, NUM_HARPSTRINGS);
  manager.LoadGameSequenceFromEEPROM(EEPROM_DATA_START_ADDRESS);
  
  // Устанавливаем начальное состояние игры - нормальный режим
  manager.SetState(new NormalGameState(&manager));

  // Настраиваем действие для длительного нажатия на кнопку игрока
  // При удержании кнопки в течение CALIBRATION_MODE_TIMEOUT переходим в режим калибровки
  btn.AddClickCallback(CreateEventHandlerFunction([](){
    manager.SetState(new CalibrationState(&manager));
  }), CALIBRATION_MODE_TIMEOUT);

  // Настраиваем действие для длительного нажатия на кнопку администратора
  // При удержании кнопки в течение ADMIN_BUTTON_TIMEOUT переходим в режим завершения игры
  adminButton.AddClickCallback(CreateEventHandlerFunction([](){
    // Передаем true вторым параметром, чтобы проиграть мелодию перед открытием
    manager.SetState(new FinishGameState(&manager, true));
  }), ADMIN_BUTTON_TIMEOUT);
}

/**
 * Функция loop вызывается постоянно в цикле после запуска
 * Здесь обрабатываются нажатия кнопок и выполняется основная логика игры
 */
void loop() 
{
  btn.Run();          // Проверяем нажатие кнопки игрока
  adminButton.Run();  // Проверяем нажатие кнопки администратора
  manager.Run();      // Выполняем основной цикл игры
}
