#include <bits/stdc++.h>

using namespace std;
using ld = long double;
#define pb push_back

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
ld FASTEST_PACE = 0;

map<string,set<Edge>> adj;
map<string,pair<ld, ld>> geo;
map<string,int> cost;
map<string,int> parent;
map<int,Edge> input;
map<string,map<string,set<int>>> dojazd;
map<string,set<pair<int,int>,greater<pair<int,int>>>> dojazdAll;
map<string,int> timePassed;

stringstream str;

ld getDistance(ld Ax, ld Ay, ld Bx, ld By) {
	return sqrt((Bx-Ax)*(Bx-Ax) + (By-Ay)*(By-Ay));
}
ld getPace(ld dist, ld time) {
	return dist/time;
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
	res += getLD(tmp)*60*60;
	getline(str,tmp,':');
	res += getLD(tmp)*60;
	getline(str,tmp,',');
	res += getLD(tmp);
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
		FASTEST_PACE = max(FASTEST_PACE, getPace(getDistance(Ax,Ay,Bx,By), timeB-timeA));
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
		for (auto i : dojazdAll[v]) {
			Edge aktEdge = input[i.second];
			string startV = aktEdge.start;
			string potentialLine = aktEdge.line;
			int potentialStartTime = aktEdge.timeA;
			if (potentialStartTime > aktTime) continue;
			if (potentialLine == l && cost[startV] == cost[v] && dojazd[startV][l].find(potentialStartTime) != dojazd[startV][l].end()) {
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
		
		if (akt.name == B) {
			generatePathStops(akt.line, akt.time);
			return;
		}
		for (auto vertex : adj[akt.name]) {
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
						timePassed[vertex.dest] = vertex.timeB;
					}
				}
			}
			if (newCost < cost[vertex.dest]) {
				AStarElement searcher = {cost[vertex.dest], timePassed[vertex.dest], vertex.dest, ""};
				auto it = s.lower_bound(searcher);
				if (it != s.end() and (*it).name == vertex.dest) {
					s.erase(it);
				}
				s.insert({newCost,vertex.timeB,vertex.dest,vertex.line});

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

void generatePathTime() {
	assert(cost[B] != INF);
	string akt = B;
	
	string prevLine = "";
	string startName;
	int startTime;
	string prevName = "";
	vector<Edge> output;
	while (true) {
		Edge here = input[parent[akt]];
		if (here.line != prevLine) {
			if (prevLine != "") {
				Edge prevE = input[parent[prevName]];
				output.pb({prevLine,prevE.timeA,startTime,startName,0,prevE.start});
			}
			prevLine = here.line;
			startName = here.dest;
			startTime = here.timeB;
		}
		if (parent[here.start] == -1) break;
		prevName = akt;
		akt = here.start;
	}
	Edge prevE = input[parent[akt]];
	output.pb({prevLine,prevE.timeA,startTime,startName,0,prevE.start});
	
	reverse(output.begin(),output.end());
	for (auto i : output) {
		cout << i.line << ": (" << i.start << ", " << getTimeFromS(i.timeA) << ") -> (" << i.dest << ", " << getTimeFromS(i.timeB) << ")\n";
	}
}
ld h(string vertex) {
	pair<ld,ld> WspA = geo[vertex], WspB = geo[B];
	return getDistance(WspA.first,WspA.second,WspB.first,WspB.second)/FASTEST_PACE;
}
void aStarTime() {
	cout << "A-STAR START\n";
	priority_queue< pair<ld,string>, vector<pair<ld,string>>, greater<pair<ld,string>> > pq;
	map<string,ld> fScore;
	parent[A] = -1;
	cost[A] = 0;

	ld tmp = h(A);
	fScore[A] = tmp;
	pq.push({tmp,A});

	while (pq.size()) {
		pair<int,string> tmp = pq.top(); pq.pop();
		int aktTime = tmp.first;
		string aktName = tmp.second;
		if (aktTime > fScore[aktName]) continue;
		if (aktName == B) {
			generatePathTime();
			return;
		}
		for (auto vertex : adj[aktName]) {
			if (vertex.timeA < cost[aktName]+T) continue;
			int newTime = (vertex.timeB-T);
			if (newTime < cost[vertex.dest]) {
				parent[vertex.dest] = vertex.edgeId;
				cost[vertex.dest] = newTime;
				fScore[vertex.dest] = newTime + h(vertex.dest);
				pq.push({fScore[vertex.dest],vertex.dest});
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
	cout << A << " " << B << " " << MODE << " " << Time << "\n";
	str = stringstream(Time);
	T = getMyTime();
	if (MODE == "p") 
		aStarStops();
	else if (MODE == "t")
		aStarTime();
	return 0;
}