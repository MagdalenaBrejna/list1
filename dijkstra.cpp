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
string MODE;
int T;
string Time;

map<string,set<Edge>> adj;
map<string,int> cost;
map<string,int> parent;
map<int,Edge> input;

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
		cost[stopA] = INF;
		cost[stopB] = INF;
		input[eId] = {linia, timeA, timeB, stopB, eId, stopA};
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
			int newTime = (vertex.timeB-T);
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
	getline(cin,A);
	getline(cin,B);
	getline(cin,MODE);
	getline(cin,Time);
	cout << A << " " << B << " " << MODE << " " << Time << "\n";
	str = stringstream(Time);
	T = getMyTime();
	dijkstra();
	return 0;
}