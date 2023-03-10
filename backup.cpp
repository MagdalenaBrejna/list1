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

map<string,set<Edge>> adj;
map<string,pair<ld, ld>> geo;
map<string,int> cost;
map<string,int> parent;
vector<Edge> input;

stringstream str;
string::size_type sz;

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

int getMyLD() {
	ld res = 0;
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
		input.pb({linia, timeA, timeB, stopB, eId, stopA});
	}

	// for (auto it : adj) {
	// 	cout << it.first << ":\n";
	// 	for (auto i : it.second) {
	// 		cout << "\t" << i.line << " " << i.timeA << " " << i.timeB << " " << i.dest << "\n";
	// 	}
	// }
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
			// cout << aktName << " " << cost[aktName] << "+" << vertex.timeB << "-" << T << "-" << aktTime << "\n";
			int newTime = cost[aktName] + (vertex.timeB-T)-aktTime;
			if (newTime < cost[vertex.dest]) {
				// cout << aktName << " -> " << vertex.dest << ": " << newTime << " / " << cost[vertex.dest] << "\n";
				parent[vertex.dest] = vertex.edgeId;
				cost[vertex.dest] = newTime;
				pq.push({newTime,vertex.dest});
			}
		}
	}
	assert(cost[B] != INF);
	string akt = B;
	
	while (parent[akt] != -1) {
		Edge here = input[parent[akt]];
		cout << here.line << " (" << here.start << ", " << here.timeA << "), (" << here.dest << ", " << here.timeB << ")\n";
		akt = here.start;
	}
}

int main() {
	read();
	cin >> A >> B >> MODE >> Time;
	cout << A << " " << B << " " << MODE << " " << Time << "\n";
	str = stringstream(Time);
	T = getMyTime();
	dijkstra();

	return 0;
}