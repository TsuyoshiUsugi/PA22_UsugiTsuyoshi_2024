# include <Siv3D.hpp> // Siv3D v0.6.13

//titleの定数
constexpr Size TITLE_SIZE = { 800, 600 };
constexpr Vec2 TITLE_POS = { 400, 300 };
//インゲームの定数
constexpr int RADIUS = 40;
constexpr float BALLSPEED = 500.0f;
constexpr Vec2 BALLSIZE = { 40, 20 };
constexpr Vec2 BLOCKSIZE = { 40, 20 };
constexpr int XCOUNT = 20;
constexpr int YCOUNT = 5;
constexpr int MAX = XCOUNT * YCOUNT;
constexpr Size BRICKSIZE = { 40, 20 };
constexpr int BALLDUR = 5;
//インゲームのスコア
int SCORE = 0;

class State
{
public:
	virtual void update() = 0;
	virtual void start(State& nextState) = 0;
};

class Title : public State
{
	Font font{ 60 };
	State* gameState = nullptr;

public:
	void start(State& nextState) override
	{
		SCORE = 0;
		Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	}
	void update() override;
};

void Title::update()
{
	while (System::Update())
	{
		font(U"ブロック崩し").drawAt(Scene::Center(), Palette::Skyblue);
		if (SimpleGUI::ButtonAt(U"スタート", TITLE_POS + Vec2{ 0, 100 }))
		{
			break;
		}
	}
}

class Game : public State
{
	Font font{ 60 };
	State* gameState = nullptr;

public:
	void start(State& nextState) override
	{
		gameState = &nextState;
	}
	void update() override
	{
		Vec2 ballVelocity{ 0, -BALLSPEED };
		Circle ball{ 400, 400, 8 };
		Rect bricks[MAX]{};
		for (int y = 0; y < YCOUNT; ++y) {
			for (int x = 0; x < XCOUNT; ++x) {
				int index = y * XCOUNT + x;
				bricks[index] = Rect{ x * (BRICKSIZE.x + BALLDUR), y * (BRICKSIZE.y + BALLDUR), BRICKSIZE };
			}
		}
		while (System::Update())
		{
			auto paddleX = Clamp(Cursor::Pos().x, 30, 770);
			const Rect paddle{ Arg::center(paddleX, 500), 60, 10 };
			ball.moveBy(ballVelocity * Scene::DeltaTime());

			for (int i = 0; i < MAX; ++i)
			{
				auto& refBrick = bricks[i];

				if (refBrick.intersects(ball))
				{
					// ブロックの上辺、または底辺と交差
					if (refBrick.bottom().intersects(ball) || refBrick.top().intersects(ball))
					{
						ballVelocity.y *= -1;
					}
					else
					{
						ballVelocity.x *= -1;
					}
					refBrick.y -= 600;
					SCORE += 1;
					break;
				}
			}

			// 天井との衝突を検知
			if ((ball.y < 0) && (ballVelocity.y < 0))
			{
				ballVelocity.y *= -1;
			}

			if (((ball.x < 0) && (ballVelocity.x < 0)) || ((Scene::Width() < ball.x) && (0 < ballVelocity.x)))
			{
				ballVelocity.x *= -1;
			}

			if ((0 < ballVelocity.y) && paddle.intersects(ball))
			{
				ballVelocity = Vec2{ (ball.x - paddle.center().x) * 10,-ballVelocity.y
				}.setLength(BALLSPEED);
			}

			if (ball.y > 600 || SCORE == MAX)
			{
				break;
			}
			for (size_t i = 0; i < MAX; i++)
			{
				bricks[i].stretched(1).draw(HSV(bricks[i].y - 40));
			}

			paddle.rounded(3).draw();
			ball.draw();

			font(U"Score:", SCORE).drawAt(Scene::Center() - Vec2{0, 1}.movedBy(0, 200));

		}

	}
};

class GameOver : public State
{
	Font font{ 60 };
	State* gameState = nullptr;

public:
	void start(State& nextState) override
	{
		gameState = &nextState;
		Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	}
	void update() override
	{
		while (System::Update())
		{
			font(U"Game Over").drawAt(Scene::Center() - Vec2{ 0, 100 }, Palette::Black);
			font(U"スコア: " + Format(SCORE)).drawAt(Scene::Center(), Palette::Black);
			if (SimpleGUI::ButtonAt(U"リトライ", TITLE_POS + Vec2{ 0, 100 }))
			{
				break;
			}
		}
	}
};

class GameClear : public State
{
	void start(State& nextState) override
	{
	}
	void update() override
	{
	}
};

void Main()
{
	Title title;
	Game game;
	GameOver gameover;

	while (System::Update())
	{
		title.start(game);
		title.update();
		game.start(gameover);
		game.update();
		gameover.start(title);
		gameover.update();
	}
}


