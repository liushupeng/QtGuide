#include "snake.h"
#include <iostream>

bool Snake::LoadPlayDataFromFile(const std::string& file)
{
    std::ifstream fin(file);
    if (!fin) {
        std::cerr << "can not open file " << file << std::endl;
        return false;
    }

    return m_control.LoadPlayDataFromFile(fin);
}

bool Snake::Play(char direction)
{
    if (!m_control.GoAhead(direction)) {
        std::cout << "Game Over!\n";
        return true;
    }
    return false;
}

void Snake::PrintMatrix(QPainter& painter, int pictureSize) const
{
    m_control.m_model.ShowGame(painter, pictureSize);
}

bool Control::GoAhead(char userInputDirection)
{
    switch (userInputDirection) {
    case 'w':
    case 'W':
        // The logic of moving up: move up 1 line (the top line of the first line number is 0
        //, see the figure in the article), the column does not move
        return GoAhead(/*row_step*/ -1, /*column_step*/ 0);
    case 'a':
    case 'A': return GoAhead(0, -1);   // Following the logic of moving up
    case 'd':
    case 'D': return GoAhead(0, +1);   // Following the logic of moving up
    case 's':
    case 'S': return GoAhead(+1, 0);   // Following the logic of moving up
    default: return true;
    }
}

bool Control::LoadPlayDataFromFile(std::istream& fin)
{
    std::string line;
    std::getline(fin, line);
    std::istringstream iss(line);   // https://zhuanlan.zhihu.com/p/441027904
    int                row_count = 0, column_count = 0;
    // read row number and column number from line
    iss >> row_count >> column_count;

    for (int row = 0; row < row_count; row++) {
        std::vector<char> lineData;
        std::getline(fin, line);
        std::istringstream lineDataIss(line);
        for (int column = 0; column < column_count; column++) {
            char element;
            // read one element
            lineDataIss >> element;
            lineData.push_back(element);
            // The snake has no body at first, only a snake head
            //   https://zhuanlan.zhihu.com/p/357348144
            if (element == '1') {
                m_model.IncreaseOnlyBody(std::make_pair(row, column));
            }
        }
        m_model.AppendToBoard(lineData);
    }
    if (m_model.m_snakeBody.empty()) {
        std::cout << "snake body is empty! init game failed." << std::endl;
        assert(false);
        return false;
    }
    return true;
}

bool Control::GoAhead(int row_step, int column_step)
{
    // nextPosition = currentPostion + (row_step, column_step)
    auto nextPosition = m_model.GetNextPosition(row_step, column_step);
    if (m_model.IsGameOver(nextPosition.first, nextPosition.second)) {
        return false;
    }
    if (m_model.ExistFood(nextPosition.first, nextPosition.second)) {
        m_model.EatFood(nextPosition);
        m_model.CreateFood();
    }
    else {
        m_model.MoveOneStepTo(nextPosition);
    }

    return true;
}

void Model::EatFood(std::pair<int, int> nextPosition)
{
    // new snake head
    m_snakeBody.push_front(nextPosition);
    // Eat it straight away and don't move the tail
    m_playBoard[nextPosition.first][nextPosition.second] =
        static_cast<char>(MatrixValueEnum::SNAKE_BODY);
}

bool Model::PushFoodAt(int row, int col)
{
    bool result = false;
    if (m_playBoard[row][col] == static_cast<char>(MatrixValueEnum::NOTHING)) {
        m_playBoard[row][col] = static_cast<char>(MatrixValueEnum::FOOD);
        result                = true;
    }
    return result;
}

bool Model::ExistFood(int row, int col) const
{
    // whether the coordinates (i, j) are snake food
    return m_playBoard[row][col] == static_cast<int>(MatrixValueEnum::FOOD);
}

void Model::IncreaseOnlyBody(std::pair<int, int> nextPosition)
{
    m_snakeBody.push_front(nextPosition);
}

void Model::AppendToBoard(const std::vector<char>& lineData)
{
    m_playBoard.push_back(lineData);
}

bool Model::IsGameOver(int row, int col) const
{
    // Determine if the game is over
    //  The xy is the destination the snake head intends to go and this destination will cause
    //  gomeover for example, index outof range for example, the destination is snake body
    return (row < 0 || row >= static_cast<int>(m_playBoard.size()) || col < 0 ||
            col >= static_cast<int>(m_playBoard[0].size()) ||
            (m_playBoard[row][col] == static_cast<int>(MatrixValueEnum::SNAKE_BODY)));
}

void Model::MoveOneStepTo(std::pair<int, int> nextPosition)
{
    // snake tail move to snake new head
    // new snake head
    // move tail
    auto head                            = nextPosition;
    auto tail                            = m_snakeBody.back();
    m_playBoard[tail.first][tail.second] = static_cast<char>(MatrixValueEnum::NOTHING);
    m_playBoard[head.first][head.second] = static_cast<char>(MatrixValueEnum::SNAKE_BODY);
    m_snakeBody.push_front(head);
    m_snakeBody.pop_back();
}

void Model::CreateFood()
{
    // A new position is randomly generated
    //, but this position may already be the snake's body
    // So, creating food needs to be repeated in a newly generated random position until successful
    do {
        auto         seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 g(seed);   // mt19937 is a standard mersenne_twister_engine
        // Generates the new random coordinates
        // https://blog.csdn.net/calmreason/article/details/72655060
        int row = g() % GetRow();
        int col = g() % GetCol();
        // Once released, remember to exit the loop and let the program continue
        bool pushSuccess = PushFoodAt(row, col);
        if (pushSuccess) {
            break;
        }
    } while (true);
}

void Model::ShowGame(QPainter& painter, int pictureSize) const
{
    auto headPosition = m_snakeBody.front();
    for (size_t row = 0; row < m_playBoard.size(); row++) {
        for (size_t col = 0; col < m_playBoard[row].size(); col++) {
            auto    element = m_playBoard[row][col];
            QPixmap pixmap;
            bool    load_result = false;
            if (row == static_cast<size_t>(headPosition.first) &&
                col == static_cast<size_t>(headPosition.second)) {
                // qt window position https://zhuanlan.zhihu.com/p/666232727
                // show snake head (snake_head.jpeg)
                //(7) your code. load your picture resource in qrc file by pixmap
                load_result = pixmap.load(":/images/snake_head.jpeg");
            }
            else if (element == static_cast<char>(MatrixValueEnum::FOOD)) {
                // show food( you should add food picture to qrc by yourself)
                //  https://zhuanlan.zhihu.com/p/662881605
                //(7) your code.
                load_result = pixmap.load(":/images/food.png");
            }
            else if (element == static_cast<char>(MatrixValueEnum::NOTHING)) {
                // show nothing, that is grass.png which is in qrc already
                //(7) your code.
                load_result = pixmap.load(":/images/grass.png");
            }
            // snake body
            else if (element == static_cast<char>(MatrixValueEnum::SNAKE_BODY)) {
                // show snake_body.png which is in qrc already
                //(7) your code.
                load_result = pixmap.load(":/images/snake_body.png");
            }
            else {
                assert(false);
            }
            assert(load_result);
            //(8) your code. draw the picture in the wright position by
            // the painter's member function drawPixmap(x, y, w, h, pixmap)
            // you should give drawPixmap the right x, and y which is determined by col and row

            painter.drawPixmap(
                col * pictureSize, row * pictureSize, pictureSize, pictureSize, pixmap);
        }
    }
}
