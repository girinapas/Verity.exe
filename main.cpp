#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Простая 2D текстовая игра-песочница "Mini-Terraria" на C++
// Здесь есть генерация мира, копание блоков, инвентарь и крафт предметов.

const int WORLD_WIDTH = 20;
const int WORLD_HEIGHT = 10;

// Типы блоков мира
enum BlockType {
    AIR = 0,
    GRASS,
    DIRT,
    STONE,
    IRON_ORE,
    WOOD
};

// Структура предмета в инвентаре
struct Item {
    std::string name;
    int count;
};

// Функция для получения символа блока
char getBlockSymbol(BlockType type) {
    switch (type) {
        case AIR:       return ' ';
        case GRASS:     return '"';
        case DIRT:      return '.';
        case STONE:     return '#';
        case IRON_ORE:  return '%';
        case WOOD:      return '|';
        default:        return '?';
    }
}

// Функция для получения названия предмета из блока
std::string getBlockName(BlockType type) {
    switch (type) {
        case GRASS:     return "Grass Block";
        case DIRT:      return "Dirt Block";
        case STONE:     return "Stone Block";
        case IRON_ORE:  return "Iron Ore";
        case WOOD:      return "Wood";
        default:        return "Unknown";
    }
}

int main() {
    std::srand(std::time(nullptr));

    // 1. Генерация мира Terraria в миниатюре
    BlockType world[WORLD_HEIGHT][WORLD_WIDTH];
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            if (y < 3) {
                // Воздух, но иногда генерируем дерево на поверхности
                if (y == 2 && x % 6 == 3) {
                    world[y][x] = WOOD;
                } else {
                    world[y][x] = AIR;
                }
            } else if (y == 3) {
                world[y][x] = GRASS; // Поверхность земли
            } else if (y < 7) {
                world[y][x] = DIRT;  // Подземелье (земля)
            } else {
                // Глубокое подземелье (камень и железная руда)
                if (std::rand() % 5 == 0) {
                    world[y][x] = IRON_ORE;
                } else {
                    world[y][x] = STONE;
                }
            }
        }
    }

    // Инициализация игрока
    int playerX = WORLD_WIDTH / 2;
    int playerY = 2; // Начинает в воздухе над травой
    std::vector<Item> inventory;

    auto addToInventory = [&](std::string name, int amount) {
        for (auto& item : inventory) {
            if (item.name == name) {
                item.count += amount;
                return;
            }
        }
        inventory.push_back({name, amount});
    };

    std::cout << "=== ДОБРО ПОЖАЛОВАТЬ В MINI-TERRARIA C++ ===" << std::endl;
    std::cout << "Управление: A (влево), D (вправо), W (вверх), S (вниз)" << std::endl;
    std::cout << "Копать: M + направление (например, MA - копать влево, MS - копать под себя)" << std::endl;
    std::cout << "Крафт: C (открыть меню крафта), Q (выход)" << std::endl << std::endl;

    while (true) {
        // Отрисовка мира
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            for (int x = 0; x < WORLD_WIDTH; ++x) {
                if (x == playerX && y == playerY) {
                    std::cout << 'P'; // Игрок
                } else {
                    std::cout << getBlockSymbol(world[y][x]);
                }
            }
            std::cout << std::endl;
        }

        // Вывод инвентаря
        std::cout << "\n[Инвентарь]: ";
        if (inventory.empty()) std::cout << "Пусто";
        for (const auto& item : inventory) {
            std::cout << item.name << " (" << item.count << ") ";
        }
        std::cout << "\nПозиция: X=" << playerX << ", Y=" << playerY << std::endl;

        // Ввод команды
        std::cout << "Ваш ход: ";
        std::string input;
        std::cin >> input;

        if (input.empty()) continue;
        char cmd = toupper(input[0]);

        if (cmd == 'Q') {
            std::cout << "Выход из игры. До встречи!" << std::endl;
            break;
        }

        // Меню Крафта
        if (cmd == 'C') {
            std::cout << "\n--- МЕНЮ КРАФТА ---" << std::endl;
            std::cout << "1. Железный меч (Требуется: 3 Iron Ore)" << std::endl;
            std::cout << "2. Кирка (Требуется: 2 Wood, 2 Stone Block)" << std::endl;
            std::cout << "Выберите номер или любой другой символ для отмены: ";
            char craftChoice;
            std::cin >> craftChoice;

            if (craftChoice == '1') {
                bool found = false;
                for (auto& item : inventory) {
                    if (item.name == "Iron Ore" && item.count >= 3) {
                        item.count -= 3;
                        addToInventory("Iron Sword", 1);
                        std::cout << "Вы скрафтили Железный меч!" << std::endl;
                        found = true;
                        break;
                    }
                }
                if (!found) std::cout << "Недостаточно ресурсов!" << std::endl;
            } else if (craftChoice == '2') {
                int woodIdx = -1, stoneIdx = -1;
                for (size_t i = 0; i < inventory.size(); ++i) {
                    if (inventory[i].name == "Wood" && inventory[i].count >= 2) woodIdx = i;
                    if (inventory[i].name == "Stone Block" && inventory[i].count >= 2) stoneIdx = i;
                }
                if (woodIdx != -1 && stoneIdx != -1) {
                    inventory[woodIdx].count -= 2;
                    inventory[stoneIdx].count -= 2;
                    addToInventory("Pickaxe", 1);
                    std::cout << "Вы скрафтили Кирку!" << std::endl;
                } else {
                    std::cout << "Недостаточно ресурсов!" << std::endl;
                }
            }
            continue;
        }

        // Логика копания (Mine)
        if (cmd == 'M' && input.length() > 1) {
            char dir = toupper(input[1]);
            int targetX = playerX;
            int targetY = playerY;

            if (dir == 'A') targetX--;
            else if (dir == 'D') targetX++;
            else if (dir == 'W') targetY--;
            else if (dir == 'S') targetY++;

            if (targetX >= 0 && targetX < WORLD_WIDTH && targetY >= 0 && targetY < WORLD_HEIGHT) {
                if (world[targetY][targetX] != AIR) {
                    std::string droppedItem = getBlockName(world[targetY][targetX]);
                    addToInventory(droppedItem, 1);
                    world[targetY][targetX] = AIR;
                    std::cout << "Вы добыли: " << droppedItem << "!" << std::endl;
                } else {
                    std::cout << "Там ничего нет!" << std::endl;
                }
            } else {
                std::cout << "Край мира!" << std::endl;
            }
            continue;
        }

        // Логика перемещения
        int nextX = playerX;
        int nextY = playerY;

        if (cmd == 'A') nextX--;
        else if (cmd == 'D') nextX++;
        else if (cmd == 'W') nextY--;
        else if (cmd == 'S') nextY++;

        // Проверка коллизий с блоками
        if (nextX >= 0 && nextX < WORLD_WIDTH && nextY >= 0 && nextY < WORLD_HEIGHT) {
            if (world[nextY][nextX] == AIR) {
                playerX = nextX;
                playerY = nextY;
            } else {
                std::cout << "Путь заблокирован! Сначала выкопайте этот блок (M + направление)." << std::endl;
            }
        } else {
            std::cout << "Край мира!" << std::endl;
        }

        // Гравитация (простая проверка: если под ногами воздух — падаем)
        while (playerY + 1 < WORLD_HEIGHT && world[playerY + 1][playerX] == AIR) {
            playerY++;
        }
    }

    return 0;
}
