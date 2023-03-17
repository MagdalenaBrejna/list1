#include <bits/stdc++.h>

using namespace std;
using ld = long double;
#define pb push_back

typedef wchar_t ucs4_t;

struct Edge {
	string line;
	int timeA;
	int timeB;
	string dest, start;
	int edgeId;
	Edge(string Line, int TA, int TB, string B, int EID=-1, string XD = "") {
		line = Line;
		timeA = TA;
		timeB = TB;
		dest = B;
		start = XD;
		edgeId = EID;
	}
	Edge() {}
	bool operator<(const Edge &o) const {
		if (timeA == o.timeA) return line < o.line;
		return timeA < o.timeA;
 	}
};

struct AStarElement {
	int cost;
	int time;
	string name;
	string line;
	AStarElement(int C, int T, string N, string L) {
		cost = C;
		time = T;
		name = N;
		line = L;
	}
	AStarElement() {}
	bool operator<(const AStarElement &o) const {
		if (cost == o.cost && time == o.time) return name < o.name;
		if (cost == o.cost) return time < o.time;
		return cost < o.cost;
	}
};

const int INF = INT_MAX-24*60*60;
string A, B;
string MODE;
int T;
string Time;
ld SLOWEST_PACE = INT_MAX;

map<string,set<Edge>> adj;
map<string,pair<ld, ld>> geo;
map<string,int> cost;
map<string,int> parent;
map<int,Edge> input;

stringstream str;
string::size_type sz;

ld getDistance(ld Ax, ld Ay, ld Bx, ld By) {
	// cout << Ax << " " << Ay << " " << Bx << " " << By << "\n";
	return sqrt((Bx-Ax)*(Bx-Ax) + (By-Ay)*(By-Ay));
}
ld getPace(ld dist, ld time) {
	return dist/time;
}

int getInt(string s) {
	return (s[0]-'0')*10+s[1]-'0';
}
ld getLD(string s) {
	ld res = 0, mult=1;
	bool wasDot = false;
	for (auto i : s) {
		if (i=='.') {
			wasDot = true;
			continue;
		}
		if (wasDot) {
			mult*=10;
			res += (i-'0')/mult;
		} else {
			res*=10,res+=i-'0';
		}
	}
	return res;
}

int getMyTime() {
	int res = 0;
	string tmp;
	getline(str,tmp,':');
	res += getInt(tmp)*60*60;
	getline(str,tmp,':');
	res += getInt(tmp)*60;
	getline(str,tmp,',');
	res += getInt(tmp);
	return res;
}

ld getMyLD() {
	ld res = .0;
	string tmp;
	getline(str,tmp,',');
	res = getLD(tmp);
	return res;
}

void read(string fname = "graph.csv") {
	//locale::global(locale("pl_PL.utf8"));
	//cout.imbue(locale("pl_PL.utf8"));
	ifstream file(fname);
	if (!file.is_open()) {
		cout << "error in reading file\n";
		return;
	}

	string line;

	getline(file,line);
	
	while (getline(file,line)) {
		str = stringstream(line);
		string word;

		string linia, stopA, stopB;
		int timeA, timeB, eId;
		ld Ax,Ay,Bx,By;

		getline(str,word,',');
		eId = getLD(word);
		getline(str,word,',');
		getline(str,word,',');
		getline(str,linia,',');
		timeA = getMyTime();
		timeB = getMyTime();
		getline(str,stopA,',');
		getline(str,stopB,',');
		Ax = getMyLD();
		Ay = getMyLD();
		Bx = getMyLD();
		By = getMyLD();
		adj[stopA].insert({linia,timeA,timeB,stopB,eId});
		geo[stopA] = {Ax,Ay};
		geo[stopB] = {Bx,By};
		cost[stopA] = INF;
		cost[stopB] = INF;
		input[eId] = {linia, timeA, timeB, stopB, eId, stopA};
		// cout << SLOWEST_PACE << " " << getDistance(Ax,Ay,Bx,By) << " " << getPace(getDistance(Ax,Ay,Bx,By), timeB-timeA) << "\n";
		SLOWEST_PACE = min(SLOWEST_PACE, getPace(getDistance(Ax,Ay,Bx,By), timeB-timeA));
	}
}

string getTimeFromS(int S) {
	int intH = S/60/60;
	int intM = S/60 - intH*60;
	int intS = S - intM*60 - intH*60*60;
	string h = (intH < 10 ? "0" : "")+to_string(intH);
	string m = (intM < 10 ? "0" : "")+to_string(intM);
	string s = (intS < 10 ? "0" : "")+to_string(intS);
	return h+":"+m+":"+s;
}

map<string,map<string,set<int>>> dojazd;
map<string,set<pair<int,int>,greater<pair<int,int>>>> dojazdAll;
map<string,int> timePassed;

void generatePathStops(string lineParameter, int timeParameter) {
	vector<Edge> output;
	string prevLine = "";
	string startName = B;
	int startTime = timeParameter;
	string prevName = "";

	cout << "Restoring path for " << lineParameter << "(" << cost[B]-1 << ", " << getTimeFromS(timeParameter) << "):\n";
	string v = B, l = lineParameter;
	int aktTime = timeParameter;
	while (v != A) {
		string tmpH = v;
		// cout << v << " " << getTimeFromS(aktTime) << "\n";
		for (auto i : dojazdAll[v]) {
			Edge aktEdge = input[i.second];
			string startV = aktEdge.start;
			string potentialLine = aktEdge.line;
			int potentialStartTime = aktEdge.timeA;
			if (potentialStartTime > aktTime) continue;
			// cout << "\t\tPOTENCJALNA " << getTimeFromS(potentialStartTime) << " " << getTimeFromS(aktTime) << "\n";
			if (potentialLine == l && cost[startV] == cost[v] && dojazd[startV][l].find(potentialStartTime) != dojazd[startV][l].end()) {
				// cout << "\tZMIANA " << getTimeFromS(potentialStartTime) << " " << getTimeFromS(aktTime) << "\n";
				v = startV;
				aktTime = potentialStartTime;
				break;
			}
			else if (cost[startV]+1 == cost[v] && potentialLine == l) {
				string betterLine;
				int betterTime=INT_MAX;
				for (auto it : dojazd[startV]) {
					if (it.second.size() == 0) continue;
					betterTime = (*it.second.begin());
					if (betterTime <= aktTime) {
						betterLine = it.first;
						break;
					}
				}
				if (betterLine == l) continue;

				if (l != prevLine) {
					output.pb({l,potentialStartTime,startTime,startName,0,startV});
					prevLine = l;
					startTime = (betterTime == INT_MAX ? aktTime : betterTime);
					startName = startV;
				}

				v = startV;
				aktTime = betterTime;
				l = betterLine;
				break;
			}
		}
		if (v == tmpH) break;
	}

	reverse(output.begin(),output.end());
	for (auto i : output) {
		cout << i.line << ": (" << i.start << ", " << getTimeFromS(i.timeA) << ") -> (" << i.dest << ", " << getTimeFromS(i.timeB) << ")\n";
	}
}

void aStarStops() {
	cout << "A-STAR TIME START\n";
	set<AStarElement> s;
	
	cost[A] = 0;

	timePassed[A] = T;
	s.insert({0,T,A,"Autobus Piekieł Bram"});

	while (s.size()) {
		auto akt = *s.begin(); s.erase(s.begin());
		// cout << "INSIDE " << akt.name << "\n";
		
		if (akt.name == B) {
			generatePathStops(akt.line, akt.time);
			return;
		}
		for (auto vertex : adj[akt.name]) {
			// cout << "FROM " << akt.name << " CHECKING " << vertex.dest << "\n";
			// cout << getTimeFromS(vertex.timeA) << " " << getTimeFromS(akt.time) << "\n";
			if (vertex.timeA < akt.time) continue;
			int newCost = akt.cost;
			if (dojazd[akt.name][vertex.line].find(vertex.timeA) == dojazd[akt.name][vertex.line].end()){
				newCost++;
			}


			if (newCost == cost[vertex.dest]) {
				dojazd[vertex.dest][vertex.line].insert(vertex.timeB);
				dojazdAll[vertex.dest].insert({vertex.timeB,vertex.edgeId});

				if (vertex.timeB < timePassed[vertex.dest]) {
					AStarElement searcher = {cost[vertex.dest], timePassed[vertex.dest], vertex.dest, ""};
					auto it = s.lower_bound(searcher);
					if (it != s.end() and (*it).name == vertex.dest) {
						s.erase(it);
						s.insert({cost[vertex.dest],vertex.timeB,vertex.dest,vertex.line});
						// cout << "INSERTED " << newCost << " " << getTimeFromS(vertex.timeB) << " " << vertex.dest << " " << vertex.line << "\n";
						timePassed[vertex.dest] = vertex.timeB;
					}
				}
			}
			if (newCost < cost[vertex.dest]) {
				// cout << "FINDING NEXT " << akt.name << " -> " << vertex.dest << "\n";
				AStarElement searcher = {cost[vertex.dest], timePassed[vertex.dest], vertex.dest, ""};
				auto it = s.lower_bound(searcher);
				if (it != s.end() and (*it).name == vertex.dest) {
					s.erase(it);
				}
				s.insert({newCost,vertex.timeB,vertex.dest,vertex.line});
				// cout << "INSERTED " << newCost << " " << getTimeFromS(vertex.timeB) << " " << vertex.dest << " " << vertex.line << "\n";

				cost[vertex.dest] = newCost;
				timePassed[vertex.dest] = vertex.timeB;

				dojazdAll[vertex.dest].clear();
				dojazd[vertex.dest].clear();
				dojazdAll[vertex.dest].insert({vertex.timeB,vertex.edgeId});
				dojazd[vertex.dest][vertex.line].insert(vertex.timeB);
			} 
			
		}

	}
}

int main() {
	read();
	getline(cin,A);
	getline(cin,B);
	getline(cin,MODE);
	getline(cin,Time);
	// cin >> A >> B >> MODE >> Time;
	cout << A << " " << B << " " << MODE << " " << Time << "\n";
	str = stringstream(Time);
	T = getMyTime();
	aStarStops();
	return 0;
}