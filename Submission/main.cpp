#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <memory>
#include <array> 
#include <limits>
using namespace std;
string inFile = "input.csv";
string outFile = "output.csv";
struct Point{
    float x;
    float y; 
    float z;
};
struct OctNode{
    float min_x, min_y, min_z, max_x, max_y, max_z;
     array<unique_ptr<OctNode>, 8> children;
    vector<Point> pts;
   
     
     void BuildTree(OctNode & node, const vector<Point> & allPoints, vector<Point> & nodePoints, float  min_x, float  max_x, float  min_y, float  max_y, float  min_z, float  max_z, int depth) {
    if (depth == 0 || nodePoints.empty()) {
        return;
    }

    float mid_x = (min_x + max_x) / 2;
    float mid_y = (min_y + max_y) / 2;
    float mid_z = (min_z + max_z) / 2;
   //  cout<<min_x<<" "<<max_x<<" "<<min_y<<" "<<max_y<<" "<<min_z<<" "<<max_z;

    for (int i = 0; i < 8; i++) {
        OctNode *octC = new OctNode();
        node.children[i].reset(octC);
        
        float child_min_x = (i & 1) ? mid_x : min_x;
        float child_max_x = (i & 1) ? max_x : mid_x;
        float child_min_y = (i & 2) ? mid_y : min_y;
        float child_max_y = (i & 2) ? max_y : mid_y;
        float child_min_z = (i & 4) ? mid_z : min_z;
        float child_max_z = (i & 4) ? max_z : mid_z;
        
        vector<Point> childPoints;
        for (auto & point : nodePoints) {
            if (point.x >= child_min_x && point.x <= child_max_x && point.y >= child_min_y && point.y <= child_max_y && point.z >= child_min_z && point.z <= child_max_z) {
              //cout<<"here";
                childPoints.push_back(point);
            }
        }
          node.children[i]->pts = childPoints;
          //cout<<node.children[i] -> pts.size();
        BuildTree(*node.children[i], allPoints, childPoints, child_min_x, child_max_x, child_min_y, child_max_y, child_min_z, child_max_z, depth - 1);
    }
}
};


vector<Point> ReadInputCSV(){
    vector<Point> Points;
  
    ifstream file;
    file.open("input.csv");
    string line;
    string delimiter_char = ",";
    while(getline(file, line)){
        replace(line.begin(), line.end(), ',', ' '); 
            istringstream coords(line);
            Point pt;
            coords>>pt.x>>pt.y>>pt.z;
            Points.push_back(pt);
    }
    file.close();
    return Points;
}
void FindThresh(const vector<Point> & points, float & min_x, float & max_x, float & min_y, float & max_y, float & min_z, float & max_z){
    min_x = min_y = min_z = numeric_limits<float>::max();
     max_x = max_y = max_z = numeric_limits<float>::lowest();
     for(const auto & pt : points){
         
        min_x = min(pt.x, min_x);
        min_y = min(pt.y, min_y);
        min_z = min(pt.z, min_z);
        max_x = max(pt.x, max_x);
        max_y = max(pt.y, max_y);
        max_z = max(pt.z, max_z);
        
     }
}
void Filter(OctNode & node, vector<Point> & resPts){
     if(node.pts.empty()){
            return;
        }
    float CentroidX = 0.0f;
    float CentroidY = 0.0f;
    float CentroidZ = 0.0f;
    for(auto & allP : node.pts){
       
        CentroidX += allP.x;
        CentroidY += allP.y;
        CentroidZ += allP.z;
    }
   
   CentroidX /= static_cast<float>(node.pts.size());
   CentroidY /= static_cast<float>(node.pts.size());
   CentroidZ /= static_cast<float>(node.pts.size());
    Point np;
    np.x = CentroidX;
    np.y = CentroidY;
    np.z = CentroidZ;
    
    resPts.push_back(np);
   // cout<<resPts.size()<<'\n';
    for(int i = 0; i < node.children.size(); i++){
                if(node.children[i])
            Filter( *node.children[i], resPts);
    }

}
void ReturnData(vector<Point> & pts){
    ofstream file;
    file.open("output.csv");
  
    for(auto & pt : pts){
       
        file<<pt.x<<','<<pt.y<< ','<<pt.z<<'\n';
    }
    file.close();
}
int main() {
   
    vector<Point> points = ReadInputCSV();
    float min_x, max_x, min_y, max_y,min_z, max_z;
    FindThresh(points, min_x, max_x,min_y, max_y, min_z, max_z);
    OctNode parent;
    parent.pts = points;
      
    parent.min_x = min_x; parent.max_x = max_x; parent.min_y = min_y; parent.max_y = max_y; parent.max_z = max_z; parent.min_z = min_z;
   // parent.BuildTree(parent,parent.pts,min_x, max_x, min_y, max_y, min_z, max_z, 8);
   parent.BuildTree(parent, points, parent.pts, min_x, max_x, min_y, max_y, min_z, max_z, 8);
    vector<Point> resPts;
    Filter(parent, resPts);
    //cout<<resPts.size();
    ReturnData(resPts);
    return 0;
}