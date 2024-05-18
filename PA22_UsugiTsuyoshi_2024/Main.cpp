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


void Main()
{
#pragma region ゲームオーバー


	while (System::Update())
	{
		
	}
#pragma endregion
}

class State
{
	public:
		virtual void update() = 0;
		virtual void Start() = 0;
};

class Title : public State
{
	Font font{ 60 };
	void start() {
		Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	}
	void update() {
		while (System::Update())
		{
			font(U"ブロック崩し").drawAt(Scene::Center(), Palette::Skyblue);
			if (SimpleGUI::ButtonAt(U"スタート", TITLE_POS + Vec2{ 0, 100 }))
			{
				break;
			}
		}
	}
};

class Game : public State
{
	
	void update()
	{
		Vec2 ballVelocity{ 0, -BALLSPEED };
	Circle ball{ 400, 400, 8 };
	Rect bricks[MAX];
	int Score = 0;
	bool isGameOver = false;

	for (int y = 0; y < YCOUNT; ++y) {
		for (int x = 0; x < XCOUNT; ++x) {
			int index = y * XCOUNT + x;
			bricks[index] = Rect{ x * (BRICKSIZE.x + BALLDUR), y * (BRICKSIZE.y + BALLDUR), BRICKSIZE };
		}
	}
		auto paddleX = Clamp(Cursor::Pos().x, 30, 770);
		const Rect paddle{ Arg::center(paddleX, 500), 60, 10 };
		ball.moveBy(ballVelocity * Scene::DeltaTime());

		for (int i = 0; i < MAX; ++i) {
			auto& refBrick = bricks[i];

			if (refBrick.intersects(ball)) {

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
				Score += 1;
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

		if (ball.y > 600)
		{
			isGameOver = true;
			break;
		}
		for (size_t i = 0; i < MAX; i++)
		{
			bricks[i].stretched(1).draw(HSV(bricks[i].y - 40));
		}

		paddle.rounded(3).draw();
		ball.draw();

		font(U"Score:", Score).drawAt(Scene::Center().movedBy(0, 200));
	}
	}
};

class GameOver : public State
{
	void update() {
		font(U"Game Over").drawAt(Scene::Center(), Palette::Black);

		if (SimpleGUI::ButtonAt(U"リトライ", TITLE_POS + Vec2{ 0, 100 }))
		{
			isGameOver = false;
			goto title;	// タイトルに戻る(一番上)
		}
	}
};

class GameClear : public State
{
	void update() {}
};
