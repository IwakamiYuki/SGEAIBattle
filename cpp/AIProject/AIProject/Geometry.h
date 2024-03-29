#include <iostream>
#include <cstdio>
//#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <queue>
//#include <stack>
//#include <limits>
#include <sstream>
//#include <functional>
#include <complex>
using namespace std;

#define len(array)  (sizeof (array) / sizeof *(array))
#define rep(i, s, e) for(int i = s;i < e;i++)
#define rrep(i, e, s) for(int i = e;s <= i;i--)
#define vrange(v) v.begin(), v.end()
#define vrrange(v) v.rbegin(), v.rend()
#define vsort(v) sort(vrange(v))
#define vrsort(v) sort(vrrange(v))
#define arange(a) a, a + len(a)
#define asort(a) sort(arange(a))
#define arsort(a, t) sort(arange(a), greater<t>())
#define afill(a, v) fill(arange(a), v)
#define afill2(a, v, t) fill((t *)a, (t *)(a + len(a)), v)
#define fmax(a, b) (a < b? b : a)
#define fmin(a, b) (a > b? b : a)
#define fabs(a) (a < 0? -a : a)
typedef unsigned int ui;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> P;
const int INF = (int)1e9;
const int MOD = (int)1e9 + 7;
const double EPS = 1e-10;
//const int dx[] = {1, -1, 0, 0, 1, -1, -1, 1};
//const int dy[] = {0, 0, 1, -1, -1, -1, 1, 1};
//const int weight[] = {0,1,10,100,1000,10000,100000,1000000,10000000};
//priority_queue< int, vector<int>, greater<int> > q;


// http://d.hatena.ne.jp/otaks/20100929
// http://www.cplusplus.com/reference/complex/
typedef complex<double> p;
//XY座標
#define X real()
#define Y imag()


class Triangle{
private:
  //三角形の3点の座標
  p a, b, c;
  //三角形の3辺の長さ
  double ab, bc, ca;
  //三角形の3角の大きさ(ラジアン)
  //double A, B, C;
  double cosA, cosB, cosC, sinA, sinB, sinC;
public:
  //コンストラクタ(3つの点と辺と角度を初期化)
  Triangle(p p1, p p2, p p3){
	a = p1;
	b = p2;
	c = p3;
	ca = abs(c-a);
	bc = abs(b-c);
	ab = abs(a-b);
	cosA = getCosine(p2 - p1, p3 - p1);
	cosB = getCosine(p1 - p2, p3 - p2);
	cosC = getCosine(p1 - p3, p2 - p3);
	sinA = getSine(p2 - p1, p3 - p1);
	sinB = getSine(p1 - p2, p3 - p2);
	sinC = getSine(p1 - p3, p2 - p3);
  }
  //p1とp2の内積
  double dot(p p1, p p2){
	return p1.X*p2.X + p1.Y*p2.Y;
  }
  //p1とp2の外積
  double det(p p1, p p2){
	return p1.X*p2.Y - p2.X*p1.Y;
  }
  //3点が一直線上だったら三角形ではない
  bool isTriangle(){
	double t = det(a - c, b - c);
	return abs(t) < EPS? false : true;
  }
  //余弦定理からcosを求める関数
  double LawOfCosines(double a,double b, double c){
	return (b*b+c*c-a*a) / (2.0*b*c);
  }
  //外積からsineを求める
  double getSine(p p1, p p2){
	return abs(det(p1, p2) / (abs(p1)*abs(p2)));
  }
  //内積からcosineを求める
  double getCosine(p p1, p p2){
	return dot(p1, p2) / (abs(p1)*abs(p2));
  }
  //double circumscribedCircleRadius(){//外接円の半径を返す
  double getCR(){
	return ( bc / (2*sinA));
  }
  //p circumscribedCircle(){//外心(外接円の中心)の座標を返す
  //http://www.h6.dion.ne.jp/~jpe02_tn/ki-5.html
  p getCC(){ // get a circumcenter
	int a1 = 2*(b.X-a.X);
	int b1 = 2*(b.Y-a.Y);
	int c1 = a.X*a.X - b.X*b.X + a.Y*a.Y - b.Y*b.Y;
	int a2 = 2*(c.X-a.X);
	int b2 = 2*(c.Y-a.Y);
	int c2 = a.X*a.X - c.X*c.X + a.Y*a.Y - c.Y*c.Y;
	double x = (b1*c2 - b2*c1)*1.0 / (a1*b2 - a2*b1);
	double y = (c1*a2 - c2*a1)*1.0 / (a1*b2 - a2*b1);
	return p(x, y);
  }
  //http://www004.upp.so-net.ne.jp/s_honma/urawaza/area2.htm
  double getArea(){
	double s = 0;
	s += (a.X - b.X) * (a.Y + b.Y);
	s += (b.X - c.X) * (b.Y + c.Y);
	s += (c.X - a.X) * (c.Y + a.Y);
	return fabs(s) * 0.5;
  }
  //http://www.blackpawn.com/texts/pointinpoly/
  //線分abから見てp1, p2は同じ方向にあるか？
  bool SameSide(p p1, p p2, p a, p b){
    p cp1 = det(b-a, p1-a);
    p cp2 = det(b-a, p2-a);
    if (dot(cp1, cp2) >= 0) return true;
    else return false;
  }
  //点p0がa, b, cからなる三角形の内部にあるか？
  bool _PointInTriangle(p p0, p a, p b, p c){
    if (SameSide(p0,a, b,c) && SameSide(p0,b, a,c)
        && SameSide(p0,c, a,b)) return true;
    else return false;
  }
  bool PointInTriangle(p p0){
    return _PointInTriangle(p0, a, b, c);
  }

};

// 円クラス
class Circle{
private:
  //中心座標
  p c;
  //半径
  double r;
public:
  //コンストラクタ(3つの点と辺と角度を初期化)
  Circle(p center, double radius){
	c = center;
	r = radius;
  }
  //aは円の領域内（線上含む）か？
  bool inRegion(p a){
	return (a.X-c.X)*(a.X-c.X)+(a.Y-c.Y)*(a.Y-c.Y) <= r*r + EPS? true : false;
  }
};




//二次元座標(from 蟻本)
typedef struct _PT{
  double x, y;
  _PT() {}
  _PT(double x, double y) : x(x), y(y){
  }
  _PT operator + (_PT p){
	return _PT(x + p.x, y + p.y);
  }
  _PT operator - (_PT p){
	return _PT(x - p.x, y - p.y);
  }
  _PT operator * (double d){
	return _PT(d*x, d*y);
  }
  double dot(_PT p){ //pとの内積
	return x * p.x + y * p.y;
  }
  double det(_PT p){ //pとの外積
	return x * p.y - p.x * y;
  }
  double dist(_PT p){ //pとの距離の2乗
	return (x-p.x)*(x-p.x) + (y-p.y)*(y-p.y);
  }
  double abs(){
	return sqrt(norm());
  }
  double norm(){
	return x*x + y*y;
  }
  bool operator <(const struct _PT &e) const{
    return x == e.x? (y < e.y) : x < e.x;
  }
  bool operator >(const struct _PT &e) const{
    return x == e.x? (y > e.y) : x > e.x;
  }
} pt;
// 線分p1-p2上に点qがあるか判定
bool on_seg(pt p1, pt p2, pt q){
	return (p1 - q).det(p2 - q) == 0 && (p1 - q).dot(p2 - q) <= 0;
}
// 直線p1-p2とq1-q2の交点
pt intersection(pt p1, pt p2, pt q1, pt q2){
	return p1 + (p2 - p1) * ((q2 - q1).det(q1 - p1) / (q2 - q1).det(p2 - p1));
}
//http://www.prefield.com/algorithm/geometry/ccw.html
//与えられた三点 a, b, c を a → b → c と進む
int ccw(_PT a, _PT b, _PT c) {
  _PT bb = b - a, cc = c - a;;
  if (bb.det(cc) > 0)   return +1;       // counter clockwise
  if (bb.det(cc) < 0)   return -1;       // clockwise
  if (bb.dot(cc) < 0)     return +2;       // c--a--b on line
  if (bb.norm() < cc.norm()) return -2;       // a--b--c on line
  return 0;
}
double pdet(pt p1, pt p2){ //pとの外積
	return p1.x * p2.y - p1.x * p2.y;
}

// 線分クラス
typedef struct _LINE{
  pt a, b;
  _LINE(){}
  _LINE(pt x, pt y) : a(x), b(y){
  }
} line;
// http://tomiflu.hatenablog.com/entry/20121209/1355063106
// 2直線の交差判定
bool intersectLL(line &l, line &m) {
  return abs((l.b - l.a).det( m.b - m.a)) > EPS || // 平行ではない
	abs((l.b - l.a).det(m.a - l.a)) < EPS; // 同一直線上(オーバーラップ)
}
// 直線と線分の交差判定
bool intersectLS(line &l, line &s) {
  return (l.b - l.a).det(s.a - l.a) * // s.aはlの左側
	(l.b - l.a).det(s.b - l.a) < EPS; // s.bはlの右側
}
// 直線と点の交差判定
bool intersectLP(line &l, pt &p) {
  return abs((l.b - p).det(l.a - p)) < EPS;
}
// 2線分の交差判定
bool intersectSS(const line &s, const line &t) {
    return ccw(s.a, s.b, t.a)*ccw(s.a,s.b,t.b) <= 0 &&
        ccw(t.a,t.b,s.a)*ccw(t.a,t.b,s.b) <= 0;
}
// 線分と点の交差判定
bool intersectSP(line &s, pt &p) {
  return (s.a - p).abs() + (s.b - p).abs() - (s.b - s.a).abs() < EPS; // 三角不等式
}
//http://www.prefield.com/algorithm/geometry/distance.html
pt projection(line &l, pt &p) {
  double t = (p-l.a).dot(l.a-l.b) / (l.a-l.b).norm();
  return l.a + (l.a - l.b)*t;
}
pt reflection(line &l, pt &p) {
  return p + ((projection(l, p) - p) * 2);
}
double distanceLP(line &l, pt &p) {
  return (p - projection(l, p)).abs();
}
double distanceLL(line &l, line &m) {
  return intersectLL(l, m) ? 0 : distanceLP(l, m.a);
}
double distanceLS(line &l, line &s) {
  if (intersectLS(l, s)) return 0;
  return min(distanceLP(l, s.a), distanceLP(l, s.b));
}
//線分と点の距離
double distanceSP(line &s, pt &p) {
  pt r = projection(s, p);
  if (intersectSP(s, r)) return (r - p).abs();
  return fmin((s.a - p).abs(), (s.b - p).abs());
}
double distanceSS(line &s, line &t) {
  if (intersectSS(s, t)) return 0;
  return min(min(distanceSP(s, t.a), distanceSP(s, t.b)),
             min(distanceSP(t, s.a), distanceSP(t, s.b)));
}
//二直線の交点
pt crosspoint(line &l, line &m) {
  double A = (l.b - l.a).det(m.b - m.a);
  double B = (l.b - l.a).det(l.b - m.a);
  if (abs(A) < EPS && abs(B) < EPS) return m.a; // same line
  // if (abs(A) < EPS) assert(false); // !!!PRECONDITION NOT SATISFIED!!!
  return m.a + (m.b - m.a) * (B/A);
}


class Polygon{
private:
public:
  vector<pt> pts;
  int n;
  Polygon(){}
  Polygon(vector<pt> v){
	n = v.size();
	pts = v;
  }
  //凸包（蟻本ver）
  vector<pt> convex_hull(){
	vector<pt> qs(n * 2);
	int k = 0;
	vsort(pts);
	rep(i, 0, n){ //下側凸包の作成
	  while(k > 1 && (qs[k-1] - qs[k-2]).det(pts[i] - qs[k-1]) <= 0) k--;
	  qs[k++] = pts[i];
	}
	for(int i = n - 2, t = k; i >= 0; i--){ //上側凸包の作成
	  while(k > t && (qs[k-1] - qs[k-2]).det(pts[i] - qs[k-1]) <= 0) k--;
	  qs[k++] = pts[i];
	}
	qs.resize(k-1);
	n = k-1;
	pts = qs;
	return qs;
  }
  //点が図形を構成する順番に並んでないと駄目。並んでなければconvex_hullで矯正してから
  double getArea(){
	double s = 0;
	rep(i, 0, n-1) s += pts[i].det(pts[i+1]);
	s += pts[n-1].det(pts[0]);
	return fabs(s) * 0.5;
  }
  //http://www004.upp.so-net.ne.jp/s_honma/urawaza/area2.htm
  double _getArea(){
	double s = 0;
	rep(i, 0, n-1) s += (pts[i].x - pts[i+1].x) * (pts[i].y + pts[i+1].y);
	s += (pts[n-1].x - pts[0].x) *( pts[n-1].y + pts[0].y);
	return fabs(s) * 0.5;
  }
  //http://imagingsolution.net/math/calc_n_point_area/
  //点が時計回りか反時計回りに並んでないと駄目
  double getArea2(){
	double s = 0;
	rep(i, 0, n-1) s += pts[i].x * pts[i+1].y + pts[i+1].x * pts[i].y;
	s += pts[n-1].x * pts[0].y + pts[0].x * pts[n-1].y;
	return fabs(s) * 0.5;
  }
  //キャリパー法で最遠点対を求める。要convex_hull
  double maxDist(){
	vector<pt> qs = pts;
	double res = 0;
	if(n == 2) return qs[0].dist(qs[1]);
	int i = 0, j = 0;
	rep(k, 0, n){
	  if(qs[i] > qs[k]) i = k;
	  if(qs[j] < qs[k]) j = k;
	}
	int si = i, sj = j;
	while(i != sj || j != si){
	  res = fmax(res, qs[i].dist(qs[j]));
	  if((qs[(i+1) % n] - qs[i]).det(qs[(j+1) % n] - qs[j]) < 0) i = (i + 1) % n;
	  else j = (j + 1) % n;
	}
	return res;
  }
};

