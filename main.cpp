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

const int INF = INT_MAX-24*60*60;
string A, B;
char MODE;
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

void generatePath() {
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
	if (MODE == 't') {
		pair<ld,ld> WspA = geo[vertex], WspB = geo[B];
		return getDistance(WspA.first,WspA.second,WspB.first,WspB.second)/SLOWEST_PACE;
	} else {
		return 0;
	}
}

void aStar() {
	cout << "A-STAR TIME START\n";
	priority_queue<pair<ld,string>, vector<pair<ld,string>>, greater<pair<ld,string>> > pq;
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
			generatePath();
			return;
		}
		for (auto vertex : adj[aktName]) {
			if (vertex.timeA < cost[aktName]+T) continue;
			// cout << "hmm " << cost[aktName] << "+" << vertex.timeB << "-" << T << "-" << aktTime << "\n";
			int newTime = (vertex.timeB-T);
			if (newTime < cost[vertex.dest]) {
				// cout << aktName << "->" << vertex.dest << ": " << newTime << "<" << cost[vertex.dest] << " " << newTime + h(vertex.dest) << "\n";
				parent[vertex.dest] = vertex.edgeId;
				cost[vertex.dest] = newTime;
				fScore[vertex.dest] = newTime + h(vertex.dest);
				pq.push({fScore[vertex.dest],vertex.dest});
			}
		}
	}
}

void aStarStops() {
	cout << "A-STAR TIME START\n";
	priority_queue< pair<pair<ld,int>,pair<string,vector<int>>>, vector<pair<pair<ld,int>,pair<string,vector<int>>>>, greater<pair<pair<ld,int>,pair<string,vector<int>>>> > pq;

	map<string,ld> fScore;
	map<string,set<string>> arrivedUsing;
	parent[A] = -1;
	cost[A] = 0;

	ld tmp = h(A);
	fScore[A] = tmp;
	pq.push({{tmp,T},{A,vector<int>(0)}});

	while (pq.size()) {
		auto tmp = pq.top(); pq.pop();
		int aktCost = tmp.first.first;
		int aktTime = tmp.first.second;
		string aktName = tmp.second.first;
		vector<int> aktPath = tmp.second.second;
		if (aktCost > fScore[aktName]) continue;
		if (aktName == B) {
			cout << aktCost << "\n";
			for (auto it : aktPath) {
				cout << input[it].line << " " << input[it].start << "->" << input[it].dest << ":\n";
				for (auto it2 : arrivedUsing[input[it].start]) {
					cout << it2 << " ";
				}
				cout << "\n";
			}
			generatePath();
			return;
		}
		for (auto vertex : adj[aktName]) {
			if (vertex.timeA < aktTime) continue;
			vector<int> herePath = aktPath;
			int newCost = aktCost;
			if (arrivedUsing[aktName].find(vertex.line) == arrivedUsing[aktName].end()) {
				if (aktName != A) {
					newCost++;
				}
				herePath.pb(vertex.edgeId);
			}

			if (newCost < cost[vertex.dest]) {
				parent[vertex.dest] = vertex.edgeId;
				cost[vertex.dest] = newCost;
				fScore[vertex.dest] = newCost + h(vertex.dest);
				pq.push({{fScore[vertex.dest],vertex.timeB},{vertex.dest,herePath}});
			} 
			if (newCost <= cost[vertex.dest]) {
				arrivedUsing[vertex.dest].insert(vertex.line);
			}
		}

	}
} // Dla każdego v vertor trzymający ścieżki do niego, bo z linii 270 ostra nierówność blokuje poprawną aktualizację

void dijkstra() {
	cout << "DIJKSTRA START\n";
	cost[A] = 0;
	parent[A] = -1;
	priority_queue< pair<int,string>, vector<pair<int,string>>, greater<pair<int,string>> > pq;
	pq.push({0,A});

	while (pq.size()) {
		pair<int,string> tmp = pq.top(); pq.pop();
		int aktTime = tmp.first;
		string aktName = tmp.second;
		for (auto vertex : adj[aktName]) {
			if (vertex.timeA < aktTime+T) continue;
			int newTime = cost[aktName] + (vertex.timeB-T)-aktTime;
			if (newTime < cost[vertex.dest]) {
				parent[vertex.dest] = vertex.edgeId;
				cost[vertex.dest] = newTime;
				pq.push({newTime,vertex.dest});
			}
		}
	}
	generatePath();
}

int main() {
	read();
	cin >> A >> B >> MODE >> Time;
	cout << A << " " << B << " " << MODE << " " << Time << "\n";
	str = stringstream(Time);
	T = getMyTime();
	// dijkstra();
	aStarStops();
	return 0;
}