#include <algorithm>
#include <fstream>
#include <vector>
#include "preferLearn.h"
#include "sphere.h"
#include "adaptive.h"
#include "cube.h"
#include "GeoGreedy.h"
#include "DMM.h"
#include "graphDP.h"
#include "sweepDP.h"
#include "biSearch.h"
#include "evaluate.h"

int get_current_best_pt(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec);
vector<int> generate_S(point_set_t* P, vector<int>& C_idx, int s, int current_best_idx, int& last_best, vector<int>& frame, vector<point_t*>& ext_vec, int cmp_option);

point_set_t *normalize_points(vector<vector<double>> &points, vector<int> smallerBetter){

    int dim = points[0].size();
    point_set_t* norm_points = alloc_point_set(points.size());
    vector<double> max_vals(dim, 0), min_vals(dim, INF);

    for(int i = 0; i < points.size(); ++i){
        point_t* point = alloc_point(dim, i); // dim = dim, id = i
        for(int j = 0; j < dim; ++j){
            double val = points[i][j];
            max_vals[j] = std::max(max_vals[j], val);
            min_vals[j] = std::min(min_vals[j], val);
            point->coord[j] = val;
        }
        norm_points->points[i] = point;
    }

    for(int i = 0; i < points.size(); ++i){
        for(int j = 0; j < dim; ++j){
            double norm_val = (norm_points->points[i]->coord[j] - min_vals[j]) / (max_vals[j] - min_vals[j]);
            if(smallerBetter[j]) norm_val = 1 - norm_val;
            norm_points->points[i]->coord[j] = norm_val;
        }
    }

    return norm_points;
}

class AlgorithmRunner {
public:
    AlgorithmRunner(vector<vector<double>> &candidates, vector<int> &smallerBetter, int cmp_option) {
        this->cmp_option = cmp_option;
        s = 2;
        prune_option = RTREE;
        current_best_idx = -1;
        last_best = -1;
        rr = 1;
        points_norm = normalize_points(candidates, smallerBetter); // new points
        skyline = skyline_point(points_norm); // does not copy points

        int dim = skyline->points[0]->dim;
        for(int i = 0; i < skyline->numberOfPoints; ++i){
            C_idx.push_back(i);
        }
        for(int i = 0; i < dim; ++i){
            point_t *e = alloc_point(dim);
            for(int j = 0; j < dim; ++j){
                e->coord[j] = i == j ? -1 : 0;
            }
            ext_vec.push_back(e);
        }

        current_best_idx = get_current_best_pt(skyline, C_idx, ext_vec);
        
    }

    ~AlgorithmRunner() {
        for(int i = 0; i < ext_vec.size(); ++i)
            release_point(ext_vec[i]);
        release_point_set(skyline, false);
        release_point_set(points_norm, true);
    }

    vector<int> nextPair(){
        sort(C_idx.begin(), C_idx.end());
        S = generate_S(skyline, C_idx, s, current_best_idx, last_best, frame, ext_vec, cmp_option);
        if(S.size() != 2){
            printf("invalid option numbers");
            exit(0);
        }

        vector<int> indices;
        for(int i = 0; i < S.size(); ++i){
            int id = skyline->points[C_idx[S[i]]]->id;
            indices.push_back(id);
        }

        return indices;
    }

    void choose(int option){
        int max_i = option - 1;
        last_best = current_best_idx;
        current_best_idx = C_idx[S[max_i]];

        for(int i = 0; i < S.size(); ++i){
            if(max_i == i) continue;
            point_t *tmp = sub(skyline->points[C_idx[S[i]]], skyline->points[C_idx[S[max_i]]]);
            C_idx[S[i]] = -1;
            point_t *new_ext_vec = scale(1 / calc_len(tmp), tmp);
            release_point(tmp);
            ext_vec.push_back(new_ext_vec);
        }

        int j = 0;
        for(int i = 0; i < C_idx.size(); ++i){
            if(C_idx[i] >= 0)
                C_idx[j++] = C_idx[i];
        }
        C_idx.resize(j);
        if(C_idx.size() > 1){
            rtree_pruning(skyline, C_idx, ext_vec, rr, NO_BOUND, HYPER_PLANE);
        }
    }

    int numLeftPoints() {
        return C_idx.size();
    }

    vector<int> getCandidatesIndices() {
        sort(C_idx.begin(), C_idx.end());
        vector<int> indices;
        for(int i = 0; i < C_idx.size(); ++i){
            int id = skyline->points[C_idx[i]]->id;
            indices.push_back(id);
        }
        return indices;
    }

private:
    int cmp_option;
    int s;
    int prune_option;
    point_set_t *points_norm;
    point_set_t *skyline;
    vector<int> C_idx;
    vector<point_t *> ext_vec;
    int current_best_idx;
    int last_best;
    vector<int> frame;
    double rr;
    vector<int> S;
};

vector<vector<double>> readConvexHullVertices(){
    ifstream ifs("ext_pt");
    vector<vector<double>> vertices;
    if(!ifs.is_open()) return vertices;
    int dim, size;
    ifs >> dim >> size;
    for(int i = 0; i < size; ++i){
        double sum = 0;
        vector<double> point;
        double val;
        for(int j = 0; j < dim; ++j){
            ifs >> val;
            sum += val;
            point.push_back(val);
        }
        if(sum > 0.9) vertices.push_back(point);
    }
    return vertices;
}

class SevenAlgorithmRunner {
public:
    SevenAlgorithmRunner(vector<vector<double>> &candidates, vector<int> &smallerBetter, int algorithmIdx, int k=15) {
        points_norm = normalize_points(candidates, smallerBetter); // new points
        skyline = skyline_point(points_norm); // does not copy points
        this->k = k;
        int dim = skyline->points[0]->dim;
        for(int i = 0; i < skyline->numberOfPoints; ++i){
            C_idx.push_back(i);
        }
        for(int i = 0; i < dim; ++i){
            point_t *e = alloc_point(dim);
            for(int j = 0; j < dim; ++j){
                e->coord[j] = i == j ? -1 : 0;
            }
            ext_vec.push_back(e);
        }

        switch (algorithmIdx) {
            case 4 :
                current_best_points = DMM(skyline, k);
            break;
            
            case 5 :
                current_best_points = cube(k,skyline);
            break;
            
            case 6 :
                current_best_points = sphere(skyline, k);
            break;
            
            case 7 :
                current_best_points = graphDP(skyline, k);
            break;
            
            case 8 :
                current_best_points = biSearch(skyline, k);
            break;
            
            case 9 :
                current_best_points = sweepDP(skyline, k);
            break;
            
            case 10 :
                current_best_points = IncGreedy(k, skyline, current_best_points);
            break;
            
            default :
                current_best_points = DMM(skyline, k);
        }
        mrr = evaluateLP(skyline, current_best_points, 0);
    }
    vector<int> getIndices() {
        indices.clear();
        for(int i = 0; i < current_best_points->numberOfPoints; ++i){
            int id = current_best_points->points[i]->id;
            indices.push_back(id);
        }
        return indices;
    }
    double getMrr() {
        return mrr;
    }

    void globalBest_localBest(vector<double> D) {
        point_t* u = alloc_point(D.size());
        bool isValid = false;

        while (!isValid)
        {
            for(int i = 0; i < D.size(); i++)
                u->coord[i] = D[i];

            bool allZero = true;
            bool hasNegative = false;

            for(int i = 0; i < D.size(); i++)
            {
                if(!isZero(u->coord[i]))
                    allZero = false;
                if(u->coord[i] < 0)
                    hasNegative = true;
            }
            if (!allZero && !hasNegative)
                isValid = true;
        }
        point_t* globalBest = findFavorite(skyline, u);
        point_t* localBest = findFavorite(current_best_points, u);
        this->globalBestId = globalBest->id;
        this->localBestId = localBest->id;
        this->regret_ratio = 1 - dot_prod(localBest,u)/dot_prod(globalBest,u);
    }
    int get_globalBestId() {
        return globalBestId;
    }
    int get_localBestId() {
        return localBestId;
    }
    double get_regret_ratio() {
        return regret_ratio;
    }
    void Next_IncGreedy() {
        current_best_points = IncGreedy(k, skyline, current_best_points);
        mrr = evaluateLP(skyline, current_best_points, 0);
    }

    ~SevenAlgorithmRunner() {
        for(int i = 0; i < ext_vec.size(); ++i)
            release_point(ext_vec[i]);
        release_point_set(skyline, false);
        release_point_set(current_best_points, false);
        release_point_set(points_norm, true);
    }

public:
    int k; int globalBestId; int localBestId;
    double mrr; double regret_ratio;
    point_set_t *points_norm;
    point_set_t *skyline;
    point_set_t *current_best_points = alloc_point_set(0);
    vector<int> indices;
    vector<int> C_idx;
    vector<point_t *> ext_vec;
};

#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_module) {

    register_vector<int>("VectorInt");
    register_vector<double>("VectorDouble");
    register_vector<vector<double>>("VecVecDouble");

    class_<AlgorithmRunner>("AlgorithmRunner")
        .constructor<vector<vector<double>> &, vector<int> &, int>()
        .function("nextPair", &AlgorithmRunner::nextPair)
        .function("choose", &AlgorithmRunner::choose)
        .function("numLeftPoints", &AlgorithmRunner::numLeftPoints)
        .function("getCandidatesIndices", &AlgorithmRunner::getCandidatesIndices)
        ;
    class_<SevenAlgorithmRunner>("SevenAlgorithmRunner")
        .constructor<vector<vector<double>> &, vector<int> &, int, int>()
        .function("getIndices", &SevenAlgorithmRunner::getIndices)
        .function("getMrr", &SevenAlgorithmRunner::getMrr)
        .function("Next_IncGreedy", &SevenAlgorithmRunner::Next_IncGreedy)
        .function("get_globalBestId", &SevenAlgorithmRunner::get_globalBestId)
        .function("get_localBestId", &SevenAlgorithmRunner::get_localBestId)
        .function("get_regret_ratio", &SevenAlgorithmRunner::get_regret_ratio)
        .function("globalBest_localBest", &SevenAlgorithmRunner::globalBest_localBest);

    emscripten::function("readConvexHullVertices", &readConvexHullVertices);
}

#else

int main() {
    demo();
    return 0;
}

#endif
