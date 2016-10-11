    	#include <iostream>
    	#include <string>
    	#include <vector>
    	#include <algorithm>
    	#include <memory>
    	#include <map>
    	#include <stdlib.h>
    	#include <math.h>
    	#include <sstream>
    	#include <string>
    	#include <iostream>
    	#include <vector>
    	#include <chrono>
    	#include <list>

#define _USE_MATH_DEFINES 
#include <math.h>
    
    	#define DUMP
    
    	static double dCrossoverRate = 0.7;
    	static double dMutationRate = 0.4 ;
		static int	iPopulation = 20;
    	//the maximum amount the ga may mutate each weight by
    	static double dMaxPerturbation = 0.3;
    	int rounds = 0;
    	//used for elitism
    	static const int    iNumElite = 2;
    	static const int    iNumCopiesElite = 2;
    	
    	using namespace std;
		static const int	iGenomeLength = 80;
		static const int	iWeightCount = 3;
    

    	//returns a random integer between x and y
    	inline int	  RandInt(int x, int y) { return rand() % (y - x + 1) + x; }
    
    	//returns a random double between zero and 1
    	inline double RandFloat() { return (rand()) / (RAND_MAX + 1.0); }
    
    	//returns a random bool
    	inline bool   RandBool()
    	{
    		if (RandInt(0, 1)) return true;
    
    		else return false;
    	}
    
		int dmgAtDistance(double distance) {
			double dmg_x[14] = {
				2020.5993258735589	* 2020.5993258735589,
				2154.4346900318847	* 2154.4346900318847,
				2309.458400641476	* 2309.458400641476,
				2491.3453197236013	* 2491.3453197236013,
				2708.041651254433	* 2708.041651254433,
				2971.0450660370902	* 2971.0450660370902,
				3297.670744113212	* 3297.670744113212,
				3715.3284539558354	* 3715.3284539558354,
				4270.277546846042	* 4270.277546846042,
				5047.557202311502	* 5047.557202311502,
				6223.504012394597	* 6223.504012394597,
				8237.744862210333	* 8237.744862210333,
				12609.047911857519	* 12609.047911857519,
				31498.026247371843	* 31498.026247371843 };

			for (size_t i = 0; i < 14; i++)
				if (distance < dmg_x[i])
					return 14 - i;

			cerr << "0 DMG!!!! dist=" << distance << endl;
			return 0;
		}

		inline double RandomClamped() { return RandFloat() - RandFloat(); }


		struct Gene
		{
			double weight[iWeightCount];

			Gene()
			{
				for (size_t i = 0; i <iWeightCount; ++i)
				{
					weight[i] = RandFloat();
				}
			}
		};


    	
    
    	void Clamp(double &arg, double min, double max)
    	{
    		if (arg < min)
    		{
    			arg = min;
    		}
    
    		if (arg > max)
    		{
    			arg = max;
    		}
    	}
    
	
		struct Genome
		{
			list<Gene> Genes;
			int fitness;
			Genome()
			{
				Genes.resize(iGenomeLength);
			}
		};

    	class CGenAlg
    	{
    	private:
    
    		//this holds the entire population of chromosomes
    		
    
    		//size of population
    		int m_iPopSize;
    
    		//amount of weights per chromo
    		int m_iChromoLength;
    
    		//total fitness of population
    		double m_dTotalFitness;
    
    		//best fitness this population
    		double m_dBestFitness;
    
    		//average fitness
    		double m_dAverageFitness;
    
    		//worst
    		double m_dWorstFitness;
    
    		//keeps track of the best genome
    		
    
    		//probability that a chromosones bits will mutate.
    		//Try figures around 0.05 to 0.3 ish
    		double m_dMutationRate;
    
    		//probability of chromosones crossing over bits
    		//0.7 is pretty good
    		double m_dCrossoverRate;
    
    		//generation counter
    		int	  m_cGeneration;
    
    
			void Crossover(const Genome &mum,
				const Genome &dad,
				Genome       &baby1,
				Genome       &baby2);
			
    
    
			void  Mutate(Genome &chromo);
    
			Genome& GetChromoRoulette();
    
    		//use to introduce elitism
    		void	  GrabNBest(int	            NBest,
    			const int       NumCopies,
				vector<Genome>	&vecPop);
    
    
    		void	  CalculateBestWorstAvTot();
    
    		void	  Reset();
    
    
    
		public:
			void DropFirst()
			{
				for (auto& g : m_vecPop)
				{
					g.Genes.pop_front();
					g.Genes.push_back(Gene());
				}
			}
			void Epoch();
			vector<Genome>	 m_vecPop;
			vector<Genome> vecNewPop;
    		CGenAlg(int 		popsize,
    			double	MutRat,
    			double	CrossRat,
    			int	  	numweights);
    
    		double			    AverageFitness()const { return m_dTotalFitness / m_iPopSize; }
    		double		    	BestFitness()const { return m_dBestFitness; }
    	};


    
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
    
    		Vec2D& operator *=(double t)
    		{
    			double _x = x*t;
    			double _y = y*t;
    			x = floor(_x);
    			y = floor(_y);
    			return (*this);
    		}
    
    		void Move(const Vec2D& to, const int maxMove)
    		{
    			int dist = Distance(*this, to);
    			if (dist > maxMove)
    			{
    				double rate = sqrt((double)maxMove / dist);
    				Vec2D movement = to - *this;
    				x = floor((double)movement.x * rate + (double)x);
    				y = floor((double)movement.y * rate + (double)y);
    			}
    			else
    			{
    				*this = to;
    			}
    		}
    	};
    	inline bool operator ==(const Vec2D& a, const Vec2D& b)
    	{
    		return a.x == b.x && a.y == b.y;
    	}
    
    
    
    	class Enemy 
    	{
    	public:
    		Vec2D pos;
    		int life;
    		bool dead = false;
    		int id;
    	private:
    	
    	};
    
    	class Data 
    	{
    	public:
    		int id;
    		Vec2D pos;
    	};
    
    
    	enum StepType { MOVE, SHOOT };
    
    	struct Step
    	{
    		StepType type;
    		Vec2D pos;
    		double enemy = -1.0;


			friend ostream& operator<<(ostream& os, const Step& step)
			{
				os << (step.type == SHOOT ? "SHOOT " : "MOVE ");
				if (step.type == SHOOT)
					os << step.enemy;
				else
					os << step.pos;
				return os;
			}
    	};
    
    

    	class Player 
    	{
    	public:
    		Vec2D pos;
    		void Move(const Vec2D& to)
    		{
    			pos.Move(to, 1000000);
    		}
    		void Kill(Enemy& enemy, double distance)
    		{
				int dmg = dmgAtDistance(distance);
				enemy.life -= dmg;
    		}
    	};
    
    
    
    	class Field
    	{
    	public:
    		Player player;
    	
    		int enemyHP;
    		bool firstUpdate;
    		int shot;
    		vector<Enemy>	enemies;
    		vector<Data>	datas;
			int alive;
    
    		Field() :  enemyHP(0), shot(0)  
    		{
    	#ifndef _MSC_VER
    	
    	#endif // DEBUG
    		}
    
    		double FinalPoint()
    		{
				int enemyDead = count_if(enemies.begin(), enemies.end(), [](const Enemy& e) {return e.dead; });
    			return (double)(datas.size() * 100 + datas.size() * max(0, enemyHP - 3 * shot) * 3 + enemyDead * 10 );
    		}
    
    
    		void Update()
    		{
    			cin >> player.pos.x >> player.pos.y;
    			int dataCount; cin >> dataCount;
    
    
    			if (!datas.size())
    			{
    				datas.resize(dataCount);
    			}
    		
    			for (int i = 0; i < dataCount; i++)
    			{
    				int id; cin >> id;
    			
    				cin >> datas[id].pos.x >> datas[id].pos.y;
    
    				datas[id].id = id;
    			}
    			int enemyCount; cin >> enemyCount;
    
    
    	
    			if (!enemies.size())
    			{
    				enemies.resize(enemyCount);
    			}
    			
    			for (int i = 0; i < enemyCount; i++)
    			{
    				int id;
    				cin >> id;
    				cin >> enemies[id].pos.x >> enemies[id].pos.y >> enemies[id].life;
    
    				enemies[id].id = id;
    			}
    		
    	#ifdef DUMP
    
    			cerr << "DUMP\n";
    
    			uint32_t* p = reinterpret_cast<uint32_t*>(&player);
    
    			for (size_t i = 0; i < sizeof(Player); ++i)
    			{
    				cerr << p[i] << " ";
    			}
    			cerr << endl;
    			p = reinterpret_cast<uint32_t*>(&enemies[0]);
    		
    			for (size_t i = 0; i < enemies.size() * sizeof(Enemy); ++i)
    			{
    				cerr << p[i] << " ";
    			}
    			cerr << endl;
    			p = reinterpret_cast<uint32_t*>(&datas[0]);
    			for (size_t i = 0; i < datas.size() * sizeof(Data); ++i)
    			{
    				//cerr << p[i] << " ";
    			}
    
    
    			if(firstUpdate)
    			{
    				for (auto&e : enemies)
    				{
    					enemyHP += e.life;
    				}
    				firstUpdate = false;
    				cerr << "FIRST UPDATE";
    			}
				int alive = enemies.size();
    			cerr << "\nEND_DUMP\n";
    	#endif
    
    	}
    
    
    
    
    	
    		bool Step(const Step& nextStep, bool print = false)
    		{
    			for (auto& e : enemies)
    			{
    				if (!e.dead)
    				{
    					auto closestData = min_element(datas.begin(), datas.end(), [&](const Data& a, const Data& b) {
    						return Distance(a.pos, e.pos) < Distance(b.pos, e.pos);
    					});
    					e.pos.Move(closestData->pos, 250000);
    				}
    			}
    
    			if (nextStep.type == MOVE)
    			{
    				player.Move(nextStep.pos);
    
    				if (print)
    				{
    					cout << "MOVE " << player.pos.x << " " << player.pos.y << endl;
    				}
    		
    			}
    
    			for (auto&e : enemies)
    			{
    				if (Distance(player.pos, e.pos) < 4000000)
    				{
    					return true;
    				}
    			}
    
    			if (nextStep.type == SHOOT)
    			{
					//cerr << nextStep.enemy << endl;


					vector<const Enemy*> aliveEnemies;
					for (auto& e : enemies)
					{
						if (!e.dead)
						{
							aliveEnemies.push_back(&e);
						}
					}
					double n = (double)(aliveEnemies.size() - 1);
					int id = aliveEnemies[round((1.0 - nextStep.enemy) * n)]->id;


					Enemy& e = enemies[id];


    				
    				player.Kill(e, Distance(player.pos, (e.pos)));
    
		

    				//cerr << enemies.at(nextStep.enemy).life << endl;
    				if (e.life < 1)
    				{
						--alive;
    					e.dead = true;
    				}

					if (nextStep.enemy == 0 )
					{
						int i = 0;;
						i++;
					}

    				if (print)
    				{
    					cout << "SHOOT " << id << endl;
    				}
    				shot++;
    			}
    
    
				for (auto& e : enemies)
				{
					if (!e.dead)
					{
						for (int i = 0; i < datas.size(); ++i)
						{
							if (datas[i].pos == e.pos)
							{
								datas.erase(datas.begin() + i);
								break;
							}

						}
					}
    			
    			}

				if (!datas.size() && alive == enemies.size())
				{
					return true;
				}

    			return false;
    		}
    
    		bool Ended() const
    		{
    			bool allDead = true;
    			for (auto& e : enemies)
    			{
    				if (!e.dead)
    				{
    					allDead = false;
    					break;
    				}
    			}
    
    			return allDead || !datas.size() ;
    		}
    
    	};
    
    
    
    	template<typename T>
    	vector<T> ReadBinary(string bin)
    	{
    		istringstream is(bin);
    		unsigned int c;
    		list<unsigned int> nums;
    		while (is >> c)
    		{
    			nums.push_back(c);
    		}
    
    		vector<T> ret;
    	
    		auto it = nums.begin();
    
    		uint32_t* binary = new uint32_t[nums.size()];
    		for (int i = 0; i < nums.size(); ++i)
    		{
    			binary[i] = *it;
    			++it;
    		}
    		ret.resize((size_t)nums.size() / sizeof(T));
    		memcpy(ret.data(), binary, nums.size());
    		delete[] binary;
    		return ret;
    	}
    
    
    	#ifdef _MSC_VER
    	Field CreateField()
    	{
    		Field field;
    		field.player =	ReadBinary<Player>("0 1200 0 32767 0 0 6442992 0")[0];
    		field.enemies = ReadBinary<Enemy>("11200 1800 6 0 0 12800 1800 6 0 1 14400 1800 6 0 2 3200 2700 6 0 3 4800 2700 6 0 4 6400 2700 6 0 5 8000 2700 6 0 6 9600 2700 6 0 7 11200 2700 6 0 8 12800 2700 6 0 9 14400 2700 6 0 10 1600 3600 6 0 11 3200 3600 6 0 12 4800 3600 6 0 13 6400 3600 6 0 14 8000 3600 6 0 15 9600 3600 6 0 16 11200 3600 6 0 17 12800 3600 6 0 18 14400 3600 6 0 19 0 4500 6 0 20 1600 4500 6 0 21 3200 4500 6 0 22 4800 4500 6 0 23 6400 4500 6 0 24 8000 4500 6 0 25 9600 4500 6 0 26 11200 4500 6 0 27 12800 4500 6 0 28 14400 4500 6 0 29 0 5400 6 0 30 1600 5400 6 0 31 3200 5400 6 0 32 4800 5400 6 0 33 6400 5400 6 0 34 8000 5400 6 0 35 9600 5400 6 0 36 11200 5400 6 0 37 12800 5400 6 0 38 14400 5400 6 0 39 0 6300 6 0 40 1600 6300 6 0 41 3200 6300 6 0 42 4800 6300 6 0 43 6400 6300 6 0 44 8000 6300 6 0 45 9600 6300 6 0 46 11200 6300 6 0 47 12800 6300 6 0 48 14400 6300 6 0 49 0 7200 6 0 50 1600 7200 6 0 51 3200 7200 6 0 52 4800 7200 6 0 53 6400 7200 6 0 54 8000 7200 6 0 55 9600 7200 6 0 56 11200 7200 6 0 57 12800 7200 6 0 58 14400 7200 6 0 59 0 8100 6 0 60 1600 8100 6 0 61 3200 8100 6 0 62 4800 8100 6 0 63 6400 8100 6 0 64 8000 8100 6 0 65 9600 8100 6 0 66 11200 8100 6 0 67 12800 8100 6 0 68 14400 8100 6 0 69 129697 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");
    		field.datas =	ReadBinary<Data>("0 8109 4500 1 8110 4500 2 8111 4500 3 8112 4500 4 8113 4500 5 8114 4500 6 8115 4500 7 8116 4500 8 8117 4500 9 8118 4500 10 8119 4500 11 8120 4500 12 8121 4500 13 8122 4500 14 8123 4500 15 8124 4500 16 8125 4500 17 8126 4500 18 8127 4500 19 8128 4500 20 8129 4500 21 8130 4500 22 8131 4500 23 8132 4500 24 8133 4500 25 8134 4500 26 8135 4500 27 8136 4500 28 8137 4500 29 8138 4500 30 8139 4500 31 8140 4500 32 8141 4500 33 8142 4500 34 8143 4500 35 8144 4500 36 8145 4500 37 8146 4500 38 8147 4500 39 8148 4500 40 8149 4500 41 8150 4500 42 8151 4500 43 8152 4500 44 8153 4500 45 8154 4500 46 8155 4500 47 8156 4500 48 8157 4500 49 8158 4500 50 8159 4500 51 8160 4500 52 8161 4500 53 8162 4500 54 8163 4500 55 8164 4500 56 8165 4500 57 8166 4500 58 8167 4500 59 8168 4500 60 8169 4500 61 8170 4500 62 8171 4500 63 8172 4500 64 8173 4500 65 8174 4500 66 8175 4500 67 8176 4500 68 8177 4500 69 8178 4500 70 8179 4500 71 8180 4500 72 8181 4500 73 8182 4500 74 8183 4500 75 8184 4500 76 8185 4500 77 8186 4500 78 8187 4500 79 8188 4500 0 0 1409 0 11200 1800 6 0 0 12800 1800 6 0 1 14400 1800 6 0 2 3200 2700 6 0 3 4800 2700 6 0 4 6400 2700 6 0 5 8000 2700 6 0 6 9600 2700 6 0 7 11200 2700 6 0 8 12800 2700 6 0 9 14400 2700 6 0 10 1600 3600 6 0 11 3200 3600 6 0 12 4800 3600 6 0 13 6400 3600 6 0 14 8000 3600 6 0 15 9600 3600 6 0 16 11200 3600 6 0 17 12800 3600 6 0 18 14400 3600 6 0 19 0 4500 6 0 20 1600 4500 6 0 21 3200 4500 6 0 22 4800 4500 6 0 23 6400 4500 6 0 24 8000 4500 6 0 25 9600 4500 6 0 26 11200 4500 6 0 27 12800 4500 6 0 28 14400 4500 6 0 29 0 5400 6 0 30 1600 5400 6 0 31 3200 5400 6 0 32 4800 5400 6 0 33 6400 5400 6 0 34 8000 5400 6 0 35 9600 5400 6 0 36 11200 5400 6 0 37 12800 5400 6 0 38 14400 5400 6 0 39 0 6300 6 0 40 1600 6300 6 0 41 3200 6300 6 0 42 4800 6300 6 0 43 6400 6300 6 0 44 8000 6300 6 0 45 9600 6300 6 0 46 11200 6300 6 0 47 12800 6300 6 0 48 14400 6300 6 0 49 0 7200 6 0 50 1600 7200 6 0 51 3200 7200 6 0 52 4800 7200 6 0 53 6400 7200 6 0 54 8000 7200 6 0 55 9600 7200 6 0 56 11200 7200 6 0 57 12800 7200 6 0 58 14400 7200 6 0 59 0 8100 6 0 60 1600 8100 6 0 61 3200 8100 6 0 62 4800 8100 6 0 63 6400 8100 6 0 64 8000 8100 6 0 65 9600 8100 6 0 66 11200 8100 6 0 67 12800 8100 6 0 68 14400 8100 6 0 69 129697 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");
    
    		for (auto&e : field.enemies)
    		{
    			field.enemyHP += e.life;
    		}
			field.alive = field.enemies.size();
    		return field;
    	}
    	#endif
    
    
    
    
    	class Controller
    	{
    	public:
    		const Field& oField;
    		unique_ptr<CGenAlg> gA;
    		Step bestFirst;
    		double bestFitness = numeric_limits<double>::min();
    		int ioCount;
    		
    		//
    		Controller(const Field& field) : oField(field)
    		{
    
    			
    			ioCount = oField.enemies.size() + 2;
    
    			gA = unique_ptr<CGenAlg>(new CGenAlg(iPopulation, dMutationRate, dCrossoverRate, iGenomeLength));
    		}
    
    

			Step GeneToMove(const Gene& gene, const Field& field) const
			{
				double angle = 0.;
				double dist = 0.;
				int target = -1;
				Step newMove;


				//cerr << gene.weight[0] << endl;
				if (gene.weight[0] < 0.5) 
				{
					newMove.type = SHOOT;
					newMove.enemy = gene.weight[1];
					
				}
				else 
				{

					newMove.type = MOVE;

					if (gene.weight[1] > 0.2) 
					{
						dist = 1000;
					}
					else 
					{
						dist = gene.weight[1] * 2000;
					}
					angle = gene.weight[2] * M_PI * 2;
					
					double x = dist*sin(angle);
					double y = dist*cos(angle);

					newMove.pos.x = floor(field.player.pos.x + x);
					newMove.pos.y = floor(field.player.pos.y + y);

					newMove.pos.x = max(0, min(16000, newMove.pos.x));
					newMove.pos.y = max(0, min(9000, newMove.pos.y));
				}

				return newMove;
			}
    
    		double Evaulate(const Genome& net, Step& firstStep)
    		{
    			Field field = oField;
    			bool dead = false;
    			bool first = true;
				auto actual = net.Genes.begin();
				while (!dead && !field.Ended())
				{
					Step step = GeneToMove(*actual, field);
					dead = field.Step(step);
					if (first)
					{
						firstStep = step;
						first = false;
					}
					++actual;
				}

    			double point = 0;
    			if (!dead)
    			{
    				point = field.FinalPoint();
    			}
    			return point;
    		}
    
    		void NextGeneration()
    		{
				
				for (auto& g : gA->m_vecPop)
				{
					Step first;
					g.fitness = Evaulate(g, first);

					if (g.fitness > bestFitness)
					{
						bestFirst = first;
						bestFitness = g.fitness;
						cerr << "best fitness found: " << bestFitness << endl;

						for (const auto& s : g.Genes)
						{
							//cerr << GeneToMove(s, oField) << " ";
						}
						//cerr << endl;


						//cerr << (first.type == MOVE ? "MOVE" : "KILL") << " " << first.pos << endl;
					}
				}
				gA->Epoch();
				
    		}
    
    
    		Step GetBest()
    		{
    			Step ret = bestFirst;
    			bestFitness = numeric_limits<double>::min();
    			bestFirst.pos = { -1, -1 };
    			return ret;
    		}
    	};
    
    
    
    	int main()
    	{
			float fitness;
			float bestFitness = numeric_limits<double>::min();
			srand(time(0));
    		bool dead = false;
    #ifdef _MSC_VER
			while (1)
			{
				Field field = CreateField();
				Controller cont(field);
				while (!dead && !field.Ended())
	#else
				Field field;
				field.Update();
				Controller cont(field);
				while (1)
	#endif 
				{
					auto start = chrono::high_resolution_clock::now();
					int r = 0;
					while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() < 90)
					{
						++r;
						cont.NextGeneration();
					}
					rounds++;
					
					field.Step(cont.GetBest(), true);
					cont.gA->DropFirst();
					if (cont.bestFitness > bestFitness)
					{
						bestFitness = cont.bestFitness;
					}


					cerr << r << " rounds simulated\n";
				}

				//cerr <<  " mut: " << dMutationRate << " cross: " << dCrossoverRate << " perturb: " << dMaxPerturbation << " pop: " << iPopulation << endl;

				//dCrossoverRate = RandFloat();
				//dMutationRate = RandFloat();
				//iPopulation = (rand() % 20 + 1) * 2;
				//the maximum amount the ga may mutate each weight by
				//dMaxPerturbation = RandFloat();
#ifdef _MSC_VER
			}
    
    	
    		getchar();
#endif // _MSC_VER
    	}

    
    
    	CGenAlg::CGenAlg(int	  popsize,
    		double	MutRat,
    		double	CrossRat,
    		int	  numweights) : m_iPopSize(popsize),
    		m_dMutationRate(MutRat),
    		m_dCrossoverRate(CrossRat),
    		m_iChromoLength(numweights),
    		m_dTotalFitness(0),
    		m_cGeneration(0),
    		m_dBestFitness(0),
    		m_dWorstFitness(99999999),
    		m_dAverageFitness(0)
    	{
    		//initialise population with chromosomes consisting of random
    		//weights and all fitnesses set to zero
			m_vecPop.resize(m_iPopSize);
			vecNewPop.resize(m_iPopSize);
    	}
    
    
    	//---------------------------------Mutate--------------------------------
    	//
    	//	mutates a chromosome by perturbing its weights by an amount not 
    	//	greater than CParams::dMaxPerturbation
    	//-----------------------------------------------------------------------
    	void CGenAlg::Mutate(Genome &chromo)
    	{
    		//traverse the chromosome and mutate each weight dependent
    		//on the mutation rate

			for (auto& g : chromo.Genes)
			{

				if (RandFloat() < m_dMutationRate)
				{
					for (auto& w : g.weight)
					{
						w += (RandomClamped() * dMaxPerturbation);

						w = min(1.0, max(0.0, w));
					}
				}
			}
    	}
    
    	//----------------------------------GetChromoRoulette()------------------
    	//
    	//	returns a chromo based on roulette wheel sampling
    	//
    	//-----------------------------------------------------------------------
    	Genome& CGenAlg::GetChromoRoulette()
    	{
    		//generate a random number between 0 & total fitness count
    		double Slice = (double)(RandFloat() * m_dTotalFitness);
    
    		//this will be set to the chosen chromosome
			Genome* TheChosenOne = 0;
    
    		//go through the chromosones adding up the fitness so far
			double FitnessSoFar = 0.0;
			for(auto& g : m_vecPop)
    		{
				FitnessSoFar += g.fitness;
    
    			//if the fitness so far > random number return the chromo at 
    			//this point
    			if (FitnessSoFar >= Slice)
    			{
    				TheChosenOne = &g;
    
    				break;
    			}
    
    		}
    
    		return *TheChosenOne;
    	}
    
    	//-------------------------------------Crossover()-----------------------
    	//	
    	//  given parents and storage for the offspring this method performs
    	//	crossover according to the GAs crossover rate
    	//-----------------------------------------------------------------------
    	void CGenAlg::Crossover(const Genome &mum,
			const Genome &dad,
			Genome       &baby1,
			Genome       &baby2)
    	{
    		//just return parents as offspring dependent on the rate
    		//or if parents are the same
    		if ((RandFloat() > m_dCrossoverRate) )
    		{
    			baby1 = mum;
    			baby2 = dad;
    
    			return;
    		}
    
    		//determine a crossover point
    		int cp = RandInt(0, m_iChromoLength - 1);
    
    		//create the offspring

			auto b1 = baby1.Genes.begin();
			auto b2 = baby1.Genes.begin();
			auto m = mum.Genes.begin();
			auto d = dad.Genes.begin();
    		for (int i = 0; i<cp; ++i)
    		{
				*b1++ = *m++;
				*b2++ = *d++;
    		}
    
    		for (int i = cp; i<m_iChromoLength - 1; ++i)
    		{
				*b2++ = *m++;
				*b1++ = *d++;
    		}
    
    
    		return;
    	}
    
    	//-----------------------------------Epoch()-----------------------------
    	//
    	//	takes a population of chromosones and runs the algorithm through one
    	//	 cycle.
    	//	Returns a new population of chromosones.
    	//
    	//-----------------------------------------------------------------------
    	void CGenAlg::Epoch()
    	{
    		//assign the given population to the classes population
    		
    		//reset the appropriate variables
    		Reset();
    
    		//sort the population (for scaling and elitism)
			sort(m_vecPop.begin(), m_vecPop.end(), [](Genome& a, Genome& b) {
				return b.fitness < a.fitness;
			});

	
    		//calculate best, worst, average and total fitness
    		CalculateBestWorstAvTot();
    
    		//create a temporary vector to store new chromosones
		
    
    		//Now to add a little elitism we shall add in some copies of the
    		//fittest genomes. Make sure we add an EVEN number or the roulette
    		//wheel sampling will crash
    		if (!(iNumCopiesElite * iNumElite % 2))
    		{
    			GrabNBest(iNumElite, iNumCopiesElite, vecNewPop);
    		}
    
    
    		//now we enter the GA loop
    
    		//repeat until a new population is generated
    		for(auto nG = vecNewPop.begin() + iNumCopiesElite; nG!= vecNewPop.end()- vecNewPop.size() / 2; nG+=2)
    		{
    			//grab two chromosones
    			Genome& mum = GetChromoRoulette();
    			Genome& dad = GetChromoRoulette();
    
    			//create some offspring via crossover
    			Crossover(mum, dad, *nG, *(nG  + 1));
    
    			//now we mutate
    			Mutate(*nG);
    			Mutate(*(nG + 1));
				(*(nG)).fitness = 0;
				(*(nG + 1)).fitness = 0;
    			//now copy into vecNewPop population
    		}


			for (auto nG = vecNewPop.end() - vecNewPop.size() / 2; nG != vecNewPop.end(); ++nG )
			{
				*nG = Genome();
			}

    
    		//finished so assign new pop back into m_vecPop
    		m_vecPop = vecNewPop;
   
    	}
    
    
    	//-------------------------GrabNBest----------------------------------
    	//
    	//	This works like an advanced form of elitism by inserting NumCopies
    	//  copies of the NBest most fittest genomes into a population vector
    	//--------------------------------------------------------------------
    	void CGenAlg::GrabNBest(int	            NBest,
    		const int	      NumCopies,
			vector<Genome>	&Pop)
    	{
    		//add the required amount of copies of the n most fittest 
    		//to the supplied vector
			auto g = m_vecPop.begin();
			auto nG = m_vecPop.begin();
    		while (NBest--)
    		{
    			for (int i = 0; i<NumCopies; ++i)
    			{
					*nG ++ = *g++;
    			}
    		}
    	}
    
    	//-----------------------CalculateBestWorstAvTot-----------------------	
    	//
    	//	calculates the fittest and weakest genome and the average/total 
    	//	fitness scores
    	//---------------------------------------------------------------------
    	void CGenAlg::CalculateBestWorstAvTot()
    	{
    		m_dTotalFitness = 0;
    
    		double HighestSoFar = 0;
    		double LowestSoFar = 9999999;
			for(auto& g: m_vecPop)
    		{
    			//update fittest if necessary
    			if (g.fitness> HighestSoFar)
    			{
    				HighestSoFar = g.fitness;
    
    				m_dBestFitness = HighestSoFar;
    			}
    
    			//update worst if necessary
    			if (g.fitness < LowestSoFar)
    			{
    				LowestSoFar = g.fitness;
    
    				m_dWorstFitness = LowestSoFar;
    			}
    
    			m_dTotalFitness += g.fitness;
    
    
    		}//next chromo
    
    		m_dAverageFitness = m_dTotalFitness / m_iPopSize;
    	}
    
    	//-------------------------Reset()------------------------------
    	//
    	//	resets all the relevant variables ready for a new generation
    	//--------------------------------------------------------------
    	void CGenAlg::Reset()
    	{
    		m_dTotalFitness = 0;
    		m_dBestFitness = 0;
    		m_dWorstFitness = 9999999;
    		m_dAverageFitness = 0;
    	}
    
    
