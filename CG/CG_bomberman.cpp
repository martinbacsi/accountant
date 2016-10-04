#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <stdio.h>
#include <list>
#include <iterator>
#include <queue>
#include <chrono>
#include <memory>

#ifndef _MSC_VER


#endif


int myId = -1;


using namespace std;

/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/

typedef pair<unsigned short, unsigned short> Pos;

enum StepType { MOVE, BOMB };
enum FieldType : char { Box, Empty, Wall, Item };
enum ItemType { NoItem = 0, Range = 1, ExtraBomb = 2 };



ostream& operator<<(ostream& os, const Pos& dt)
{
	os << "(" << dt.first << " " << dt.second << ") ";
	return os;
}

class Tile;
typedef pair<const Pos, Tile>  Cell;
struct Bomb
{
	int owner;
	Pos pos;
	int explosionTimer;
	int explosionRange;
	vector<Cell*> detonated;
	Bomb() {}
	Bomb(int owner, Pos pos, int explosionTimer, int explosionRange) :owner(owner), pos(pos), explosionTimer(explosionTimer), explosionRange(explosionRange) {   }
	Bomb(const Bomb& b)
	{
		explosionRange = b.explosionRange;
		pos = b.pos;
		owner = b.owner;
		explosionTimer = b.explosionTimer;
	}

};


struct Tile
{
	FieldType type;
	shared_ptr<Bomb> bomb;
	vector<shared_ptr<Bomb>> detonate;
	ItemType item;
	pair<const Pos, Tile>* from = nullptr;

	int MinBomb() const
	{
		int ret = 9;
		for (auto& b : detonate)
		{
			ret = min(ret, b->explosionTimer);
		}
		return ret;
	}

	//vector<pair<const Pos, Tile>*> to;
	int distance;
};


struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2> &p) const {
		auto h1 = std::hash<T1>{}(p.first << 3);
		auto h2 = std::hash<T2>{}(p.second);

		// Mainly for demonstration purposes, i.e. works but is overly simple
		// In the real world, use sth. like boost.hash_combine
		return h1 ^ h2;
	}
};



struct Player
{
	int id = -1;
	Pos pos;
	int bombsLeft;
	int explosionRange;
	int recharge = 8;
	int maxBombs = 1;;
};

class Field
{

	friend class Heuristic;
private:
	unordered_map<Pos, Tile, pair_hash> field;

	vector<shared_ptr<Bomb>> bombs;
	vector<ItemType> items;
	int startTimer = 0;
	void Clear()
	{
		for (auto& f : field)
		{
			f.second.from = nullptr;
			f.second.bomb = nullptr;

			f.second.detonate.clear();
			bombs.clear();
		}

	}

public:
	Field() {}

	Field(const Field &original)
	{
		//startTimer = 1;
		field = original.field;
		Clear();
		width = original.width;
		height = original.height;
		for (const auto b : original.bombs)
		{
			//Bomb dummy(*b);

			//--dummy.explosionTimer;
			AddBomb(*b);
		}
	}






	int width, height;


	void Print()
	{
		cerr << "-----------------------------------------------------------\n";
		for (int y = 0; y < height; ++y)
		{
			cerr << y % 10 << " |";

			for (int x = 0; x < width; ++x)
			{

				cerr << (char)(field.at({ x,y }).from ? min(9,field.at({ x,y }).distance)+ '0' : '#');
				cerr << " ";
			}
			cerr << endl;
		}


		cerr << "-----------------------------------------------------------\n";
		cerr << " | 0 1 2 3 4 5 6 7 8 9 0 1 2\n";
	}

	const unordered_map<Pos, Tile, pair_hash>&  Get()
	{
		return field;
	}

	void Read(int width, int height)
	{
		Clear();
		field.clear();
		this->height = height;
		this->width = width;
		for (int y = 0; y < height; y++)
		{


			string line;
			getline(cin, line);
			for (int x = 0; x < width; x++)
			{
				field[Pos(x, y)].item = NoItem;
				if (line[x] == '.')
				{
					field[Pos(x, y)].type = Empty;// = { Empty, -1, false };	
				}
				else if (line[x] == 'X')
				{
					field[Pos(x, y)].type = Wall;// = { Empty, -1, false };	
				}
				else
				{
					field[Pos(x, y)].type = Box;
					field[Pos(x, y)].item = ItemType(line[x] - '0');
				}
			}

		}
	}
	void AddItem(Pos p, int param1)
	{
		field.at(p).type = Item;
		field.at(p).item = ItemType(param1);
	}

	vector<pair<const Cell*, int>> RemoveItem(Pos p)
	{

		vector<pair<const Cell*, int>> newDetonates;

		auto& actual = field.at(p);
		actual.type = Empty;
		for (auto& b : actual.detonate)
		{
			auto newRange = DetonationRange(*field.find(b->pos), b->explosionRange);

			//cerr << newRange.size();
			for (auto& newd : newRange)
			{

				if (newd->second.detonate.end() == find(newd->second.detonate.begin(), newd->second.detonate.end(), b))
				{

					newDetonates.push_back(make_pair(newd, b->explosionTimer));
				}
			}
		}
		actual.type = Item;
		return newDetonates;
	}


	int AddBomb(Bomb& b)   //(Pos p, int param1, int param2, int owner)
	{
		int detonatedBoxes = 0;

		auto& actual = *field.find(b.pos);

		shared_ptr<Bomb> actualBomb(new Bomb(b));

		bombs.push_back(actualBomb);

		if (actualBomb->pos != b.pos)
			throw 1;
		if (actualBomb->detonated.size())
			throw 1;
		actualBomb->detonated = DetonationRange(actual, b.explosionRange);
		actual.second.detonate.push_back(actualBomb);
		actual.second.bomb = actualBomb;

		for (auto& actualDetonates : actual.second.detonate)
		{
			actualBomb->explosionTimer = min(actualBomb->explosionTimer, actualDetonates->explosionTimer);
		}
		b.explosionTimer = actualBomb->explosionTimer;
		ApplyBomb(actualBomb);

		for (auto& d : actualBomb->detonated)
		{
			if (d->second.type == Box)
			{
				bool yields = true;
				for (auto& det : d->second.detonate)
				{
					if (det->explosionTimer < actualBomb->explosionTimer || det->owner == myId)
					{
						yields = false;
						break;
					}
				}
				if (yields)
				detonatedBoxes += (d->second.item + 1);
			}

			d->second.detonate.push_back(actualBomb);
		}




		return detonatedBoxes;
	}


private:
	const pair<const Pos, Tile>* GetBestMove(const pair<const Pos, Tile>* from) const
	{
		//int latestDet = from->second.MinBomb()  ;

		const pair<const Pos, Tile>* ret = from;

		for (const Cell& c : field)
		{
			if (c.second.from)
			{
				//cerr << "detonete: "  << c.de
				//double point = min(c.second.distance, 10) / (c.second.detonate.size()*c.second.detonate.size() + 1);
				if (c.second.MinBomb() > ret->second.MinBomb())
				{
					ret = &c;


				}
			}

		}
		return ret;
	}



	vector<Cell*> DetonationRange(const Cell& cell, int range) // todo fix this shit
	{
		const Pos& bombPos = cell.first;
		vector<Cell*> ret;
		auto TestPLace = [&](Pos& p)
		{

			auto t = field.find(p);

			if (t == field.end())
				return true;

			ret.push_back(&*t);

			return p != bombPos && (t->second.type == Wall || t->second.type == Item || t->second.type == Box || t->second.bomb);
		};
		//UP


		//for (int y = bombPos.second - 1; y != bombPos.second; --y)
		for (Pos p(bombPos.first, bombPos.second - 1); p.second != bombPos.second - range; p.second--)
		{
			if (TestPLace(p))
				break;
		}


		//DOWN
		for (Pos p(bombPos.first, bombPos.second + 1); p.second != bombPos.second + range; p.second++)
		{
			if (TestPLace(p))
				break;
		}
		//LEFt
		for (Pos p(bombPos.first - 1, bombPos.second); p.first != bombPos.first - range; p.first--)
		{
			if (TestPLace(p))
				break;
		}
		//REIGHT
		for (Pos p(bombPos.first + 1, bombPos.second); p.first != bombPos.first + range; p.first++)
		{
			if (TestPLace(p))
				break;
		}
		return ret;
	}

	const Cell* GetRoute(const Cell& from, const Cell& to) const
	{
		//cerr << "from " << from.first << " to " << to.first<< endl;


		if (from.first == to.first)
			return &from;


		const Cell* prev = &to;

		while (prev->second.from->first != from.first)
		{
			prev = prev->second.from;
		}
		return prev;
	}






public:

	int GetPossibleMoves(const Pos& from, int delay = 0)
	{

		for (auto& b : bombs)
		{
			//b->explosionTimer-=delay;
		}


		for (auto& f : field)
		{
			f.second.distance = numeric_limits<int>::max();
			f.second.from = nullptr;
		}

		for (auto& b : field.at(from).detonate)
		{
			if (b->explosionTimer < 2)
			{
				return 0;
			}
		}


		field.at(from).from = &*field.find(from);
		field.at(from).distance = delay;

		int maxDist = 0;
		queue<pair<const Pos, Tile>*> q;
		q.push(&*field.find(from));



		while (!q.empty())
		{
			
			auto current = q.front();
			q.pop();


			vector<pair<const Cell*, int>> extraDetonations;

			if (current->second.type == Item)
			{
				extraDetonations = RemoveItem(current->first);
			}



			int currentDist = current->second.distance + 1;

			int x = current->first.first;
			int y = current->first.second;

			auto move_if_found = [&](const Pos& p)
			{

				auto it = field.find(p);
				if (it != field.end())
				{
					if (currentDist < it->second.distance && (it->second.type == Empty || it->second.type == Item) && (!it->second.bomb || (it->second.bomb->explosionTimer < currentDist)))
					{
						bool free = true;

						for (auto& extra : extraDetonations)
						{
							if (extra.first->first == p)
							{
								if (extra.second < currentDist * 2)
								{
									free = false;
									break;
								}

							}
						}
						for (const auto& b : it->second.detonate)
						{
							if (currentDist > 8)
							{
								free = false;
								break;
							}
							if (b->explosionTimer == currentDist + 1)
							{

								free = false;
								break;
							}
						}
						if (free)
						{
							maxDist = max(maxDist, currentDist);
							it->second.distance = currentDist;
							it->second.from = current;
							q.push(&*it);
						}
					}
				}
			};
			move_if_found({ x - 1 , y });
			move_if_found({ x + 1 , y });
			move_if_found({ x, y - 1 });
			move_if_found({ x, y + 1 });
		}


		for (auto& b : field.at(from).detonate)
		{
			if (b->explosionTimer == 2)
			{
				field.at(from).from = nullptr;
				break;
			}
		}


		int possible = count_if(field.begin(), field.end(), [](const pair<const Pos, Tile>& f) {
			return f.second.from != nullptr;
		});
		//fprintf(stderr, "Found possible moves: %i\n", possible);
		return maxDist;
	}


	bool Rekt(const Pos& p)
	{
		return 0 == GetPossibleMoves(p);

	}


	void ApplyBomb(const shared_ptr<Bomb> actualBomb)
	{
		for (auto& d : actualBomb->detonated)
		{
			if (d->second.bomb && d->second.bomb->explosionTimer > actualBomb->explosionTimer)
			{
				d->second.bomb->explosionTimer = actualBomb->explosionTimer;
				ApplyBomb(d->second.bomb);
			}
		}
	}


	Pos BestMove(const Pos& from) const
	{

		return Next(from, GetBestMove(&*field.find(from))->first);
	}

	Pos Next(const Pos& from, const Pos& to) const
	{
		return GetRoute(*field.find(from), *field.find(to))->first;
	}

};



struct Step
{
	StepType command;
	string comment;
	Pos target;
	Step(StepType command, const Pos target, const char* comment = "") : command(command), comment(comment), target(target) {}
};


struct Move
{
	Pos target = { -1, -1 };
	int explosion = -2;
	int item = -1;
	int distance = numeric_limits<int>::max();
	int bombTimer = numeric_limits<int>::max();
};




class Heuristic
{
public:

	const Field& originalField;
	const Player* player;
	Field currentEnemyBombs;
	vector<const Player*> enemies;
	int boxesLeft = 0;
	Heuristic(const Field& origiField, const unordered_map<int, Player>& players, int playerID) : originalField(origiField)
	{
		currentEnemyBombs = origiField;
		for (const auto& p : players)
		{
			if (p.second.id != playerID)
				enemies.push_back(&p.second);
		}
		player = &players.at(playerID);



		for (auto& e : enemies)
		{
			if (e->bombsLeft)
			{
				Bomb dummy(e->id, e->pos, 8, e->explosionRange);
				currentEnemyBombs.AddBomb(dummy);
			}
		}
	}



	Step GetBestMove()
	{
		Move* playerMove = 0;
		vector<const Move*> detonations;
		vector<const Move*> items;

		vector<Move> possible = GetPossibeMoves();
		for (auto& m : possible)
		{
			if (m.target == player->pos)
			{
				playerMove = &m;
				continue;
			}
			if (m.item)
				items.push_back(&m);
			if (m.explosion)
				detonations.push_back(&m);

		}

		sort(detonations.begin(), detonations.end(), [](const Move* a, const  Move* b) {
			return a->explosion > b->explosion;
		});

		sort(items.begin(), items.end(), [](const Move* a, const Move* b) {
			return a->distance < b->distance;
		});

		cerr << "detonateplay\n";
		if (playerMove && (playerMove->explosion > 0 ))
		{


			if (player->bombsLeft > 0)
			{
				vector<Pos> safeNext;

				Field nextF = currentEnemyBombs;
				
				Bomb bomb(player->id, player->pos , 8, player->explosionRange);
				nextF.AddBomb(bomb);
				nextF.GetPossibleMoves(player->pos);
				bool safe = false;
			
				for (auto& c : nextF.field)
				{
					
					if (c.second.from && bomb.explosionTimer < c.second.MinBomb())
					{
						safe = true;
						safeNext.push_back(c.first);
					}
				}
				cerr << ".";
				if (safe)
				{
				
					for (auto& d : detonations)
					{
						if (find(safeNext.begin(), safeNext.end(), d->target) != safeNext.end())
						{
							return Step(BOMB, nextF.Next(player->pos, d->target));
						}
					}
					for (auto& i : items)
					{
						if (find(safeNext.begin(), safeNext.end(), i->target) != safeNext.end())
						{
							return Step(BOMB, nextF.Next(player->pos, (i->target)));
						}
					}
				}
				else if(player->recharge < 2)
				{
					cerr << "rechargeeee\n";
					return Step(MOVE,player->pos);
				}

			}
		}

		double bestDetPoint = numeric_limits<double>::min();
		double bestItemPoint = numeric_limits<double>::min();
		const Move* bestDet = 0;
		const Move* bestItem = 0;
		cerr << "item\n";
		for (auto& d : detonations)
		{
			double point = (double)d->explosion / d->distance;
			if (point > bestDetPoint)
			{
				bestDetPoint = point;
				bestDet = d;
			}
		}
		if (items.size())
		{
			bestItemPoint = (double)8 / items[0]->distance;
		}

		if (bestDet)
		{
			cerr << "best det: " << bestDet->target << " point: " << bestDetPoint << endl;
		}

		if (playerMove)
		{
			if ( bestItem  && !player->bombsLeft)
			{
				if(bestItemPoint > bestDetPoint)
					return Step(MOVE,   currentEnemyBombs.Next(player->pos,  bestItem->target));
			}
			if (bestDet)
			{
				cerr << "..............";
				return Step(MOVE, currentEnemyBombs.Next(player->pos,bestDet->target));
			}

			cerr << "..............";

			if(playerMove->explosion  && player->recharge < 2)
				return Step(MOVE, player->pos);
		}
		
		if (bestItem && (bestItemPoint > bestDetPoint   || !player->bombsLeft))
		{

			return Step(MOVE, currentEnemyBombs.Next(player->pos, bestItem->target));
		}
		cerr << "detoneee\n";
		if (bestDet)
		{
			
			cerr << "best det: " << bestDet->target << " point: " << bestDetPoint << endl;
			return Step(MOVE, currentEnemyBombs.Next(player->pos, bestDet->target));
		}

		return Step(MOVE, currentEnemyBombs.Next(player->pos, possible[0].target));

	}

private:

	vector<Move> GetPossibeMoves()
	{
		currentEnemyBombs.Print();
		
		vector<Move> ret;
		for (auto& c : originalField.field)
		{
			
			if (!Valid(c))
				continue;

			int explosion = Explode(c);

			Move move;
			move.target = c.first;
			move.explosion = explosion;
			move.item = c.second.item;
			move.distance = c.second.distance;
			move.bombTimer = c.second.MinBomb();
			ret.push_back(move);
		}
		return ret;
	}



	int Explode(const Cell& cell)
	{
		Field currentMyBomb = originalField;
		Bomb unsafeBomb(player->id, cell.first, 8, player->explosionRange);
		int detonate = currentMyBomb.AddBomb(unsafeBomb);


		currentMyBomb = currentEnemyBombs;
		Bomb safeBomb(player->id, cell.first, 8, player->explosionRange);
		currentMyBomb.AddBomb(safeBomb);
		int poss = currentMyBomb.GetPossibleMoves(cell.first,1);
		bool safe = false;


		for (auto& c : currentMyBomb.field)
		{

			if (c.second.from && safeBomb.explosionTimer < c.second.MinBomb())
			{
				safe = true;
			}
		}

		if (!safe && cell.first != player->pos)
			return -1;
		return detonate;


	}


	bool Valid(const Cell& cell)
	{
		if (!cell.second.from )
			return false;


		if (!currentEnemyBombs.field.at(cell.first).from)
			return false;


		return (0 < (currentEnemyBombs.GetPossibleMoves(cell.first, 1)));
			

		return true;
	}

};



class Map
{
public:
	int width;
	int height;
	int round = 0;
	const Player& me() const
	{
		return players.at(myId);
	}

	Map()
	{
		cin >> width >> height >> myId; cin.ignore();
	}

	//int maxBombs = 1;
	//int recharge = 8;

	vector<Bomb> bombs;


	unordered_map<int, Player> players;

	Field field;
	vector<string> rofl = { "barna", "ha", "ezt", "olvasod", "buzi", "vagy" };
	void PrintBestStep()
	{

		Heuristic h(field, players, myId);

		auto start = std::chrono::high_resolution_clock::now();
		auto closest = h.GetBestMove();
		auto end = std::chrono::high_resolution_clock::now();
		cout << (closest.command == BOMB ? "BOMB" : closest.command == MOVE ? "MOVE" : "") << " " << closest.target.first << " " << closest.target.second << " " << rofl[round % (rofl.size())] << endl;
		round++;
	}


	void GetInputs()
	{
		Field prevField = field;
		//vector<Bomb> prevBombs = bombs;

		field.Read(width, height);

		int entities;
		cin >> entities; cin.ignore();
		bombs.clear();



		for (int i = 0; i < entities; i++)
		{
			int entityType;
			int owner;
			int x;
			int y;
			int param1;
			int param2;
			cin >> entityType >> owner >> x >> y >> param1 >> param2; cin.ignore();


			if (entityType == 0) // PLAYER
			{
				players[owner].bombsLeft = param1;
				players[owner].explosionRange = param2;
				players[owner].pos = (Pos(x, y));
				players[owner].id = owner;
			}
			if (entityType == 1) // Bomb
			{
				bombs.push_back({ owner, Pos(x, y), param1,param2 });
				//param 1 rounds

				if (param1 == 8)
					players[owner].recharge = 0;
			}

			if (entityType == 2) // Item
			{
				field.AddItem(Pos(x, y), param1);
			}

		}

		for (auto& p : players)
		{
			if (round > 0 && prevField.Get().at(p.second.pos).type == Item && prevField.Get().at(p.second.pos).item == ExtraBomb)
				p.second.maxBombs++;
		}




	}




	void Step()
	{
		GetInputs();

		for (auto& b : bombs)
		{
			field.AddBomb(b);
		}
		field.GetPossibleMoves(me().pos);
		PrintBestStep();
		//field.Print();

	}

};




int main()
{

	Map map;

	// game loop
	while (1)
	{
		map.Step();
	}
}