#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <set>

using namespace std;

/**
 * Shoot enemies before they collect all the incriminating data!
 * The closer you are to an enemy, the more damage you do but don't get too close or you'll get killed.
 **/
template<typename T>
inline int Distance(const T& a, const T& b)
{
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

struct Vec2D
{
	int x;
	int y;

	friend ostream& operator<< (ostream &out,  const Vec2D& p)
	{
		out << "(" << p.x << ", " << p.y << ")";
		return out;
	}

	Vec2D operator +(const Vec2D& v) const
	{
		return{ x + v.x, y + v.y };
	}

	Vec2D operator -(const Vec2D& v) const
	{
		return{ x - v.x, y - v.y };
	}

	Vec2D& operator *=(float t)
	{
		float _x = x*t;
		float _y = y*t;
		x = floor(_x);
		y = floor(_y);
		return (*this);
	}

	void Move(const Vec2D& to, const float maxMove)
	{
		int dist = Distance(*this, to);
		if (dist > maxMove)
		{
			float rate = sqrt(maxMove / dist);
			Vec2D movement = to - *this;
			x = floor((float)movement.x * rate + (float)x);
			y = floor((float)movement.y * rate + (float)y);
		}
		else
		{
			*this = to;
		}
	}
};



inline bool operator ==(const Vec2D& a, const Vec2D& b)
{
	return a.x - b.x && a.y == b.y;
}

class Entity
{
public:
	Vec2D pos;
protected:
	Entity() {}
	int id;
};

class Enemy : public Entity
{
public:
	Enemy()
	{
		cin >> id >> pos.x >> pos.y >> life; cin.ignore();
	}
private:
	int life;
};

class Data : public Entity
{
public:
	Data()
	{
		cin >> id >> pos.x >> pos.y; cin.ignore();
	}
private:
	int life;
};

class Player : public Entity
{
public:
	Player()
	{
		cin >> pos.x >> pos.y; cin.ignore();
	}
};


int Point(int distance)
{
	return round(125000 / pow(1.2, sqrt(distance)));
}

class Field
{
public:
	void Update()
	{
		datas.clear();
		enemies.clear();
		
		int dataCount;
		cin >> dataCount; cin.ignore();

		for (int i = 0; i < dataCount; i++)
		{
			datas.insert(make_shared<Data>());
		
		}
		int enemyCount;
		cin >> enemyCount; cin.ignore();
		for (int i = 0; i < enemyCount; i++)
		{
			enemies.insert(make_shared<Enemy>());
			
		}
	}
	void Step()
	{
		for (auto e : enemies)
		{
			auto closestData = min_element(datas.begin(), datas.end(), [&] (const shared_ptr<Data> a, const shared_ptr<Data> b){
				return Distance(a->pos, e->pos) < Distance(b->pos, e->pos);
			});
			if (closestData == datas.end()) throw "no data";
			e->pos.Move((**closestData).pos, 250000.0f);
		}


	}

	void PrintAll()
	{
		for (auto e : enemies)
		{
			cerr << e->pos << " ";
		}
	}



private:
	shared_ptr<Player> player;
	set<shared_ptr<Enemy>>	enemies;
	set<shared_ptr<Data>>	datas;
};

int main()
{
	Field field;

    // game loop
    while (1)
	{
		
		field.Update();
		field.PrintAll();
		field.Step();
		field.PrintAll();
        cout << "MOVE 8000 4500" << endl; // MOVE x y or SHOOT id
    }
}