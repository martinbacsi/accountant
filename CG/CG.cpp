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
#include <set>

#define _USE_MATH_DEFINES 
#include <math.h>

#define DUMP

static double dCrossoverRate = 0.7;
static double dMutationRate = 0.4;
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

	friend ostream& operator<< (ostream &out, const Vec2D& p)
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
	set<int> alive;


	Field() : enemyHP(0), shot(0)
	{
#ifndef _MSC_VER

#endif // DEBUG
	}

	double FinalPoint()
	{
		int enemyDead = count_if(enemies.begin(), enemies.end(), [](const Enemy& e) {return e.dead; });
		return  (double)(datas.size() * 100 + datas.size() * max(0, enemyHP - 3 * shot) * 3 + enemyDead * 10);
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
			cerr << p[i] << " ";
		}



		for (auto&e : enemies)
		{
			enemyHP += e.life;
			alive.insert(e.id);
		}

		cerr << "\nEND_DUMP\n";
#endif

	}




	pair<int, double> bestEnemToShoot() const
	{
		auto best = max_element( alive.begin(), alive.end() ,[&](int a, int b) {
			int dmgA = dmgAtDistance(Distance( enemies[a].pos, player.pos));
			int dmgB = dmgAtDistance(Distance(enemies[b].pos, player.pos));

			int maxDmgA = min(14, enemies[a].life);
			int maxDmgB = min(14, enemies[b].life);
			
			return  (double)dmgA / maxDmgA   < (double)dmgB / maxDmgB;
		});

		int dmg = dmgAtDistance(Distance(enemies[*best].pos, player.pos));
		//cerr << "best enemy to shoot: " << (double)(dmgAtDistance(Distance(enemies[*best].pos, player.pos)) / enemies[*best].life) << endl;
		int maxDmg = min(14, enemies[*best].life);
		return	make_pair(*best, (double)dmg / maxDmg);
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

			int dist = Distance(player.pos, e.pos);
			if ( dist < 4000000)
			{
				return true;
			}
		}

		if (nextStep.type == SHOOT)
		{
			//cerr << nextStep.enemy << endl;
			//int id = (alive.begin() + (rand() % alive.size()))					aliveEnemies[round((1.0 - nextStep.enemy) * n)]->id;


			Enemy& e = enemies[bestEnemToShoot().first];



			player.Kill(e, Distance(player.pos, (e.pos)));



			//cerr << enemies.at(nextStep.enemy).life << endl;
			if (e.life < 1)
			{
				alive.erase(e.id);
				e.dead = true;
			}

			if (nextStep.enemy == 0)
			{
				int i = 0;;
				i++;
			}

			if (print)
			{
				cout << "SHOOT " << e.id << endl;
			}
			shot++;
		}


		int enemiesRemoved = 0;

		for (auto& e : enemies)
		{
			if (!e.dead && enemiesRemoved < 2)
			{
				for (int i = 0; i < datas.size(); ++i)
				{
					if (datas[i].pos == e.pos)
					{
						++enemiesRemoved;
						datas.erase(datas.begin() + i);
						break;
					}

				}
			}

		}

		if (!datas.size() && alive.size() == enemies.size())
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

		return allDead || !datas.size();
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
	field.player = ReadBinary<Player>("15999 3500 0 32767 0 32767 6450304 0")[0];
	field.enemies = ReadBinary<Enemy>("4000 4500 50 0 0 0 131953 0 0 0 0 0 0 0 0 0 0 0 0 0");
	field.datas = ReadBinary<Data>("0 5236 8304 1 2763 8304 2 763 6851 3 0 4500 4 763 2148 5 2763 695 6 5236 695 0 33 0 4000 4500 50 0 0 0 131953 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

	for (auto&e : field.enemies)
	{
		field.enemyHP += e.life;
		field.alive.insert(e.id);
	}

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


	int actualGene = 0;


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

		auto best = field.bestEnemToShoot();
		double maxDmg = (double)min(field.enemies[best.first].life, 14);
		double expP = exp2(best.second) - 1.1;
		double p1 = floor((double)(expP *  maxDmg));



		double p = (double)p1 / maxDmg;
		if (gene.weight[0] < p) 
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

		Vec2D firstPos;

		while (!dead && !field.Ended())
		{
			Step step = GeneToMove(*actual, field);
			dead = field.Step(step);
			if (first)
			{
				firstStep = step;
				firstPos = field.player.pos;
				first = false;
			}
			++actual;
		}

		double point = 0;
		if (!dead)
		{
			point = field.FinalPoint() * 100.0;
			if (point < 1.0)
			{
				auto closest = min_element(oField.enemies.begin(), oField.enemies.end(), [&](const Enemy& a, const Enemy& b) {
					return (Distance(a.pos, firstPos) < Distance(b.pos, firstPos));
				});


				point = (double) (16000* 16000 + 9000* 9000) /    Distance(closest->pos, firstPos);
				cerr << "point: " << point << endl;
			}
		

		}
		return point;
	}



	void NextGene()
	{
		Step first;
		auto& g = gA->m_vecPop[actualGene];
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

		++actualGene;
		if (actualGene == iPopulation)
		{
			gA->Epoch();
			actualGene = 0;
		}
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
		while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() < 99)
		{
			++r;
			cont.NextGene();
		}
		rounds++;

		field.Step(cont.GetBest(), true);
		cont.gA->DropFirst();
		if (cont.bestFitness > bestFitness)
		{
			bestFitness = cont.bestFitness;
		}


		for (auto& f : field.enemies)
		{
			cerr << "e hp: " << f.life << endl;
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
	for (auto& g : m_vecPop)
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
	if ((RandFloat() > m_dCrossoverRate))
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
	for (auto nG = vecNewPop.begin() + iNumCopiesElite; nG != vecNewPop.end() - vecNewPop.size() / 2; nG += 2)
	{
		//grab two chromosones
		Genome& mum = GetChromoRoulette();
		Genome& dad = GetChromoRoulette();

		//create some offspring via crossover
		Crossover(mum, dad, *nG, *(nG + 1));

		//now we mutate
		Mutate(*nG);
		Mutate(*(nG + 1));
		(*(nG)).fitness = 0;
		(*(nG + 1)).fitness = 0;
		//now copy into vecNewPop population
	}


	for (auto nG = vecNewPop.end() - vecNewPop.size() / 2; nG != vecNewPop.end(); ++nG)
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
			*nG++ = *g++;
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
	for (auto& g : m_vecPop)
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

