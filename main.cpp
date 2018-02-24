// PATRICK LIESVELD
#include <iostream>
#include <string>
#include <deque>
#include <hash_map.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;
class board;
void bomb(char *why);


typedef struct __action action;
typedef struct __actionPi actionPi;
typedef struct __actionValue actionValue;
typedef struct __episode episode;


struct eqstr
{
	bool operator()(const string &a1, const string &a2) const
	{
		return a1 == a2;
	}
};




struct __action
{
/*
	__action() { box = 0; memset(dir,'.',4); dir[4] = '\0'; };
	__action(const struct __action &cpy) { box = cpy.box; strcpy(dir,cpy.dir); };
	__action& __action::operator=(const struct __action &p);
*/	
	
	unsigned short box;
	char dir[5];
};
/*
__action& __action::operator=(const struct __action &p)
{
	if(this != &p)
	{
		box = p.box;
		strcpy(dir,p.dir);
	}
	return *this;
}
*/
struct __actionPi
{
/*
	__actionPi() {} ;
	__actionPi(const struct __actionPi &cpy) { a = cpy.a; prob = cpy.prob; a_rew = cpy.a_rew; };
	actionPi& __actionPi::operator=(const __actionPi &p);
*/

	deque<action> a; //available actions
	deque<double> prob; //probability of selecting action
//	deque<double> a_rew; 
};

/*
__actionPi& __actionPi::operator=(const struct __actionPi &p)
{
	if(this != &p)
	{
		a = p.a;
		prob = p.prob;
		a_rew = p.a_rew;
	}
	return *this;
}
*/

//Expected value function of selecting given action
struct __actionValue
{
/*
	__actionValue(const struct __actionValue &cpy) { n = cpy.n; rew = cpy.rew; };
	__actionValue() { n = 0; rew.clear(); };
	*/
	deque<double> rew;
	deque<int> n;
};

struct __episode
{

	__episode() { a = 0; r = 0.0000; state = 0; };
	__episode(const struct __episode &cpy) { state = cpy.state; a = cpy.a; r = cpy.r; };
	__episode& __episode::operator=(const struct __episode &p);

	unsigned long state;
	int a; //action indexed by state
	double r; //reward from taking action a in state state - oppunitions action from state'
};

__episode& __episode::operator=(const struct __episode &p)
{
	if(this != &p)
	{
		state = p.state;
		a = p.a;
		r = p.r;
	}
	return *this;
}


class box
{
	public:
		box();
		box(box *attchLeft = NULL, box *attchTop = NULL);
		box(const box &cpy);
		bool isFull();
		void clear();
		bool isAlmostFull();
		bool is2Fulled();

		friend ostream &operator<<(ostream &stream, board &arg);
		friend class board;


		~box();
	private:
		char ownd;
		int *west, *north, *east, *south;
};

box::box()
{
	west = NULL;
	north = NULL;
	east = NULL;
	south = NULL;
	ownd = ' ';
}

box::box(const box &cpy)
{
	if(cpy.west)
	{
		west = new int;
		*west = *cpy.west;
	}	else
		west = NULL;
		
	if(cpy.east)
	{
		east = new int;
		*east = *cpy.east;
	}	else
		east = NULL;
		
	if(cpy.north)
	{
		north = new int;
		*north = *cpy.north;
	}	else
		north = NULL;
		
	if(cpy.south)
	{
		south = new int;
		*south = *cpy.south;
	}	else
		south = NULL;
	ownd = cpy.ownd;
}

box::box(box *attchLeft, box *attchTop)
{
	ownd = ' ';
	if(attchLeft == NULL)
	{
		west = new int;
		*west = 0;
	}
	else
		west = attchLeft->east;
		
	if(attchTop == NULL)
	{
		north = new int;
		*north = 0;
	}
	else
		north = attchTop->south;
	
	south = new int;
	*south = 0;
	east = new int;
	*east = 0;
}

void box::clear()
{
	if(west)
		*west = 0;
	if(east)
		*east = 0;
	if(north)
		*north = 0;
	if(south)
		*south = 0;
		
		ownd = ' ';
}

box::~box()
{
	if(west)
		delete west;
	if(north)
		delete north;
	if(east)
		delete east;
	if(south)
		delete south;
}


bool box::isFull()
{
	if(north == NULL || south == NULL || east == NULL || west == NULL)
		bomb("invalid box");
		
	if(*north == 1 && *south == 1 && *east == 1 && *west == 1)
		return true;
	return false;
}

bool box::isAlmostFull()
{
	int ret = 0;
	if(north == NULL || south == NULL || east == NULL || west == NULL)
		bomb("invalid box");
		
		
	ret += *north + *south + *east + *west;
	if(ret == 3)
		return true;
	return false;
}

bool box::is2Fulled()
{
	int ret = 0;
	if(north == NULL || south == NULL || east == NULL || west == NULL)
		bomb("invalid box");
		
		
	ret += *north + *south + *east + *west;
	if(ret == 2)
		return true;
	return false;

}


class board
{
	public:
		board(int x, int y);
		board(const board &cpy);
		bool gameOver();


		int makeMove(char who);
		int makeChain(char who);

		int makeMove(char who, action &a);

		void getAvailableMoves(deque<action> &completeable_box, deque<action> &unadvised_moves, deque<action> &others);
		void clearBoard();
		int Count(char player);
		unsigned long boardHash();
		void boardHashStr(char *buf);
		~board();
		friend ostream &operator<<(ostream &stream, board &arg);
	private:
		int m,n;
		
		
		box **table;
	
	
	// 2 ^ (2mn - m - n) states
	
	// m - 1 boxes each row
	// n - 1 boxes each col
};

int board::Count(char player)
{
	int k;
	int ret = 0;
	
	for(k = 0;k <m*n;k++)
		if(table[k]->ownd == player)
			ret++;
			
	return ret;
}

board::board(int x, int y)
{
	int i,j,k;
	box *prev_box = NULL;
	box *prev_row = NULL;

	m = x-1;
	n = y-1;
	
	table = new box*[(k=((x-1)*(y-1)))];
	
	for(i=0;i<k;i++)
		table[i] = NULL;
		
	cout << 'k' << k << endl;

	for(i = 0;i < m;i++)
	{
		prev_box = NULL;
		for(j = 0;j < n;j++)
		{
			k = j + i*m;
			if(i == 0)
				prev_row = NULL;
			else
				prev_row = table[(j + (i-1)*m)];

			table[k] = new box(prev_box,prev_row);
			cout << 'k' << k << endl;
			prev_box = table[k];
		}
	}
}

//struct action;

//probability of selecting given action

typedef hash_map<unsigned long ,actionValue *, hash<unsigned long> > hash_value;
typedef	hash_map<unsigned long ,actionPi *, hash<unsigned long> > hash_pi;


class learningAgent
{
	public:
		learningAgent();
		bool newState(unsigned long state);
		bool addState(unsigned long state);
		bool addState(board &state);
		int TD(unsigned long state1, int a, double reward, unsigned long state2);

	// bool evaluate(episode) // updates Q values

		int makeMove(board &state, int &action);
		
		hash_pi::iterator findPi(unsigned long hash);
//		hash_pi::iterator findPi(string hash) { return findPi(hash.c_str()); };

		hash_value::iterator findValue(unsigned long hash);
	//	hash_value::iterator findValue(string hash) { return findValue(hash.c_str()); };

		//policy evaluation, learn from episode
		int policyEvaluation(deque<episode> &trial);
		int policyImprovement(deque<episode> &trial);
		
		int bestAction(unsigned long state2);
//		learnFrom(deque<sarsa> episode) 
		
		
		double Reward;
		int reward;
		
		//policy improvement.. update pi based on argmax Q(s,a);
	private:
	
		hash_value Q; //  Q(s,a) state, action pair
		hash_pi pi;
		
		deque<unsigned long> learned_states; //used to determine weither to alloc the state, or use hashed state
		
		// pi = mapping from states to actions
		// Q(s,a)
};


int learningAgent::bestAction(unsigned long state2)
{
	int i;
	int a = -1;
	double max = -99.0000;
	
//	if(newState(state2))
//		addState(state2);
		
	hash_value::iterator i_value = findValue(state2);
	
	actionValue *p = i_value->second;
	
	for(i = 0;i < p->rew.size();i++)
	{
		if(max < p->rew[i])
			max = p->rew[i];
	}
	
	for(i = 0;i < p->rew.size();i++)
	{
		if(max == p->rew[i])
		{
			a = i;
			return a;
		}
	}
	
	
	return a;
	
}

learningAgent::learningAgent()
{
}

int learningAgent::TD(unsigned long state1, int a, double reward, unsigned long state2)
{
	double alpha = 0.300;
	double gamma = 0.8500;
	int a2;
	
	a2 = bestAction(state2);
	
	hash_value::iterator i_value = findValue(state1);
	hash_value::iterator i_value2 = findValue(state2);
	
	
	
	
	i_value->second->rew[a] = i_value->second->rew[a] + alpha*((reward + gamma*i_value2->second->rew[a2]) - i_value->second->rew[a]);
	
}


bool learningAgent::newState(unsigned long state)
{
	deque<unsigned long>::iterator i;
	
	for(i = learned_states.begin();i != learned_states.end();*i++)
	{
		if(state == *i) //checks if the state passed has been hashed
			return false;
	}
	
	return true;
}

bool learningAgent::addState(unsigned long state) //add state to learned 
{
	unsigned long new_state;
	if(!newState(state))
	{
		bomb("wtf");
		return false;
	}
	
	new_state = state;
	
	learned_states.push_back(new_state);
	return true;
}

hash_pi::iterator learningAgent::findPi(unsigned long hash)
{
	//hash_map<const char *,struct actionPi *,hash<const char *>,eqstr>::iterator i;

	if(newState(hash))
	{
		cout << "new state?\n";
		addState(hash);
	}


	return pi.find(hash);
}

hash_value::iterator learningAgent::findValue(unsigned long hash)
{
	//hash_map<const char *,struct actionPi *,hash<const char *>,eqstr>::iterator i;

	if(newState(hash))
		bomb("shouldn't be finding it");

	return Q.find(hash);

}


bool learningAgent::addState(board &state)
{
	unsigned long buf = 0;
	hash_pi::iterator policy;
	deque<action> abox;
	deque<action> unadvised;
	deque<action> others;
	pair< hash_pi::iterator, bool> p;
	pair< hash_value::iterator, bool> p2;

	deque<action>::iterator i;

	struct __actionPi *new_pi = NULL; //new policy
	struct __actionValue *new_rew = NULL; // new reward
	
//	bzero(buf,512);
	buf = state.boardHash();

	if(!newState(buf))
		bomb("oh god why\n");
		
	addState(buf);


	
//	Q[buf] = new_rew; //updated after generation of an episode
//	pi[buf] = new_pi;

/*
		
	if(new_rew != NULL || new_pi != NULL)
	{
		bomb("omg...");
	}
	*/
	new_pi = new struct __actionPi;
	new_rew = new struct __actionValue;
	

	
	
	if(new_rew == NULL || new_pi == NULL)
	{
		bomb("omg!!!");
	}
	
	
	state.getAvailableMoves(abox,unadvised,others);
//new_pi->a
	
	double n_moves = abox.size() + unadvised.size() + others.size();
	
	
	
	while(!abox.empty() || !unadvised.empty() || !others.empty())
	{// for each available action, distribute the probability among available actions uniformly
		action D;
		double rand_act = 1.0000/double(n_moves);
		if(!abox.empty())
		{
			D = abox.front();
			abox.pop_front(); // wait probability more here
			rand_act += 1.0000/double(n_moves);
		}
		else if(!unadvised.empty())
		{
			D = unadvised.front();
			unadvised.pop_front(); //wait probability less here
			rand_act += 1.0000/double(n_moves);
		}
		else
		{
			D = others.front();
			others.pop_front(); // leave probability the rest here.
		//	rand_act += 1.4000/double(n_moves);
		}
		
		new_pi->a.push_back(D);
		new_pi->prob.push_back(rand_act);
		new_rew->rew.push_back(0.0000);
		new_rew->n.push_back(0);
	}	
	

	p = pi.insert( pair<unsigned long, actionPi *>(buf,new_pi) );
	
	if(!p.second)
//		cout << "Q insertion successful hash[" << buf << "] ptr:"  << endl;
		bomb("first insertion not successful\n");
	
	
	p2 = Q.insert( pair<unsigned long, actionValue *>(buf,new_rew) );
	
	if(!p2.second)
		bomb("Q insertion failed hash\n");
	
/*
	pi[buf] = new_pi;
	Q[buf] = new_rew;
*/	
	
	/*
	new_pi = findPi(buf);
	new_rew = findValue(buf);
	
	cout << "Q[" << buf << "]: " << new_rew << endl;
	cout << "pi["<< buf <<  "]: " << new_pi << endl;
*/


	

	policy = pi.find(buf); //findPi(buf);
	if(policy == pi.end())
		cout << "pi[" << buf << "]: is bad\n "<< endl;
//	else
//		cout << "pi[" << buf << "]: " << (*policy).second << endl;
	
	
	return p.second && p2.second;
	// state.get available actions
	// Q(
}


int learningAgent::makeMove(board &state, int &a)
{
//	char buf[512];
	unsigned long buf;
	int j = 0, r = 0, r2 = 0;
	double total = 0, retotal = 0;
	hash_pi::iterator policy;
	hash_value::iterator value;
	
	deque<struct __action>::iterator i;
	
//	bzero(buf,512); 
	buf = state.boardHash();
	
	//cout << "looking up state [" << buf << "]\n";
	if(newState(buf))
		addState(state);


	policy = pi.find(buf); // findPi(buf);
	value = findValue(buf);
	
	if(policy == pi.end() )
	{
		cout << "error in state " << buf << endl;
		bomb("couldn't lookup hashed value");
	}
	
	
	if((*policy).second == NULL)
		bomb("null?");

	//cout << "pi["<< buf << "] " << policy->first << " addr " << (*policy).second << endl;
	
	
	for(i = policy->second->a.begin(); i != policy->second->a.end();*i++)
	{
	//	cout << "j" << j << ' '<< policy->second->prob[j] << endl;
	//	cout << "box: (*i).box" << (*i).box << " actions: " << (*i).dir << endl;

		total += policy->second->prob[j];
		j++;
	}
	

	retotal = total;
	j = 0;
	for(i = policy->second->a.begin(); i != policy->second->a.end();*i++)
	{
		if(policy->second->a.size() == 0)
		{
			bomb("oops");
		}
		else if(policy->second->a.size() == 1)
		{
			a = j;
			return state.makeMove('A',*i);
		}	

	//	cout << "new j" << j << ' ' << pi[buf]->prob[j] << endl;
	//	cout << "new box: " << (*i).box << " actions: " << (*i).dir << endl;
	   /// cout << (double) (random()%((int)(total*1000.000))) << " <left right> " << (policy->second->prob[j])*1000.000  << endl;

		if( (double) (random()%((int)(total*100.000))) <= (policy->second->prob[j])*100.000 )
		{
			a = j;
//			cout << "Agent["<< buf <<"] marks " << (*i).box << " in " << (*i).dir << " action " << a << endl;
			
			return state.makeMove('A',*i);
//			return r2;
		} else {
			total -= policy->second->prob[j];
		}

		j++;
	}
	
	bomb("shouldn't happened!\n");
	

	return 0;
	
}


int learningAgent::policyEvaluation(deque<episode> &trial)
{

	unsigned long hash;
	hash_pi::iterator policy;
	hash_value::iterator value;
	hash_value::iterator value_next;

	int a, tot = 0, idx = 0,j;
	double r = 0.0000;
	double gamma = 0.8400;
	double discount = 0.0000;
	
	actionPi *update_policy = NULL;
	actionValue *update_value = NULL;

	deque<episode>::iterator i;
	deque<episode>::iterator i_next;

	
	
//	cout << "Episode: ";
	for(i = trial.begin();i != trial.end();i++)
	{
		hash = (*i).state;
		//cout << " in state " << hash;
		//cout << " took action " <<
		 (a = (*i).a);
		//cout << " for reward " << 
		(r = double((*i).r));// << endl;
		
		
		policy = findPi(hash);
		value = findValue(hash);
		
		if(policy == pi.end() || value == Q.end())
		{
			cout << "searching for [" << hash<< "]\n";
			bomb("couldn't find policy");
		}
		
		r = 0.0000;
		idx = 0;
		
		for(i_next = i; i_next != trial.end(); *i_next++)
		{	
			discount = gamma;
			//value_next = findValue( (*i_next).state );
			if(idx == 0)
				r = double((*i_next).r) + 0.0000;
			else
			{
				for(j = 1;j < idx;j++)
					discount *= gamma;
				
				r += double((*i_next).r)*(discount);
			}
			idx++;
		}
		
	//	policy->second;
		//cout << "policy: " << hash << " goes to " << policy->second << endl;
		update_policy = policy->second;
		update_value = value->second;
		
		
		tot = update_value->n[a];
		//cout << "r: " << r << endl;
		
	//	cout << "previous reward: " << update_value->rew[a];
		update_value->rew[a] =  ((double(tot)*update_value->rew[a]+0.0000) + r)/double(tot+1.0000);
		update_value->n[a] = tot+1;
	//	cout << " new reward: " << update_value->rew[a] << endl;		
		
		
		
//		Q[hash.c_str()].rew[(*i).a] = (*i).r;
		
	}
	
	return 0;
}


// Set pi[] to be an e-soft policy
int learningAgent::policyImprovement(deque<episode> &trial)
{
	unsigned long hash;
	hash_pi::iterator policy;
	hash_value::iterator value;
	hash_value::iterator value_next;

	int a, tot = 0, idx = 0,j;
	double r = 0.0000;
	double max = -9999.00000;
	double total = 0.0000;
	double gamma = 0.9000;
	double discount = 0.0000;
	
	actionPi *update_policy = NULL;
	actionValue *update_value = NULL;

	deque<episode>::iterator i;
	deque<episode>::iterator i_next;

	
	
	//cout << "Episode: ";
	for(i = trial.begin();i != trial.end();i++)
	{
		hash = (*i).state;
		//cout << " in state " << hash;
		//cout << " took action " << 
		(a = (*i).a);
//		cout << " for reward " << 
		(r = (*i).r);// << endl;
		
		
		policy = findPi(hash);
		value = findValue(hash);
		
		if(policy == pi.end() || value == Q.end())
		{
			cout << "searching for [" << hash<< "]\n";
			bomb("couldn't find policy");
		}

		update_policy = policy->second;
		update_value = value->second;
		
		for(j = 0;j < update_policy->a.size();j++)
		{
			if(update_value->rew[j] > max)
				max = update_value->rew[j];
		}
		
		idx = 0;
		for(j = 0;j < update_policy->a.size();j++)
		{
//			cout << "rew: " << update_value->rew[j];
			if(update_value->rew[j] == max)
				idx++;
		}
		

		for(j = 0;j < update_policy->a.size();j++)
		{
//			cout << "idx" << idx << endl;
			if(update_value->rew[j] == max)
			{
				update_policy->prob[j] = 0.9000/double(idx + 0.0000);
			} else {
				update_policy->prob[j] = 0.1000/double(update_policy->a.size());
			}
		}
		
		
		total = 0.0000;
		for(j = 0;j < update_policy->a.size();j++)
		{
			total += double(update_policy->prob[j]);
		}
		
		for(j = 0;j < update_policy->a.size();j++)
		{
			update_policy->prob[j] = double(update_policy->prob[j])/(total + 0.0000);
		}
		

		
		

		for(j = 0;j < update_policy->a.size();j++)
		{
//			cout << "new policy[" << hash << "][" << j << "] " << update_policy->prob[j] << " for Q(s,a): " << update_value->rew[j] << " n" << update_value->n[j] << endl;
			
		}
				
		

	}
	
	
	return 0;
}



ostream& operator << (ostream& os, board& arg)
{
	int i,j,k;
	box ** table = arg.table;
	string row1;
	string row2;
	string row3;
	string row4;
	string row5;
	
	char *empty =  " ......";
	char *filled = " ======";
	
	for(i = 0;i < arg.m;i++)
	{
		row1 = "";
		row2 = "";
		row3 = "";
		row4 = "";
		row5 = "";
		
		for(j = 0;j < arg.n;j++)
		{
			k = j + i*(arg.m);
		
			
			if(*table[k]->west == 0)
			{
				row2 = row2 + '.';
				row3 = row3 + '.';
				row4 = row4 + '.';
			}
			else
			{
				row2 = row2 + '|';
				row3 = row3 + '|';
				row4 = row4 + '|';
			}
				
			row2 += "      ";
			if(table[k]->isFull())
			{
				row3 += "  ";
				row3 += table[k]->ownd;
				row3 += "   ";
			}
			else
				row3 += "      ";
			row4 += "      ";
				
			if(*table[k]->north == 0)
				row1 = row1 + empty;
			else
				row1 = row1 + filled;
				
				
			if(i+1 >= arg.m)
			{
			
				if(*table[k]->south == 0)
					row5 += empty;
				else
					row5 += filled;
			}
			
		}

		if(*table[k]->east == 0)
		{
			row2 += '.';
			row3 += '.';
			row4 += '.';

		} else {

			row2 += '|';
			row3 += '|';
			row4 += '|';

		}


		os << row1 << endl;
		os << row2 << endl;
		os << row3 << endl;
		os << row4 << endl;
		if(row5 != "")
			os << row5 << endl;
	}
	return os;
}

board::board(const board &cpy)
{
	m = cpy.m;
	n = cpy.n;

}

board::~board()
{

	
}

void board::clearBoard()
{
	int k;
	for(k = 0;k < n*m;k++)
	{
		table[k]->clear();
		table[k]->ownd = ' ';
	}
}



bool board::gameOver()
{
	int k;
	
	for(k = 0;k < n*m;k++)
	{
		if(!table[k]->isFull())
			return false;
	}
	return true;
}


void board::getAvailableMoves(deque<action> &completeable_box, deque<action> &unadvised_moves, deque<action> &others)
{
	int i,j,k,l;
	action A;
	deque<action> available;
	
	bzero(A.dir,5);

	
	for(i = 0;i < m;i++)
	{
		for(j = 0;j < n;j++)
		{
			char all_act[] = { 'W', 'N', 'E', 'S' , '\0' };
			k = j + i*m;
			if(table[k]->isFull())
				continue;
				
			memset(A.dir,'.',4);
				
				
			A.box = k;
			if(*table[k]->west == 0)
				A.dir[0] = 'W';
			else
				A.dir[0] = '.';
				
			if(*table[k]->north == 0)
				A.dir[1] = 'N';
			else
				A.dir[1] = '.';
				
				
				
			if(*table[k]->east == 0 && k%m == m - 1) //only count east for right column
				A.dir[2] = 'E';
			else
				A.dir[2] = '.';
				
			if(*table[k]->south == 0 && k/n == n - 1) //only count south for bottom row
				A.dir[3] = 'S';
			else
				A.dir[3] = '.';

			A.dir[4] = '\0';
	
			for(l = 0; l < 4;l++)
				if(all_act[l] == A.dir[l])
				{
					action B;
					memset(B.dir,'.',4);
					B.box = A.box;
					B.dir[4] = '\0';
					B.dir[l] = A.dir[l];
					
					if(table[k]->isAlmostFull())
						completeable_box.push_back(B);
					else if(table[k]->is2Fulled())
						unadvised_moves.push_back(B);
					else
						others.push_back(B);
				}
			

			
		}
	}


//	return others;
}


//fills in board position that is defined by action a
int board::makeMove(char who, action &a)
{
	int ret = 0,k;

	if(a.dir[0] == 'W') {
		*table[a.box]->west = 1;
	} else if(a.dir[1] == 'N') {
		*table[a.box]->north = 1;
	} else if(a.dir[2] == 'E') {
		*table[a.box]->east = 1;
	} else if(a.dir[3] == 'S') {
		*table[a.box]->south = 1;
	}


	for(k = 0;k < m*n;k++)
	{
		if(table[k]->isFull())
			if(table[k]->ownd == ' ')
			{
				table[k]->ownd = who;
				ret++;
			}
	}
	
	return ret;
}



//returns 1 if it completes a box
//returns 0 otherwise
int board::makeMove(char who)
{
	int i,j,k,ret = 0;
	action A;
	deque<action> available;
	deque<action> almost;
	
	for(i = 0;i < m;i++)
	{
		for(j = 0;j < n;j++)
		{
			k = j + i*m;
			if(table[k]->isFull())
				continue;
				
				
			A.box = k;
			if(*table[k]->west == 0)
				A.dir[0] = 'W';
			else
				A.dir[0] = '.';
			if(*table[k]->north == 0)
				A.dir[1] = 'N';
			else
				A.dir[1] = '.';
			if(*table[k]->east == 0)
				A.dir[2] = 'E';
			else
				A.dir[2] = '.';
			if(*table[k]->south == 0)
				A.dir[3] = 'S';
			else
				A.dir[3] = '.';

			A.dir[4] = '\0';
	
	
			available.push_back(A);
	
			
		}
	}


	
	//return 0 if no boxes could be completed
	
	k = available.size();

	if(k == 0)
		return ret;
		
//	i = random()%k; //random policy otherwise.
	
	
	
	for(;;)
	{
		i = random()%k;
		j = random()%4;
		if(available[i].dir[j] == '.')
			continue;
		
		if(available[i].dir[j] == 'W')
		{
			*table[available[i].box]->west = 1;
//			cout << "player " << who << " used box " << available[i].box << " west\n";
		}
		else if(available[i].dir[j] == 'N')
		{
			*table[available[i].box]->north = 1;
//			cout << "player " << who << " used box " << available[i].box << " north\n";
		}
		else if(available[i].dir[j] == 'E')
		{
			*table[available[i].box]->east = 1;
///			cout << "player " << who << " used box " << available[i].box << " east\n";
		}
		else if(available[i].dir[j] == 'S')
		{
			*table[available[i].box]->south = 1;
	///		cout << "player " << who << " used box " << available[i].box << " south\n";
		}
			
			
	
		
		return 0;
	}
		  
	/*
	
	
			if(*table[k]->east == 0)
			{
				*table[k]->east = 1;
				if(table[k]->isFull())
				{
					table[k]->ownd = who;
					return true;
				}
				return false;
			}

			if(*table[k]->west == 0)
			{
				*table[k]->west = 1;
				if(table[k]->isFull())
				{
					table[k]->ownd = who;
					return true;
				}
				return false;
			}
			
			if(*table[k]->north == 0)
			{
				*table[k]->north = 1;
				if(table[k]->isFull())
				{
					table[k]->ownd = who;
					return true;
				}
				return false;
			}
			
			if(*table[k]->south == 0)
			{
				*table[k]->south = 1;
				if(table[k]->isFull())
				{
					table[k]->ownd = who;
					return true;
				}
				return false;
			}
*/
			
	return 0;
	
}

//returns 2 box completion 
//returns 1 if it completes a box
//returns 0 otherwise
int board::makeChain(char who)
{
	int i,j,k,ret = 0;
	action A;
	deque<action> available;
	deque<action> almost;
	
	for(i = 0;i < m;i++)
	{
		for(j = 0;j < n;j++)
		{
			k = j + i*m;
			if(table[k]->isFull())
				continue;
				
				
			A.box = k;
			if(*table[k]->west == 0)
				A.dir[0] = 'W';
			else
				A.dir[0] = '.';
			if(*table[k]->north == 0)
				A.dir[1] = 'N';
			else
				A.dir[1] = '.';
			if(*table[k]->east == 0)
				A.dir[2] = 'E';
			else
				A.dir[2] = '.';
			if(*table[k]->south == 0)
				A.dir[3] = 'S';
			else
				A.dir[3] = '.';

			A.dir[4] = '\0';
	
	
	
		if(A.dir[0] == 'W') {
			*table[k]->west = 1;
		} else if(A.dir[1] == 'N') {
			*table[k]->north = 1;
		} else if(A.dir[2] == 'E') {
			*table[k]->east = 1;
		} else if(A.dir[3] == 'S') {
			*table[k]->south = 1;
		}
		
		for(i = 0; i < n*m;i++)
			if(table[i]->isFull())
				if(table[i]->ownd == ' ')
				{
					ret++;
					table[i]->ownd = who;
//					cerr << "player " << who << " completed box " << i << endl;
				}
	
		return ret;
	
		}
	}
	
	return ret;

}

unsigned long board::boardHash()
{
	int k;
	unsigned long ret = 0;
	
	for(k = 0; k < n*m;k++)
	{
		ret += *table[k]->west;
		ret <<= 1;
		ret += *table[k]->north;
		ret <<= 1;
		
		if(k%m == m-1)
		{
			ret += *table[k]->east;
			ret <<= 1;
		}
		
		if(k/n == n-1)
		{
			ret += *table[k]->south;
			if(k+1 < n*m)
				ret <<= 1;
		}

	}
	
	return ret;
}
//todo

void board::boardHashStr(char *str)
{
	int k,i = 0,N,M,j;
//	char buf[512];
	unsigned long long ret = 0;
	
	
	memset(str,'\0',512);
	
	for(k = 0; k < n*m;k++)
	{
		ret += *table[k]->west;
		ret <<= 1;
		ret += *table[k]->north;
		ret <<= 1;
		
		if(k%m == m-1)
		{
			ret += *table[k]->east;
			ret <<= 1;
		}
		
		if(k/n == n-1)
		{
			ret += *table[k]->south;
			if(k+1 < n*m)
				ret <<= 1;
		}

	}
	N = n+1;
	M = m+1;
	
	k = ((M*N*2)-M-N);
	
	
	sprintf(str,"%u",ret);
	//cout << "boardhash [" << str << "]\n";
	return; //thanks Ian... I wasn't thinking...
	
	unsigned long long *ptr;
	ptr = &ret;	
	

	cout << "k: " << k << " states " << endl;
	j = 0;
	for(i = 0;i < k;i+=4)
	{//this way is harder than I need to make it.....
		unsigned short firstbyte = 0;

		firstbyte = *ptr & 0x000000FF; //grab first byte
		for(j=0;j < 4;j++)
		{
			unsigned short halfbyte = 0;
			halfbyte = firstbyte & 0x0000000E; //grab first 4 bits
			
			cout << "halfbyte: " << halfbyte << endl;
			firstbyte >>= 4;
		}
		ptr += 1;
		
		//take the first bit 
		
	}
	//need to uniquely identify 2^k states
	
	
//	211111111
}




void bomb(char *why)
{
	fprintf(stderr,why);
	abort();
}





int main (int argc, char * const argv[]) {
    // insert code here...
	int m,n,k,i,r=0,r2 = 0,r3 = 0, r4 = 0,a=0, olda = 0, p = 0;
	struct timeval theTime;
	episode new_state;
	unsigned long buf;
	unsigned long AliceState;
	
	double our_box = 1.000;
	double their_box = 1.0000;
	
	deque<episode> trial;

	learningAgent Alice;

/*
	bzero(buf,512);
	bzero(AliceState,512);
	
    std::cout << "Hello, World!\n";
*/	gettimeofday(&theTime,NULL);
	
	
	srandom(theTime.tv_sec+theTime.tv_usec);


	if(argc < 2)
		bomb("arguments: <n> <m>\n");
		
		
	if((m = atoi(argv[1])) < 0)
	{
		bomb("bad argument");
	} else if((n = atoi(argv[2])) < 0) {
		bomb("bad argument");
	}
		
	std::cout << "m " << m << " n " << n << endl;
	board Initial(m,n);
	
	
	
	cout << Initial;
	cout << "hash: " << Initial.boardHash() << endl;

	

	char player;
	char prev_player;
	char player1 = 'a';
	char player2 = 'b';
	int playera_wins = 0;
	int playerb_wins = 0;
	int draw = 0;
	int new_a = 0;
	int policyIteration = 0;
	bool oldstate = false;
	bool printGame = false;
	player = player1;
	k = 0;
	
	r = 0;
	
	for(p = 0;p < 5000;p++)
	{
		playera_wins = 0;
		playerb_wins = 0;
		draw = 0;
		printGame = false;
//		usleep(300000);
		if(p%3333 == 0)
		{
			printGame = true;
		}
	
		for(i = 0;i < 100;i++)
		{
			k = 0;
			trial.clear();


			
			buf = Initial.boardHash();
			while(!Initial.gameOver()) //Generate an episode based on pi.....
			{
				k++;
	
				prev_player = player;
				
				
				if(printGame)
				{
					cout << "Board -- player " << player << endl;
					cout << Initial;
				}
				
				if(player == player2)
				{
					bool goTwice = false;

					r3 = 0;
					while((r2 = Initial.makeChain(player)) != 0) //Complete as many boxes as possible
					{
						if(r2 == 2) //wait thats counting two boxes that have been completed, but not neccessarly by one stroke
							goTwice = true;
							
						
						r3 += r2;
					} 
					
					if(printGame)
					{
						cout << "player " << player << " finished " << r3 << " boxes\n";
					}
					
					if(!Initial.gameOver() )
					{
						r = Initial.makeMove(player);
						buf = Initial.boardHash(); //new state s'
						if(goTwice)
						{ 
							goTwice = false;
							continue;
						}
				
						player = player1;
					}
					
					
					

					if(k > 1 && player == player1 && !Initial.gameOver())
					{
					//	cout << "1saving state " <<AliceState<< " using action " << a << " for a net reward " << r4 - r3 << endl;

						new_state.r = double(r4+ 0.0000)*our_box - double(r3 + 0.0000)*their_box;
						new_state.a = a;
						new_state.state = AliceState;
						
						
//						if(Alice.newState(buf))
//							Alice.addState(Initial);
//						new_a = Alice.TD(AliceState,a,new_state.r,buf);
						
						trial.push_back(new_state);
						
						r4 = 0;
						r3 = 0;
					}
					
				} else if(player == player1) {
					bool goTwice = false;
					r4 = 0.0000;
					while((r = Initial.makeChain(player)) != 0) //&& Alice.chainChance()) //Reward based on players move...
					{
					
						
						r4 += r;
						if(r == 2) // go twice?
						{
							goTwice = true;
						}
						
					}
					
					if(printGame)
							cout << "player " << player << " got a reward of " << r4 << endl;
							
					if( !Initial.gameOver() )
					{
					
						r = Alice.reward = 0;
						AliceState = Initial.boardHash();
						r += Alice.makeMove(Initial,a);
						
						if(goTwice)
						{
							Alice.reward += r4 + r; // action 
							goTwice = false;
							continue;
						}
					}
					
				
					if(r == 0)
						player = player2;
						
			
				}
				
//				if(((random()%100)/100) >= 0.250)
					//printGame = false;
			} // end of one game	
			
				
			if(k > 1 && prev_player == player2 && player == player2) //incomplete update because player2 completed last box
			{
			//	cout << "2saving state " << AliceState << " using action " << a << " for a net reward " << r4 - r3 << endl;
				new_state.state = AliceState;
				new_state.r = double(r4 + 0.0000)*our_box - double(r3 + 0.0000)*their_box;
				new_state.a = a;
				trial.push_back(new_state);
				r4 = 0;
				r3 = 0;		
			} else if(k > 1 && prev_player == player1 && player == player2)
			{
			//	cout << "3saving state " << AliceState << " using action " << a << " for a net reward " << r4 << endl;
				new_state.state = AliceState;
				new_state.r = double(r4 + 0.0000)*our_box - double(r3 + 0.0000)*their_box;
				new_state.a = a;
				trial.push_back(new_state);
				r4 = 0;
				r3 = 0;		
			}
			
		//	cout << "trial size" << trial.size() << endl;

		
		//	cout << "game" << i << " over after " << k << " turns\n";

			int c1 = Initial.Count('a') + Initial.Count('A');
			int c2 =  Initial.Count('b');
			
			if(c1 == c2)
				draw++;
			else if(c1 > c2)
				playera_wins++;
			else
				playerb_wins++;


			k = 0;
			r4 = 0;
			r3 = 0;
			r2 = 0;
			r = 0;
			player = player1;
			
		//	cout << Initial; //final state.
			Initial.clearBoard();
		} // end of 100 trials


			cout << "policy iteration " << policyIteration << endl;
			cout << "player a won " << playera_wins << " and player b won " << playerb_wins << " and there were " << draw << " draws.\n";
			policyIteration++;
		
		printGame = false;
		
		Alice.policyEvaluation(trial);
		Alice.policyImprovement(trial);		
			
	} // end of 1000 policy iterations
	
	

	
			
	
    return 0;

}
