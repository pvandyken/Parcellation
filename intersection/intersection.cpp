#include "intersection.h"

// ============ Producto Punto =============
const float dotProduct(const float a[], float *&b) {
	float c = 0;

	#pragma omp simd reduction(+:c)
	for(int i=0; i<3; i++)
		c += a[i]*b[i];

	return c;
}

// ============ Producto Cruz ==============
float* crossProduct(const float a[], const float b[]) {
	float *c = new float[3];
	int i=1, j=2;

	#pragma omp simd
	for(int k=0; k<3; k++) {
		c[k] = a[i]*b[j] - a[j]*b[i];

		i = (i + 1) % 3;
		j = (j + 1) % 3;
	}

	return c;
}

// =========== Función que calcula la intersección entre un rayo con un triángulo ====================
bool ray_triangle_intersection(const float ray_near[], const float ray_dir[], const float Points[][3], float &t) {

	const float eps = 0.000001;
	float edges[2][3];

	#pragma omp simd collapse(2)
	for(int i=0; i<2; i++) {
		for(int j=0; j<3; j++) {
			edges[i][j] = Points[i+1][j] - Points[0][j];
		}
	}

	float *pvec = crossProduct(ray_dir, edges[1]);
	const float det = dotProduct(edges[0], pvec);

	if(fabs(det) < eps) {
		delete[] pvec;
		return false;
	}

  	const float inv_det = 1. / det;

  	float tvec[3];
  	#pragma omp simd
  	for(int i=0; i<3; i++)
  		tvec[i] = ray_near[i] - Points[0][i];

  	const float u = dotProduct(tvec, pvec) * inv_det;
  	delete[] pvec;
  	
  	if((u < 0.) || (u > 1.))
  		return false;

  	float *qvec = crossProduct(tvec, edges[0]);
  	const float v = dotProduct(ray_dir, qvec) * inv_det;

  	if((v < 0.) || (u + v > 1.)) {
  		delete[] qvec;
  		return false;
  	}

  	t = dotProduct(edges[1], qvec) * inv_det;
  	delete[] qvec;

  	if(t < eps)
  		return false;

	return true;
}

vector<vector<float>> multiple_vertices(vector<vector<float>> &triangle) {

	vector<vector<float>> pt(6,vector<float>(3));
	//float **pt = new float*[6];
	//for(int i=0; i<6; i++)
		//pt[i] = new float[3];

	for(int i=0; i<3; i++) {
		pt[0][i] = triangle[0][i];
		pt[2][i] = triangle[1][i];
		pt[4][i] = triangle[2][i];

		pt[1][i] = (triangle[0][i] + triangle[1][i]) / 2.0;
		pt[3][i] = (triangle[1][i] + triangle[2][i]) / 2.0;
		pt[5][i] = (triangle[0][i] + triangle[2][i]) / 2.0;
	}

    return pt;
}

vector<vector<vector<float>>> multiple_triangles(vector<vector<vector<float>>> &triangles, int &len, const int polys[][3]) {
	vector<vector<vector<float>>> new_triangles(len*4,vector<vector<float>>(3,vector<float>(3))); 

	for(int i=0; i<len; i++) {
		vector<vector<float>> tri = multiple_vertices(triangles[i]);

		for(int j=0; j<4; j++) {
			//new_triangles[i*4 + j] = new float*[3];

			for(int k=0; k<3; k++) {
				//new_triangles[i*4 + j][k] = new float[3];

				for(int l=0; l<3; l++)
					new_triangles[i*4 + j][k][l] = tri[polys[j][k]][l];
			}
		}

	}
	len = len * 4;
	return new_triangles;
}

vector<vector<float>> triangle_interpolation(vector<vector<vector<float>>> &triangles, const int &N) {
	vector<vector<float>> centroid(int(pow(4,N)),vector<float>(3));
	int len = 1;
	int polys[4][3] = {{0,1,5},{1,2,3},{5,3,4},{1,3,5}};

	for(int i=0; i<N; i++)
		triangles = multiple_triangles(triangles, len, polys);

	for(int i=0; i<len; i++) {
		//centroid[i] = new float[3];

		for(int j=0; j<3; j++) {
			float sum = 0;

			#pragma omp simd reduction(+:sum)
			for(int k=0; k<3; k++)
				sum += triangles[i][k][j];

			centroid[i][j] = sum / 3.0;
		}
	}
	return centroid;
}

const bool getMeshAndFiberEndIntersection(vector<float> &fiberP0, vector<float> &fiberP1, const int &nPoints, const int &nPtsLine, const int &N, const int &npbp,
	vector<vector<float>> &index, const float &step, vector<vector<vector<bool>>> &cubeNotEmpty, const vector<vector<vector<vector<int>>>> &centroidIndex,
	const vector<vector<vector<vector<vector<float>>>>> &almacen, vector<vector<float>> &vertex, vector<vector<int>> &polygons, int &Ind, vector<float>&ptInt) {
	
    float dd[3];

    #pragma omp simd
    for(int i=0; i<3; i++)
    	dd[i] = (fiberP0[i] - fiberP1[i]) / float(npbp);

    vector<vector<int>> indexes;

    for(int i=0; i <= nPtsLine*npbp + npbp; i++) {

    	int I[3];

    	#pragma omp simd
    	for(int j=0; j<3; j++)
    		I[j] = ( (fiberP1[j] + i * dd[j]) - index[j][0] ) / step;

        #pragma omp simd collapse(3)
        for(int a=-1; a<2; a++) {
        	for(int b=-1; b<2; b++) {
        		for(int c=-1; c<2; c++) {

        			if(cubeNotEmpty[ I[0]+a ][ I[1]+b ][ I[2]+c ]) {
        				vector<int> INDEX(3);

        				int abc[3] = {a, b, c};

        				for(int k=0; k<3; k++)
        					INDEX[k] = I[k] + abc[k];

        				indexes.emplace_back(INDEX);
        			}
        		}
        	}
        }
    }

    if(indexes.empty())
    	return false;

    sort(indexes.begin(), indexes.end());
    indexes.erase( unique( indexes.begin(), indexes.end() ), indexes.end() );
    vector<vector<double>> listDist;

    for(const vector<int>& I : indexes) {
    	for(int u=0; u<(int)centroidIndex[I[0]][I[1]][I[2]].size(); u++) {
    		double cen[3], dist = 0;

    		#pragma omp simd reduction(+:dist)
    		for(int i=0; i<3; i++) {
    			cen[i] = almacen[I[0]][I[1]][I[2]][u][i];
    			dist += fabs(cen[i] - fiberP0[i]);
    		}

    		const int& c_index = centroidIndex[I[0]][I[1]][I[2]][u];
    		listDist.emplace_back((vector<double>){dist, (double)c_index});
    	}
    }
    sort(listDist.begin(), listDist.end());
    vector<int> listIndex;
    ptInt = vector<float>(3);
    
	for(const vector<double>& ind : listDist) {
		if(find(listIndex.begin(), listIndex.end(), (int)ind[1]) == listIndex.end())
			listIndex.emplace_back((int)ind[1]);
		else
			continue;

		float ray_dir[3], ray_near[3];

		#pragma omp simd
		for(int i=0; i<3; i++) {
			ray_dir[i] = fiberP1[i] - fiberP0[i];
			ray_near[i] = fiberP1[i];
		}
        
        vector<int> Triangle = polygons[(int)ind[1]];
        float Pts[3][3];

        #pragma omp simd collapse(2)
        for(int i=0; i<3; i++) {
        	for(int j=0; j<3; j++) {
        		Pts[i][j] = vertex[Triangle[i]][j];
        	}
        }

		float t;
		// ========== Verifica la intersección entre el rayo y el triángulo =============
        if(ray_triangle_intersection(ray_near, ray_dir, Pts, t)) {

        	#pragma omp simd
        	for(int i=0; i<3; i++)
        		ptInt[i] = fiberP1[i] + (fiberP1[i] - fiberP0[i])*t;

        	Ind = (int)ind[1];
        	return true;
        }
        else {
        	// ============= Ídem, pero con el rayo apuntando hacia el sentido contrario =============

        	float ray_invert[3];
        	#pragma omp simd
        	for(int i=0; i<3; i++)
        		ray_invert[i] = -ray_dir[i];

            if(ray_triangle_intersection(ray_near, ray_invert, Pts, t)) {

            	#pragma omp simd
	        	for(int i=0; i<3; i++)
	        		ptInt[i] = fiberP1[i] - (fiberP1[i] - fiberP0[i])*t;

	        	Ind = (int)ind[1];
	        	return true;
            }
        }
	}
	return false;
}

const bool getMeshAndFiberIntersection(vector<vector<float>> &fiber, const int &nPoints, const int &nPtsLine, const int &N, const int &npbp, vector<vector<float>> &index,
	const float &step, vector<vector<vector<bool>>> &cubeNotEmpty, const vector<vector<vector<vector<int>>>> &centroidIndex,
	const vector<vector<vector<vector<vector<float>>>>> &almacen, vector<vector<float>> &vertex, vector<vector<int>>&polygons,
	int &InInd, int &FnInd, vector<float> &InPtInt, vector<float> &FnPtInt) {

	bool findInt;

	findInt = getMeshAndFiberEndIntersection(fiber[0], fiber[1], nPoints, nPtsLine, N, npbp, index, step, cubeNotEmpty,
								   			 centroidIndex, almacen, vertex, polygons, InInd, InPtInt);

	if(!findInt) return false;

	findInt = getMeshAndFiberEndIntersection(fiber[nPoints-1], fiber[nPoints-2], nPoints, nPtsLine, N, npbp, index, step, cubeNotEmpty,
								   			 centroidIndex, almacen, vertex, polygons, FnInd, FnPtInt);

	if(!findInt) {
		return false;
	}

	return true;
}

void meshAndBundlesIntersection(vector<vector<float>> &vertex, const int &n_vertex, vector<vector<int>>&polygons, const int &n_polygons,
	const int &nBundles, vector<int> &nFibers, vector<vector<int>> &nPoints, vector<vector<vector<vector<float>>>> &Points, const int& nPtsLine,
	vector<vector<int>> &InTri, vector<vector<int>> &FnTri, vector<vector<vector<float>>> &InPoints,
	vector<vector<vector<float>>> &FnPoints, vector<vector<int>> &fib_index) {

	int N = 1;

	vector<float> mdbvs(n_polygons);
	#pragma omp parallel for schedule(dynamic)
	for(int i=0; i<n_polygons; i++) {
		vector<vector<float>> pts(3,vector<float>(3));
		pts[0] = vertex[polygons[i][0]];
		pts[1] = vertex[polygons[i][1]];
		pts[2] = vertex[polygons[i][2]];

		float dists[3] = {0,0,0};
		for(int k=0; k<3; k++) dists[0] += pow(pts[0][k] - pts[1][k], 2);
		for(int k=0; k<3; k++) dists[1] += pow(pts[1][k] - pts[2][k], 2);
		for(int k=0; k<3; k++) dists[2] += pow(pts[2][k] - pts[0][k], 2);
		for(int k=0; k<3; k++) dists[k] = sqrt(dists[k]);

		mdbvs[i] = *max_element(begin(dists), end(dists));
	}
	float mdbv = *max_element(begin(mdbvs), end(mdbvs));
	const float step = mdbv / pow(2, N + 1);

	float mdbp = 0; // maximum distance between points
	for(int i=0; i<nBundles; i++) {
		for(int j=0; j<nFibers[i]; j++) {

			float dist = 0;

			for(int k=0; k<3; k++)
				dist += pow(Points[i][j][0][k] - Points[i][j][1][k], 2);

			dist = sqrt(dist);
			if(dist > mdbp)
				mdbp = dist;

			dist = 0;
			for(int k=0; k<3; k++)
				dist += pow(Points[i][j][0][k] - Points[i][j][1][k], 2);

			dist = sqrt(dist);
			if(dist > mdbp)
				mdbp = dist;
		}
	}

	int npbp = mdbp / step; // number of points between points

	vector<float> vx(n_vertex);
	vector<float> vy(n_vertex);
	vector<float> vz(n_vertex);
	// ===== Find outermost vertex ===========
	#pragma omp parallel for schedule(dynamic)
	for(int i=0; i<n_vertex; i++) {
		vx[i] = vertex[i][0];
		vy[i] = vertex[i][1];
		vz[i] = vertex[i][2];
	}
	
	
	sort(vx.begin(), vx.end());
	sort(vy.begin(), vy.end());
	sort(vz.begin(), vz.end());

	const float minx = *vx.begin()  - (nPtsLine + 1) * mdbp - 4 * step;		   // coordenada x minima
	const float maxx = *vx.rbegin() + (nPtsLine + 1) * mdbp + 4 * step;		   // coordenada x maxima
	const float miny = *vy.begin()  - (nPtsLine + 1) * mdbp - 4 * step;		   // coordenada y minima
	const float maxy = *vy.rbegin() + (nPtsLine + 1) * mdbp + 4 * step;		   // coordenada y maxima
	const float minz = *vz.begin()  - (nPtsLine + 1) * mdbp - 4 * step;		   // coordenada z minima
	const float maxz = *vz.rbegin() + (nPtsLine + 1) * mdbp + 4 * step;		   // coordenada z maxima

	vector<vector<vector<vector<float>>>> Bundles;
	vector<vector<int>> new_nBundles;

	for(int i=0; i<nBundles; i++) {
		vector<vector<vector<float>>> new_Points;
		vector<int> new_nPoints;

		for(int j=0; j<nFibers[i]; j++) {

			const bool exi = ((*vx.begin() - mdbp - 2*step) <= Points[i][j][0][0]) && (Points[i][j][0][0] <= (*vx.rbegin() + mdbp + 2*step));
			const bool eyi = ((*vy.begin() - mdbp - 2*step) <= Points[i][j][0][1]) && (Points[i][j][0][1] <= (*vy.rbegin() + mdbp + 2*step));
			const bool ezi = ((*vz.begin() - mdbp - 2*step) <= Points[i][j][0][2]) && (Points[i][j][0][2] <= (*vz.rbegin() + mdbp + 2*step));

			const bool exf = ((*vx.begin() - mdbp - 2*step) <= Points[i][j][nPoints[i][j]-1][0]) && (Points[i][j][nPoints[i][j]-1][0] <= (*vx.rbegin() + mdbp + 2*step));
			const bool eyf = ((*vy.begin() - mdbp - 2*step) <= Points[i][j][nPoints[i][j]-1][1]) && (Points[i][j][nPoints[i][j]-1][1] <= (*vy.rbegin() + mdbp + 2*step));
			const bool ezf = ((*vz.begin() - mdbp - 2*step) <= Points[i][j][nPoints[i][j]-1][2]) && (Points[i][j][nPoints[i][j]-1][2] <= (*vz.rbegin() + mdbp + 2*step));

			if(exi && eyi && ezi && exf && eyf && ezf) {
				new_Points.emplace_back(Points[i][j]);
				new_nPoints.emplace_back(nPoints[i][j]);
			}
		}
		Bundles.emplace_back(new_Points);
		new_nBundles.emplace_back(new_nPoints);
	}

	for(int i=0; i<nBundles; i++) {
		nFibers[i] = Bundles[i].size();

		vector<vector<vector<float>>> pointsi(nFibers[i]);
		Points[i] = pointsi;
		vector<int> npointsi(nFibers[i]);
		nPoints[i] = npointsi;
		//Points[i] = new float**[nFibers[i]];
		//nPoints[i] = new int[nFibers[i]];

		for(int j=0; j<nFibers[i]; j++) {
			Points[i][j] = Bundles[i][j];
			nPoints[i][j] = new_nBundles[i][j];
		}
	}

	// ================ Obtiene la cantidad de intervalos por eje ==================
	const float mins[3] = {minx, miny, minz};
	const float maxs[3] = {maxx, maxy, maxz};
	int counts[3] = {0, 0, 0};

	for(int i=0; i<3; i++) {
		float ini = mins[i];
		while(ini < maxs[i]) {
			counts[i]++;
			ini += step;
		}
	}

	// ====== Generacion de intervalos (coordenadas de los vertices de cada cubo) ===================
	vector<vector<float>> index(3,vector<float>()); 

	for(int i=0; i<3; i++) {
		vector<float> indexi(counts[i]+1);
		index[i] = indexi;
		//index[i] = new float[counts[i] + 1];

		#pragma omp simd
		for(int j=0; j<counts[i] + 1; j++) {
			index[i][j] = mins[i] + j * step;
		}
	}

	vector<vector<float>> centroids(n_polygons*pow(4,N), vector<float>(3));

	#pragma omp parallel for schedule(dynamic)
	for(int i=0; i<n_polygons; i++) {
		vector<vector<vector<float>>> triangles(1,vector<vector<float>>(3,vector<float>()));
		//triangles[0] = new float*[3];

		for(int j=0; j<3; j++)
			triangles[0][j] = vertex[polygons[i][j]];

		vector<vector<float>> centroid = triangle_interpolation(triangles, N);

		for(int j=0; j<pow(4,N); j++) {
			for(int k=0; k<3; k++)
				centroids[i*pow(4,N) + j][k] = centroid[j][k];
		}
	}

	vector<vector<vector<vector<vector<float>>>>> almacen;
	vector<vector<vector<vector<int>>>> centroidIndex;
	vector<vector<vector<bool>>> cubeNotEmpty(counts[0],vector<vector<bool>>(counts[1],vector<bool>(counts[2])));

	almacen.resize(counts[0]);
	centroidIndex.resize(counts[0]);

	for(int ix = 0; ix < counts[0]; ix++) {

		almacen[ix].resize(counts[1]);
		centroidIndex[ix].resize(counts[1]);
		//cubeNotEmpty[ix] = new bool*[counts[1]];

		for(int iy = 0; iy < counts[1]; iy++) {

			almacen[ix][iy].resize(counts[2]);
			centroidIndex[ix][iy].resize(counts[2]);
			//cubeNotEmpty[ix][iy] = new bool[counts[2]];

			for(int iz = 0; iz < counts[2]; iz++) {
				cubeNotEmpty[ix][iy][iz] = false;
			}
		}
	}

	// =========================== En cada cubo almacena una lista de centroides y su indice ======================

	for(int i=0; i<n_polygons*pow(4,N); i++) {

		int I[3];

		#pragma omp simd
		for(int j=0; j<3; j++)
			I[j] = (centroids[i][j] - index[j][0]) / step;

		almacen[I[0]][I[1]][I[2]].emplace_back(centroids[i]);
		centroidIndex[I[0]][I[1]][I[2]].emplace_back(int(i / pow(4,N)));
		cubeNotEmpty[I[0]][I[1]][I[2]] = true;
	}

	// ===============================================================================================================

	InTri.resize(nBundles);
	FnTri.resize(nBundles);
	InPoints.resize(nBundles);
	FnPoints.resize(nBundles);
	fib_index.resize(nBundles);

	for(int i=0; i<nBundles; i++) {

		cout << "Bundle: " << i << "/" << nBundles;
		cout << ", Num. Fibers: " << nFibers[i] << endl;

		vector<int> listFibInd(nFibers[i]);
		vector<vector<int>> listTri(nFibers[i],vector<int>(2));
		vector<vector<vector<float>>> listPtInt(nFibers[i],vector<vector<float>>(2));
		#pragma omp parallel for schedule(auto)
		for(int j=0; j<nFibers[i]; j++) {
			bool findInt; // find intersection
			int InT, FnT; // initial and final triangle
			vector<float> InPtInt, FnPtInt; // Initial and Final point intersection

			findInt = getMeshAndFiberIntersection(Points[i][j], nPoints[i][j], nPtsLine, N, npbp, index, step, cubeNotEmpty,
				 						    centroidIndex, almacen, vertex, polygons, InT, FnT, InPtInt, FnPtInt);
			if(!findInt) {
				listFibInd[j]=-1;
				continue;
			}
			listFibInd[j]=j;
			listTri[j]=((vector<int>){InT, FnT});
			listPtInt[j]=((vector<vector<float>>){{InPtInt[0], InPtInt[1], InPtInt[2]}, {FnPtInt[0], FnPtInt[1], FnPtInt[2]}});

		}
		for(int j=0; j<(int)listTri.size(); j++) {
			if (listFibInd[j]!=-1){
				InTri[i].push_back(listTri[j][0]);
				FnTri[i].push_back(listTri[j][1]);
				InPoints[i].push_back(listPtInt[j][0]);
				FnPoints[i].push_back(listPtInt[j][1]);
			}
		}
		listFibInd.erase(remove_if(listFibInd.begin(), listFibInd.end(), [](int i){ return i == -1;}),listFibInd.end());
		fib_index[i] = listFibInd;
	}

	
}
